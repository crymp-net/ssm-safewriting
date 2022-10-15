#include "CryEngine/IGameFramework.h"
#include "CryEngine/IConsole.h"
#include "CryEngine/ISystem.h"

#include "SafeWritingAPI.h"

SSystemGlobalEnvironment *gEnv;

extern "C" __declspec(dllexport) void Init(SafeWritingAPI* pAPI)
{
	gEnv = pAPI->GetIGameFramework()->GetISystem()->GetGlobalEnvironment();

	gEnv->pConsole->RegisterFloat("mp_circleJump", 1.0f, 0, "Enable circle jumping as in 5767");
}

int __stdcall DllMain(void* hinstDLL, unsigned long fdwReason, void* lpvReserved)
{
	return 1;
}
