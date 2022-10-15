#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "CryEngine/IGameFramework.h"
#include "CryEngine/ISystem.h"

#include "FileCheck.h"
#include "SafeWritingAPI.h"

SSystemGlobalEnvironment* gEnv = NULL;
SafeWritingAPI *g_pAPI = NULL;
bool g_isInitialized = false;
std::string g_callbackFuncName;

static void OnFileMismatch(INetChannel *pNetChannel, const char *file, void *param)
{
	int channelID = gEnv->pGame->GetIGameFramework()->GetGameChannelId(pNetChannel);

	g_pAPI->BeginFuncCall(g_callbackFuncName.c_str());
	g_pAPI->PushFuncParam(channelID);
	g_pAPI->PushFuncParam(file);
	g_pAPI->CallFunction();
}

static void DoFileCheckInit()
{
	if (g_isInitialized)
	{
		return;
	}

	const char *callbackFuncName;
	g_pAPI->GetArg(1, callbackFuncName);

	g_callbackFuncName = callbackFuncName;

	if (g_callbackFuncName.empty())
	{
		return;
	}

	if (FileCheckInit(OnFileMismatch, NULL))
	{
		CryLogAlways("[FileCheck] Init done");
		g_isInitialized = true;
	}
	else
	{
		CryLogAlways("[FileCheck] Init error");
	}
}

extern "C" DLL_EXPORT void Init(SafeWritingAPI *pAPI)
{
	if (g_pAPI)
	{
		return;
	}

	g_pAPI = pAPI;

	// CryEngine global environment
	gEnv = g_pAPI->GetIGameFramework()->GetISystem()->GetGlobalEnvironment();

	g_pAPI->RegisterLuaFunc("FileCheckInit", DoFileCheckInit);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}
