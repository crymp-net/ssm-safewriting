#include <string>
#include <new>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "CryEngine/ISystem.h"

#include "FileCheck.h"
#include "ILauncher.h"
#include "ILauncherTask.h"

static ILauncher *g_pLauncher = NULL;
static TFileCheckCallback g_callback = NULL;
static void *g_callbackParam = NULL;

struct FileCheckCallbackTask : public ILauncherTask
{
	INetChannel *pNetChannel;
	std::string file;

	FileCheckCallbackTask()
	: pNetChannel(),
	  file()
	{
	}

	void Run() override  // always executed in main thread
	{
		g_callback(pNetChannel, file.c_str(), g_callbackParam);
	}
};

// this function is called instead of kicking the player
static void FileCheckHook(INetChannel *pNetChannel, const char *file)  // executed in ServerProbe thread
{
	if (pNetChannel && file)
	{
		if (g_pLauncher)
		{
			FileCheckCallbackTask *pTask = new FileCheckCallbackTask();
			pTask->pNetChannel = pNetChannel;
			pTask->file = file;

			g_pLauncher->DispatchTask(pTask);
		}
	}
}

static void FillMem(void *address, const void *data, size_t length)
{
	DWORD oldProtection;
	VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &oldProtection);

	memcpy(address, data, length);

	VirtualProtect(address, length, oldProtection, &oldProtection);
}

bool FileCheckInit(TFileCheckCallback callback, void *param)
{
	HMODULE hExe = GetModuleHandleA(NULL);
	ILauncher::TGetFunc pGetILauncher = (ILauncher::TGetFunc) GetProcAddress(hExe, "GetILauncher");
	if (pGetILauncher)
	{
		g_pLauncher = pGetILauncher();
	}

	if (! g_pLauncher)
	{
		return false;
	}

	HMODULE hCryNetwork = GetModuleHandleA("CryNetwork.dll");
	if (! hCryNetwork)
	{
		return false;
	}

	if (g_pLauncher->GetGameVersion() != 6156)
	{
		return false;
	}

	g_callback = callback;
	g_callbackParam = param;

#ifdef _WIN64
	// 64-bit code
	unsigned char code[] = {
		0x48, 0x8B, 0xCE,                                            // mov rcx, rsi
		0x48, 0x8B, 0xD5,                                            // mov rdx, rbp
		0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0  <-- address of target function
		0xFF, 0xD0,                                                  // call rax
		0x90,                                                        // nop
		0x90                                                         // nop
	};

	// insert address of target function
	void *pFunc = &FileCheckHook;
	memcpy(&code[8], &pFunc, 8);

	FillMem((PBYTE) hCryNetwork + 0x14FF44, code, sizeof code);
#else
	// 32-bit code
	unsigned char code[] = {
		0xFF, 0x73, 0x0C,              // push dword ptr ds:[ebx+0xC]
		0x56,                          // push esi
		0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0x0  <-- address of target function
		0xFF, 0xD0,                    // call eax
		0x90,                          // nop
		0x90,                          // nop
		0x90                           // nop
	};

	// insert address of target function
	void *pFunc = &FileCheckHook;
	memcpy(&code[5], &pFunc, 4);

	FillMem((PBYTE) hCryNetwork + 0x30F3D, code, sizeof code);
#endif

	return true;
}
