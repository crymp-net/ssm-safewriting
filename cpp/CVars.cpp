#include "CVars.h"
#include "Shared.h"
#include <Windows.h>
#undef GetCommandLine
#pragma region CVars

char SvMaster[255]="gamespy.com";
extern float *EXPLOSIVE_REMOVAL_TIME;

void CmdExplosiveRemovalTime(IConsoleCmdArgs *pArgs) {
	if (pArgs->GetArgCount()>1)
	{
		const char* val = pArgs->GetArg(1);
		if (val) {
			float fval = 30000;
			if (sscanf(val, "%f", &fval)==1) {
				DWORD old;
				VirtualProtect((void*)EXPLOSIVE_REMOVAL_TIME, 16, PAGE_READWRITE, &old);
				*EXPLOSIVE_REMOVAL_TIME = fval;
				VirtualProtect((void*)EXPLOSIVE_REMOVAL_TIME, 16, old, 0);
			}
		}
	}
	pScriptSystem->BeginCall("print");
	char buff[50];
	sprintf(buff, "$0    sv_explosive_removal_time = $6%f", *EXPLOSIVE_REMOVAL_TIME);
	pScriptSystem->PushFuncParam(buff);
	pScriptSystem->EndCall();
}

void CmdChangeTimeOfDay(IConsoleCmdArgs *pArgs){
	if (pArgs->GetArgCount()>1)
	{
		const char *to=pArgs->GetCommandLine()+strlen(pArgs->GetArg(0))+1;
		if(ICVar *cVar=pConsole->GetCVar("e_time_of_day")){
			cVar->ForceSet(to);
		}
	}
}
void CmdFSetCvar(IConsoleCmdArgs *pArgs){
	if (pArgs->GetArgCount()>2)
	{
		const char *what=pArgs->GetArg(1);
		const char *val=pArgs->GetArg(2);
		if(ICVar *cVar=pConsole->GetCVar(what)){
			cVar->ForceSet(val);
		}
	}
}
void CmdSvMaster(IConsoleCmdArgs *pArgs){
	if (pArgs->GetArgCount()>1)
	{
		const char *to=pArgs->GetCommandLine()+strlen(pArgs->GetArg(0))+1;
		const char *eqpos=strstr(to,"=");
		if(eqpos) to=eqpos+1;
		while((*to) == ' ')
			to++;
		strcpy(SvMaster, to);
	}
	pScriptSystem->BeginCall("print");
	char buff[50];
	sprintf(buff,"$0    sv_master = $6%s",SvMaster);
	pScriptSystem->PushFuncParam(buff);
	pScriptSystem->EndCall();
}
void CmdRPCInfo(IConsoleCmdArgs *pArgs) {
	pScriptSystem->BeginCall("printf");
	extern unsigned long rpc_mem_usage, rpc_active_instances;
	pScriptSystem->PushFuncParam("RPC memory usage: %d bytes");
	pScriptSystem->PushFuncParam((int)rpc_mem_usage);
	pScriptSystem->EndCall();

	pScriptSystem->BeginCall("printf");
	pScriptSystem->PushFuncParam("RPC active instances: %d");
	pScriptSystem->PushFuncParam((int)rpc_active_instances);
	pScriptSystem->EndCall();

	IterateClients();
}
void CmdDoHooks(IConsoleCmdArgs *pArgs){
	pSystem=pGameFramework->GetISystem();
	pConsole=pSystem->GetIConsole();
	pScriptSystem=pSystem->GetIScriptSystem();
	pGameRules=pGameFramework->GetIGameRulesSystem()->GetCurrentGameRules();
	if(!Hooked){
		hook(SendChatMessageAddr,SendChatMessage);
		hook(RenamePlayerAddr,RenamePlayer);
		hook(OnClientConnectAddr,OnClientConnect);
#ifdef SPOOFS
		hook( Handle_SvRequestChatMessageAddr,Handle_SvRequestChatMessage);
		hook( Handle_SvRequestHitAddr,Handle_SvRequestHit );
		hook( Handle_SvRequestSpectatorModeAddr,Handle_SvRequestSpectatorMode );
		hook( Handle_SvRequestChangeTeamAddr,Handle_SvRequestChangeTeam );
		hook( Handle_SvRequestRenameAddr,Handle_SvRequestRename );
		hook( Handle_SvRequestDropItemAddr,Handle_SvRequestDropItem );
#endif
		hook( CWeapon_OnShoot,Hook_CWeaponOnShoot );
		//hook( CNanoSuit_SetSuitEnergy, Hook_CNanoSuitSetSuitEnergy );
#ifdef UNLOCK
		hook( INETCHNL_DISCONNECT, Hook_Disconnect );
#endif
		Hooked=true;
	}
}
void SvMaxPlayers(ICVar* cvar){	
	int value=cvar->GetIVal();
	if(value<2){
		value=2;
		cvar->Set(value);
		return;
	}
}
#pragma endregion