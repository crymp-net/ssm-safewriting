#pragma once

#define OFFICIAL_BUILD
//comment for release builds
//#define PRERELEASE_BUILD
//if enabled, mutexes will be used to ensure safe threading
#define THREAD_SAFE		
//if enabled, OnUpdate will be called in Lua only when async event finishes instead of every frame
#define MAX_PERFORMANCE	

//#define DO_ASYNC_CHECKS

#pragma comment(lib, "wininet")
#pragma comment(lib, "wldap32")
#pragma comment(lib, "shell32")
#pragma comment(lib, "ws2_32")

#ifdef _WIN64
#ifndef IS64
#define IS64  // 64-bit build
#endif
#endif

#ifdef IS64
typedef unsigned long long uintptr_t;
#define ARCH_BITS 64
#else
typedef unsigned int uintptr_t;
#define ARCH_BITS 32
#endif

#ifndef getField
#define getField(type,base,offset) (*(type*)(((unsigned char*)base)+offset))
#define GET_FIELD getField
#endif

#define MAX_ASYNC_QUEUE 6

struct MENU_SCREEN {
	void *PTR0;	//Virtual table pointer
	void *PTR1;	//Actual first item
};
typedef void* VOIDPTR;
enum EMENUSCREEN
{
	MENUSCREEN_FRONTENDSTART,
	MENUSCREEN_FRONTENDINGAME,
	MENUSCREEN_FRONTENDLOADING,
	MENUSCREEN_FRONTENDRESET,
	MENUSCREEN_FRONTENDTEST,
	MENUSCREEN_FRONTENDSPLASH,
	MENUSCREEN_COUNT
};
typedef MENU_SCREEN* MENU_SCREEN_PTR;
template<int T,int NPtr,class Q>
struct OFFSET_STRUCT {
	unsigned char dummy[T];
	Q arr[NPtr];
};
typedef OFFSET_STRUCT<0x68, 6, MENU_SCREEN_PTR> FLASH_OBJ_32_6156;
typedef OFFSET_STRUCT<0x80, 6, MENU_SCREEN_PTR> FLASH_OBJ_64_6156;
struct GAME_32_6156 {
	unsigned char dummy[0x30];
	FLASH_OBJ_32_6156 *pFlashObj;
};


#define REGISTER_GAME_OBJECT(framework, name, script)\
	{\
		IEntityClassRegistry::SEntityClassDesc clsDesc;\
		clsDesc.sName = #name;\
		clsDesc.sScriptFile = script;\
		struct C##name##Creator : public IGameObjectExtensionCreatorBase\
		{\
			C##name *Create()\
			{\
				return new C##name();\
			}\
			void GetGameObjectExtensionRMIData( void ** ppRMI, size_t * nCount )\
			{\
				C##name::GetGameObjectExtensionRMIData( ppRMI, nCount );\
			}\
		};\
		static C##name##Creator _creator;\
		framework->GetIGameObjectSystem()->RegisterExtension(#name, &_creator, &clsDesc);\
	}

typedef void(__cdecl *PFNSETUPDATEPROGRESSCALLBACK)(void *);		//MapDownloader::SetUpdateProgressCallback
typedef int(__cdecl *PFNDOWNLOADMAP)(const char *, const char *, const char *);
typedef void(__cdecl *PFNCANCELDOWNLOAD)();

#if _MSC_VER <= 1600  // VS2010 and older
#define OLD_MSVC_DETECTED  // almost no C++11 support
#endif

#include <string>

void ToggleLoading(const char *text, bool loading = true, bool reset = true);

#define hookp trampoline
void* trampoline(void *oldfn, void *newfn, int sz, int bits = ARCH_BITS);
void hook(void *src,void *dest);
void unhook(void *src);

int getGameVer(const char*);
void getGameFolder(char*);
std::string fastDownload(const char *url);
bool autoUpdateClient();

std::string SignMemory(void *addr, int len, const char *nonce, bool raw=false);
std::string SignFile(const char *name, const char *nonce, bool raw=false);
int FileDecrypt(const char *name, char **out);
void FileEncrypt(const char *name, const char *out);

#define CRYPT_KEY {10,32,95,44, 210,235,00,73, 77,68,42,03, 01,254,100,200}
