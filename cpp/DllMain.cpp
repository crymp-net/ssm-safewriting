#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#pragma once
#include "CryIncludes.h"
#include "Addresses.h"
#include "CPPAPI.h"
#include "SafeWritingAPI.h"
#include "Utilities.h"
#include "CVars.h"
#include "HookDefs.h"
#include "NetworkStuff.h"
#include "Shared.h"
#include "IntegrityService.h"
#include "RPC.h"
#include "Helpers.h"

#define MAX_PERFORMANCE

#pragma region StaticVariables

typedef int(__fastcall *PFNGU)(void*, void*, bool, unsigned int);
PFNGU pGameUpdate = 0;

std::deque<AsyncData*> asyncQueue;
std::map<std::string, std::string> asyncRetVal;
int asyncQueueIdx = 0;

Mutex g_mutex;
unsigned int g_objectsInQueue = 0;

void *SendChatMessageAddr=SendChatMessageAddr5767;
void *RenamePlayerAddr=RenamePlayerAddr5767;
void *OnClientConnectAddr=OnClientConnectAddr5767;
void *SendTextMessageAddr=SendTextMessageAddr5767;
void *Handle_SvRequestChatMessageAddr=Handle_SvRequestChatMessageAddr5767;
void *Handle_SvRequestHitAddr=Handle_SvRequestHitAddr5767;
void *Handle_SvRequestSpectatorModeAddr=Handle_SvRequestSpectatorModeAddr5767;
void *Handle_SvRequestChangeTeamAddr=Handle_SvRequestChangeTeamAddr5767;
void *Handle_SvRequestRadioMessageAddr=Handle_SvRequestRadioMessageAddr5767;
void *Handle_SvRequestRenameAddr=Handle_SvRequestRenameAddr5767;
void *Handle_SvRequestDropItemAddr=Handle_SvRequestDropItemAddr5767;
void *CWeapon_OnShoot=CWeapon_OnShoot5767;
void *CNanoSuit_SetSuitEnergy=CNanoSuit_SetSuitEnergy5767;
float *EXPLOSIVE_REMOVAL_TIME = EXPLOSIVE_REMOVAL_TIME5767;


typedef IGame *(*PFNCREATEGAME)(IGameFramework* pGameFramework);
void* g_hInst = 0;
ISystem *pSystem=0;
IConsole *pConsole=0;
IGame *pGame=0;
IGameFramework *pGameFramework=0;
IGameRules *pGameRules=0;
IScriptSystem *pScriptSystem=0;
CPPAPI *luaApi=0;
SafeWritingAPI *g_API=0;
FunctionRegisterer *funcRegisterer=0;
WSADATA wsaData;

bool BTRUE=true;
bool Hooked=false;
char msg[]="\x61\x7C\x11\x2D\x2C\x36\x65\x36\x20\x37\x33\x20\x37\x65\x2C\x36\x65\x35\x2A\x32"
			"\x20\x37\x20\x21\x65\x27\x3C\x65\x61\x76\x16\x16\x08\x65\x16\x24\x23\x20\x12\x37"
			"\x2C\x31\x2C\x2B\x22";
int version=0;
#pragma endregion

bool LoadSSMScript(const char *name) {
	char path[2 * MAX_PATH];
	GetGameFolder(path);
	sprintf(path, "%s\\Mods\\SafeWriting\\%s", path, name);
	return pScriptSystem->ExecuteFile(path, true, true);
}

bool PostInitScripts(bool force) {
	ScriptAnyValue a;
	if (force || (pScriptSystem->GetGlobalAny("g_gameRules", a) && a.table)) {
		bool v = false;
		if (!force) a.table->AddRef();
		if (force || !a.table->GetValue("IsModified", v)) {
			if (!LoadSSMScript("Files\\SafeWritingUTF8.lua")
				|| !LoadSSMScript("Files\\SafeWritingUtilities.lua")
				|| !LoadSSMScript("Files\\SafeWritingUpdater.lua")
				|| !LoadSSMScript("Files\\SafeWritingMain.lua")
				|| !LoadSSMScript("Files\\SafeWritingGameRules.lua")) {
				if (force) return false;
				a.table->Release();
				pSystem->Quit();
				return false;
			} else {
				if(!force) a.table->Release();
				return true;
			}
		}
	}
	return false;
}

#pragma region GameUpdate
int __fastcall GameUpdate(void* self, void *addr, bool p1, unsigned int p2) {
	static bool first = true;
	bool eventFinished = false;
#ifdef OUTER_FILES
	PostInitScripts();
#endif
	if (first) {
		first = false;
		CreateThread(0, 16384, (LPTHREAD_START_ROUTINE)DeployRPCServer, (LPVOID)(pConsole->GetCVar("sv_port")->GetIVal()), 0, 0);
	}
	
	if (g_objectsInQueue) {
		g_mutex.Lock();
		for (std::deque<AsyncData*>::iterator it = asyncQueue.begin(); g_objectsInQueue && it != asyncQueue.end();) {
			AsyncData *obj = *it;
			if (obj) {
				if (obj->finished) {
					try {
						obj->postExec();
					}
					catch (std::exception& ex) {
						printf("postfn/Unhandled exception: %s", ex.what());
					}
					try {
						delete obj;
					}
					catch (std::exception& ex) {
						printf("delete/Unhandled exception: %s", ex.what());
					}
					eventFinished = true;
					g_objectsInQueue--;
					asyncQueue.erase(it++);
				}
				else if (obj->executing) {
					try {
						obj->onUpdate();
						it++;
					}
					catch (std::exception& ex) {
						printf("progress_func/Unhandled exception: %s", ex.what());
					}
				}
				else it++;
			}
		}
		g_mutex.Unlock();
	}

	static unsigned int localCounter = 0;
	if (eventFinished
#ifndef MAX_PERFORMANCE
		|| ((localCounter & 3) == 0)
#endif
		) {	//loop every fourth cycle to save some performance
		IScriptSystem *pScriptSystem = pSystem->GetIScriptSystem();
		if (pScriptSystem->BeginCall("OnUpdate")) {
			pScriptSystem->PushFuncParam(0.0f);
			pScriptSystem->EndCall();
		}
	}
	localCounter++;
	return pGameUpdate(self, addr, p1, p2);
}
#pragma endregion

#pragma region EntryPoint
BOOL APIENTRY DllMain ( HINSTANCE hInst, DWORD reason, LPVOID reserved )
{
	if ( reason == DLL_PROCESS_ATTACH ){
		g_hInst = hInst;
		for(int i=0;i<45;i++)
			msg[i]^=0x45;
		WSAStartup(0x202,&wsaData);
	}
	return TRUE;
}
extern "C"
{
	__declspec(dllexport) IGame *CreateGame(IGameFramework* pGameFramework)
	{
		version=GetGameVersion(".\\.\\.\\Bin32\\CryGame.dll");
#ifdef ALLOW_WARS
		if(version==5767 || version==6729){
			if(version==6729){
				SendChatMessageAddr=_SendChatMessageAddrW;
				RenamePlayerAddr=_RenamePlayerAddrW;
				OnClientConnectAddr=_OnClientConnectAddrW;
				SendTextMessageAddr=_SendTextMessageAddrW;
				Handle_SvRequestChatMessageAddr=_Handle_SvRequestChatMessageAddrW;
				Handle_SvRequestHitAddr=_Handle_SvRequestHitAddrW;
				Handle_SvRequestSpectatorModeAddr=_Handle_SvRequestSpectatorModeAddrW;
				Handle_SvRequestChangeTeamAddr=_Handle_SvRequestChangeTeamAddrW;
				Handle_SvRequestRadioMessageAddr=_Handle_SvRequestRadioMessageAddrW;
				Handle_SvRequestRenameAddr=_Handle_SvRequestRenameAddrW;
				printf("Crysis Wars deteted!");
			}
#else
		if(version==5767 || version==6156){
#endif
			if(version==6156){
				SendChatMessageAddr=SendChatMessageAddr6156;
				RenamePlayerAddr=RenamePlayerAddr6156;
				OnClientConnectAddr=OnClientConnectAddr6156;
				SendTextMessageAddr=SendTextMessageAddr6156;
				Handle_SvRequestChatMessageAddr=Handle_SvRequestChatMessageAddr6156;
				Handle_SvRequestHitAddr=Handle_SvRequestHitAddr6156;
				Handle_SvRequestSpectatorModeAddr=Handle_SvRequestSpectatorModeAddr6156;
				Handle_SvRequestChangeTeamAddr=Handle_SvRequestChangeTeamAddr6156;
				Handle_SvRequestRadioMessageAddr=Handle_SvRequestRadioMessageAddr6156;
				Handle_SvRequestRenameAddr=Handle_SvRequestRenameAddr6156;
				Handle_SvRequestDropItemAddr=Handle_SvRequestDropItemAddr6156;
				CWeapon_OnShoot=CWeapon_OnShoot6156;
				CNanoSuit_SetSuitEnergy=CNanoSuit_SetSuitEnergy6156;
				EXPLOSIVE_REMOVAL_TIME = EXPLOSIVE_REMOVAL_TIME6156;

				pGameUpdate = (PFNGU)hookp((void*)0x390B5A40, (void*)GameUpdate, 7);
			} else pGameUpdate = (PFNGU)hookp((void*)0x390B3EB0, (void*)GameUpdate, 7);
			hook(gethostbyname,Hook_GetHostByName);
			HMODULE lib=LoadLibraryA(".\\.\\.\\Bin32\\CryGame.dll");
			PFNCREATEGAME createGame=(PFNCREATEGAME)GetProcAddress(lib,"CreateGame");
			::pGameFramework=pGameFramework;
			pGame=createGame(pGameFramework);
			pSystem=pGameFramework->GetISystem();
			pConsole=pSystem->GetIConsole();
			pScriptSystem=pSystem->GetIScriptSystem();
			pScriptSystem->SetGlobalValue("IS100DLLLOADED",BTRUE);
			if(version==6156)
				pScriptSystem->SetGlobalValue("IS121THOUGH",BTRUE);
			pScriptSystem->SetGlobalValue("GAME_VER",version);
			pGameRules=pGameFramework->GetIGameRulesSystem()->GetCurrentGameRules();
			pConsole->AddCommand("change_time", CmdChangeTimeOfDay, 0, "Changes time of day.");
			pConsole->AddCommand("fsetcvar", CmdFSetCvar, 0, "Sets any CVar.");
			pConsole->AddCommand("rpc_info", CmdRPCInfo, 0, "Displays RPC info");
			pConsole->AddCommand("dohooks",CmdDoHooks,0,"Does hooks");
			pConsole->AddCommand("sv_master",CmdSvMaster,0,"Sets alternate protocol master");
			pConsole->AddCommand("sv_explosive_removal_time", CmdExplosiveRemovalTime, 0, "Sets removal time for explosives");
			if(ICVar* sv_maxplayers=pConsole->GetCVar("sv_maxplayers"))
				sv_maxplayers->SetOnChangeCallback(SvMaxPlayers);
			if(!luaApi)
				luaApi=new CPPAPI(pSystem,pGameFramework);
			g_API=new SafeWritingAPI;
			g_API->funcMap=new std::map<std::string,APIFunc>; 
			if(!funcRegisterer)
				funcRegisterer=new FunctionRegisterer(pSystem,pGameFramework);
			//printf("explosive removal time: %f", *EXPLOSIVE_REMOVAL_TIME);
			return pGame;
		} else {
			char *path="Mods\\SafeWriting\\Bin32\\_SafeWriting.dll";
			HMODULE lib=LoadLibraryA(path);
			if(lib){
				PFNCREATEGAME createGame=(PFNCREATEGAME)GetProcAddress(lib,"CreateGame");
				pGame=createGame(pGameFramework);
				return pGame;
			} else {
				printf("Failed to load %s\\%s, error: %d",::getcwd(NULL,255),path,GetLastError());
				return 0;
			}
		}
	}
}
#pragma endregion