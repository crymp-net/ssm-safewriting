#include "Shared.h"
#include <Windows.h>
#include <stdio.h>
#include <map>
#include <WinInet.h>
#include <time.h>
#include <IGameFramework.h>
#include <ICryPak.h>
#include "Crypto.h"

template <int T> struct StaticBuffer{
	char content[T];
};

std::map<void*,StaticBuffer<25> > hData;

void cpymem(void *a, void *b, int sz) {
	for (int i = 0; i < sz; i++) {
		((char*)a)[i] = ((char*)b)[i];
	}
}

void print_mem(void *mem, int len) {
	for (int i = 0; i < len; i++) {
		printf("%02X ", ((unsigned char*)mem)[i] & 255);
	}
	printf("\n\n");
}

void* trampoline(void *oldfn, void *newfn, int sz, int bits) {
	/*
	let's make sure sz isn't less than minimum required for hook, if yes, fill in default values
	it's 15 bytes in most of cases for x64 and 5 or 7 bytes for x86
	most of x64 procedures in ASM begin like this:
		48 89 5c 24 08	 mov	 QWORD PTR[rsp + 8], rbx
		48 89 74 24 10	 mov	 QWORD PTR[rsp + 16], rsi
		57				 push	 rdi
		48 83 ec 20		 sub	 rsp, 32; 00000020H
	which makes it 15 bytes, for x86 you gotta check disassembly of beginning of function to see
	how many bytes we can cut so it's more than 5 and includes whole instructions
	*/
	if (bits == 64 && sz < 12) sz = 15;
	else if (bits == 32 && sz < 5) sz = 7;
	unsigned char *ptr_old = (unsigned char*)oldfn;
	unsigned char *ptr_new = (unsigned char*)newfn;
	unsigned char *cave = (unsigned char*)malloc(sz + 64);
	unsigned char *IP = cave;	//instruction pointer representation
	unsigned char *IP_Dest = ptr_old + sz;
	memset(cave, 0x90, sz + 64); //put some NOPs for safety first


	DWORD flags;
	VirtualProtect(cave, 4096, PAGE_EXECUTE_READWRITE, &flags);
	VirtualProtect(ptr_old, 4096, PAGE_EXECUTE_READWRITE, &flags);

	//Copy first sz bytes of original function to cave
	cpymem(IP, ptr_old, sz);	//Weird, but MSVC crashes on memcpy here... gotta use this cpymem :-/
	IP += sz;

	//Calculate relative jump address
	uintptr_t jmpSz = (IP_Dest)-(IP + 5);
	if (bits == 32) {
		*IP = 0xE9; IP++;	// 0E9h = JMP
		memcpy(IP, &jmpSz, sizeof(uintptr_t)); IP += sizeof(uintptr_t);
	} else if (bits == 64) {
		//RAX is safe to use, 64bit __fastcall uses RCX, RDX, R8, R9 + stack on Windows

		// MOVABS RAX, uint64_t
		*IP = 0x48; IP++;	//048h = MOVABS
		*IP = 0xB8; IP++;	//0B8h = RAX
		memcpy(IP, &IP_Dest, sizeof(void*));
		IP += sizeof(void*);

		//JMPABS RAX
		*IP = 0xFF; IP++;	//0FFh = JMPABS	
		*IP = 0xE0;			//0E0h = RAX
	}

	//Rewrite original function:
	IP = ptr_old;
	memset(IP, 0x90, sz);	//put some NOPs for safety first
	if (bits == 32) {
		jmpSz = (ptr_new - (IP + 5));
		*IP = 0xE9; IP++;
		memcpy(IP, &jmpSz, sizeof(jmpSz));
	} else if (bits == 64) {
		*IP = 0x48; IP++;
		*IP = 0xB8; IP++;
		memcpy(IP, &ptr_new, sizeof(void*));
		IP += sizeof(void*);
		*IP = 0xFF; IP++;
		*IP = 0xE0;
	}
	return (void*)(cave);
}
int getGameVer(const char *file){
    FILE *f=fopen(file,"rb"); 
    int c=0; 
    if(f){ 
		fseek(f,60,SEEK_SET); 
		if(!feof(f)) 
			c=fgetc(f); 
		fclose(f); 
		if(c==16) 
			return 6156; 
		else if(c==248) 
			return 5767; 
		else if(c=='\b')
			return 6729;
		else
			return 0;
    }
	return -1;
}
void hook(void* original_fn,void* new_fn){
	char *src=(char*)original_fn;
	char *dest=(char*)new_fn;
	DWORD fl=0;
	VirtualProtect(src,32,PAGE_READWRITE,&fl);
#ifdef IS64
	StaticBuffer<25> w;
	memcpy(w.content,src,12);
	hData[original_fn]=w;
	//x64 jump construction:
	src[0]=(char)0x48;	// 48 B8 + int64 = MOV RAX, int64
	src[1]=(char)0xB8;
	memcpy(src+2,&dest,8);
	src[10]=(char)0xFF;	// FF E0 = JMP RAX (absolute jump)
	src[11]=(char)0xE0;
#else
	unsigned long jmp_p=(unsigned long)((dest-src-5)&0xFFFFFFFF);
	StaticBuffer<25> w;
	memcpy(w.content,src,5);
	hData[original_fn]=w;
	//x86 jump construction:
	src[0]=(char)0xE9;	// E9 + int32 = JMP int32 (relative jump)
	memcpy(src+1,&jmp_p,4);
#endif
	VirtualProtect(src,32,fl,&fl);
}
void unhook(void *original_fn){
	DWORD fl=0;
	VirtualProtect(original_fn,32,PAGE_READWRITE,&fl);
	StaticBuffer<25> w=hData[original_fn];
#ifdef IS64
	memcpy(original_fn,w.content,12);
#else
	memcpy(original_fn,w.content,5);
#endif
	VirtualProtect(original_fn,32,fl,&fl);
}

std::string fastDownload(const char *url){
	HINTERNET hSession,hUrl;
	hSession = InternetOpen(NULL, 0, NULL, NULL, 0);
    hUrl = InternetOpenUrl(hSession, url, NULL, 0, 0, 0);
	DWORD readBytes = 0;
	std::string data="";
	char buffer[8192];
	do {
		::InternetReadFile(hUrl,buffer,8192,&readBytes);
		if(readBytes>0)
			data += std::string(buffer, buffer+readBytes);
	} while(readBytes!=0);

	InternetCloseHandle(hUrl);
    InternetCloseHandle(hSession);
	return data;
}

bool autoUpdateClient(){
	char cwd[MAX_PATH], params[MAX_PATH];
	getGameFolder(cwd);
	sprintf(cwd,"%s\\SfwClFiles\\",cwd);
	sprintf_s(params,"\"%s\" \"%s?%d\" \"%s\"","update","http://crymp.net/dl/client.zip",(int)time(0),cwd);
	//MessageBoxA(0, params, 0, 0);
	SHELLEXECUTEINFOA info;
	ZeroMemory(&info,sizeof(SHELLEXECUTEINFOA));
	info.lpDirectory=cwd;
	info.lpParameters=params;
	info.lpFile="MapDownloader.exe";
	info.nShow=SW_SHOW;
	info.cbSize=sizeof(SHELLEXECUTEINFOA);
	info.fMask=SEE_MASK_NOCLOSEPROCESS;
	info.hwnd=0;
	//MessageBoxA(0,cwd,0,0);
	if (!ShellExecuteExA(&info)) {
		char buffer[500];
		sprintf(buffer,"\nFailed to start auto updater, error code %d\n", GetLastError());
		MessageBoxA(0,buffer,0,MB_OK|MB_ICONERROR);
		return false;
	}
	return true;
}

std::string SignMemory(void *addr, int len, const char *nonce, bool raw) {
	unsigned char *buffer = new unsigned char[len + 128];
	if (!buffer) return "00000000000000000000000000000000";
	memcpy(buffer, nonce, 16);
	DWORD flags = 0;
	if(!VirtualProtect(addr, len + 128, PAGE_EXECUTE_READ, &flags)) return "00000000000000000000000000000000";
	memcpy(buffer+16, addr, len);
	VirtualProtect(addr, len + 128, flags, 0);
	unsigned char digest[32];
	
	std::string out = "";
	sha256(buffer, len + 16, digest);
	delete[] buffer;
	if (raw) {
		for(int i=0;i<32;i++)
			out += digest[i];
		return out;
	}
	for (int i = 0; i < 32; i++) {
		static char bf[4];
		sprintf(bf, "%02X", digest[i] & 255);
		out += bf;
	}
	return out;
}
std::string SignFile(const char *name, const char *nonce, bool raw) {
	char *contents = 0;
	int len = FileDecrypt(name, &contents);
	unsigned char digest[32];
	std::string out = "";
	if (len) {
		memcpy(contents + len, nonce, 16);
		sha256((unsigned char*)contents, len + 16, digest);
		if (raw) {
			for (int i = 0; i<32; i++)
				out += digest[i];
		} else {
			for (int i = 0; i < 32; i++) {
				static char bf[4];
				sprintf(bf, "%02X", digest[i] & 255);
				out += bf;
			}
		}
		for (int i = 0; i < len; i++) {
			contents[i] = rand() & 0xFF;
		}
		delete[] contents;
	}
	return out;
}
void getGameFolder(char *cwd) {
	GetModuleFileNameA(0, cwd, MAX_PATH);
	std::vector<int> pos;
	for (int i = 0, j = strlen(cwd); i < j; i++) {
		if (cwd[i] == '\\')
			pos.push_back(i);
	}
	if (pos.size() >= 2)
		cwd[pos[pos.size() - 2]] = 0;
}
int FileDecrypt(const char *name, char **out) {
	char cwd[MAX_PATH], path[2 * MAX_PATH];
	getGameFolder(cwd);
	sprintf(path, "%s\\Mods\\sfwcl\\%s", cwd, name);
	FILE *f = fopen(path, "rb");
	int ret = 0;
	if (f) {

		fseek(f, 0, SEEK_END);
		long len = ftell(f);
		fseek(f, 0, SEEK_SET);
		unsigned char *mem = new unsigned char[len * 2 + 64];
		memset(mem, 0, len * 2 + 64);
		fread(mem, 1, len, f);
		fclose(f);
		if (mem[0] == 0) {
			AES_ctx ctx; memset(&ctx, 0, sizeof(ctx));
			uint8_t key[] = CRYPT_KEY;
			AES_init_ctx_iv(&ctx, key, (uint8_t*)mem + 1);
			AES_CBC_decrypt_buffer(&ctx, mem + 17, len - 17);
			memcpy(mem, mem + 17, len - 17);
			*out = (char*)mem;
		}
		else *out = (char*)mem;
		ret = strlen(*out);
	} else *out = 0;

	return ret;
}
void FileEncrypt(const char *name, const char *out) {
	char cwd[MAX_PATH], path[2*MAX_PATH], outpath[2*MAX_PATH];
	getGameFolder(cwd);
	sprintf(path, "%s\\Mods\\sfwcl\\%s", cwd, name);
	sprintf(outpath, "%s\\Mods\\sfwcl\\%s", cwd, out);
	//MessageBoxA(0, path, outpath, 0);
	FILE *f = fopen(path, "rb");
	if (f) {
		fseek(f, 0, SEEK_END);
		long len = ftell(f);
		fseek(f, 0, SEEK_SET);
		unsigned char *mem = new unsigned char[len * 2];
		int remaining = 16 - (len) % 16;
		memset(mem, remaining, len * 2);

		fread(mem, 1, len, f);
		mem[len] = 0;
		fclose(f);
		unsigned char iv[16];
		for (int i = 0; i < 16; i++) {
			iv[i] = rand() & 255;
		}
		if (mem[0] != 0) {
			AES_ctx ctx; memset(&ctx, 0, sizeof(ctx));
			uint8_t key[] = CRYPT_KEY;
			AES_init_ctx_iv(&ctx, key, iv);
			AES_CBC_encrypt_buffer(&ctx, mem, len + remaining);
			f = fopen(outpath, "wb");
			fputc(0, f);
			fwrite(iv, 1, 16, f);
			fwrite(mem, 1, len + remaining, f);
			fclose(f);
		}
	}
}