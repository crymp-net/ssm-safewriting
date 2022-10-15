#pragma once
#include <WinSock2.h>
#include <IGameFramework.h>
#include <I3DEngine.h>
#include <IGameRulesSystem.h>

//#include "Utilities.h"
#include "Addresses.h"

#define SPOOFS
//#define UNLOCK

#pragma region SpoofStructDefinitions
#ifdef SPOOFS
struct ChatMessageParams{
	uint8 type;
	EntityId sourceId;
	EntityId targetId;
	string msg;
	bool onlyTeam;
};
struct RenameEntityParams{
	EntityId entityId;
	string name;
};
struct SpectatorModeParams{
	EntityId entityId;
	uint8 mode;
	EntityId targetId;
	bool resetAll;
};
struct ChangeTeamParams{
	EntityId entityId;
	int	teamId;
};
struct RadioMessageParams{
	EntityId *sourceId;
	uint8 *msg;
};
struct DropItemParams{
	float impulseScale;
	EntityId itemId;
	bool selectNext;
	bool byDeath;
};
#endif
#pragma endregion

#pragma region HookDefinitions

#define QuickHandle(name,params_,funct,field,cht)\
int __fastcall Handle_##name(void *ecx, void *unk,params_ *params,INetChannel *pNetChannel){\
	int ret=0x8;\
	bool spoof=false;\
	if(pNetChannel){\
		spoof=VerifySpoof(params->field,pNetChannel,cht);\
	}\
	if(!spoof){\
		unhook(Handle_##name##Addr);\
		funct func=(funct)Handle_##name##Addr;\
		ret=func(ecx,unk,params,pNetChannel);\
		hook(Handle_##name##Addr,Handle_##name);\
	}\
	return ret;\
}

int __fastcall SendChatMessage(void *ecx, void *unk, EChatMessageType type, EntityId sourceId, EntityId targetId, const char *msg);
int __fastcall RenamePlayer(void *ecx,void *unk,IActor *pActor, const char *name);
int __fastcall OnClientConnect(void *ecx,void *unk,int channelId,bool reset);

typedef int (__fastcall *PFNSENDCHATMESSAGE)(void*,void*,EChatMessageType, EntityId, EntityId, const char*);
typedef int (__fastcall *PFNRENAMEPLAYER)(void*,void*,IActor*,const char*);
typedef int (__fastcall *PFNONCLIENTCONNECT)(void*,void*,int,bool);
typedef int (__fastcall *PFNSENDTEXTMESSAGE)(void*,void*,ETextMessageType,const char*,unsigned int,int,const char*,const char*,const char*,const char*);

#ifdef SPOOFS
inline bool VerifySpoof(EntityId sourceId,INetChannel *pNetChannel,const char *cht,bool kick=true);

int __fastcall Handle_SvRequestChatMessage(void* ecx,void *unk,ChatMessageParams *params,INetChannel *pNetChannel);
typedef int (__fastcall *PFNSVREQUESTCHATMESSAGE)(void *,void *,ChatMessageParams*,INetChannel*);

int __fastcall Handle_SvRequestRename(void* ecx,void *unk,RenameEntityParams *params,INetChannel *pNetChannel);
typedef int (__fastcall *PFNSVREQUESTRENAME)(void *,void *,RenameEntityParams*,INetChannel*);

int __fastcall Handle_SvRequestSpectatorMode(void* ecx,void *unk,SpectatorModeParams *params,INetChannel *pNetChannel);
typedef int (__fastcall *PFNSVREQUESTSPECTATORMODE)(void *,void *,SpectatorModeParams*,INetChannel*);

int __fastcall Handle_SvRequestHit(void* ecx,void *unk,HitInfo *params,INetChannel *pNetChannel);
typedef int (__fastcall *PFNSVREQUESTHIT)(void *,void *,HitInfo*,INetChannel*);

int __fastcall Handle_SvRequestSpectatorMode(void* ecx,void *unk,SpectatorModeParams *params,INetChannel *pNetChannel);
typedef int (__fastcall *PFNSVREQUESTSPECTATORMODE)(void *,void *,SpectatorModeParams*,INetChannel*);

int __fastcall Handle_SvRequestChangeTeam(void* ecx,void *unk,ChangeTeamParams *params,INetChannel *pNetChannel);
typedef int (__fastcall *PFNSVREQUESTCHANGETEAM)(void *,void *,ChangeTeamParams*,INetChannel*);

int __fastcall Handle_SvRequestRadioMessage(void* ecx,void *unk,RadioMessageParams *params,INetChannel *pNetChannel);
typedef int (__fastcall *PFNSVREQUESTRADIOMESSAGE)(void *,void *,RadioMessageParams*,INetChannel*);

int __fastcall Handle_SvRequestDropItem(void* ecx,void *unk,DropItemParams *params,INetChannel *pNetChannel);
typedef int (__fastcall *PFNSVREQUESTDROPITEM)(void *,void *,DropItemParams*,INetChannel*);

#endif

#ifdef UNLOCK
#define INETCHNL_DISCONNECT ((void*)0x3953EBD4)
long __stdcall Hook_Disconnect(long a,long cause,long b,char *reason);
typedef long (__stdcall *PFNDISCONNECT)(long a,long cause,long b,char *reason);
#endif

int __fastcall Hook_CWeaponOnShoot(void* ecx,void *unk,EntityId shooterId, EntityId ammoId, IEntityClass* pAmmoType, const Vec3& pos, const Vec3& dir, const Vec3& vel);
typedef int (__fastcall *PFNCWEAPONONSHOOT)(void*,void*,EntityId,EntityId,IEntityClass*,const Vec3&,const Vec3&,const Vec3&);

int __fastcall Hook_CNanoSuitSetSuitEnergy(void* ecx,void *unk,float value, bool playerInitiated);
typedef int (__fastcall *PFNCNANOSUITSETSUITENERGY)(void*,void*,float,bool);

void* __stdcall Hook_GetHostByName(const char* name);
int __stdcall Hook_RecvFrom(
	_In_    SOCKET                             s,
	_Inout_ LPWSABUF                           lpBuffers,
	_In_    DWORD                              dwBufferCount,
	_Out_   LPDWORD                            lpNumberOfBytesRecvd,
	_Inout_ LPDWORD                            lpFlags,
	_Out_   struct sockaddr                    *lpFrom,
	_Inout_ LPINT                              lpFromlen,
	_In_    LPWSAOVERLAPPED                    lpOverlapped,
	_In_    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);

extern ISystem *pSystem;
extern IGameRules *pGameRules;
extern IScriptSystem *pScriptSystem;
extern IGameFramework *pGameFramework; 
#pragma endregion