#pragma once
#include <IGameFramework.h>
#include <ISystem.h>
#include <I3DEngine.h>
#include <IGameRulesSystem.h>
#include <IConsole.h>
#include <string.h>
#include "Addresses.h"
#include "HookDefs.h"

#pragma region CVarsDefinitions
void CmdChangeTimeOfDay(IConsoleCmdArgs *pArgs);
void CmdExplosiveRemovalTime(IConsoleCmdArgs *pArgs);
void CmdFSetCvar(IConsoleCmdArgs *pArgs);
void CmdRPCInfo(IConsoleCmdArgs *pArgs);
void CmdDoHooks(IConsoleCmdArgs *pArgs);
void CmdSvMaster(IConsoleCmdArgs *pArgs);
void SvMaxPlayers(ICVar* cvar);
extern IConsole *pConsole;
extern IGameRules *pGameRules;
extern IGameFramework *pGameFramework;
extern ISystem *pSystem;
extern IScriptSystem *pScriptSystem;
extern bool Hooked;
#pragma endregion