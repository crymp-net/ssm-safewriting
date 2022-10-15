#include "HookDefs.h"
#include "NetworkStuff.h"
#include <IWeapon.h>
#include <IItemSystem.h>
#include "Shared.h"
#include "Utilities.h"

extern char msg[46];

void MemScan(void *base, int size) {
	char buffer[81920] = "";
	for (int i = 0; i<size; i++) {
		if (i % 16 == 0) sprintf(buffer, "%s %#04X: ", buffer, i);
		sprintf(buffer, "%s %02X", buffer, ((char*)base)[i] & 0xFF);
		if (i % 16 == 15) sprintf(buffer, "%s\n", buffer);
	}
	MessageBoxA(0, buffer, 0, 0);
}

#pragma region CallbackHooks
int __fastcall SendChatMessage(void *ecx,void *unk,EChatMessageType type, EntityId sourceId, EntityId targetId, const char *msg){
	PFNSENDCHATMESSAGE func=(PFNSENDCHATMESSAGE)SendChatMessageAddr;
	int ret=0x10;
	char *fmsg=0;
	SmartScriptTable pScript=pGameRules->GetEntity()->GetScriptTable();
	pScriptSystem->BeginCall(pScript,"OnChatMessage");
	pScriptSystem->PushFuncParam(pScript);
	pScriptSystem->PushFuncParam(type);
	pScriptSystem->PushFuncParam(sourceId);
	pScriptSystem->PushFuncParam(targetId);
	pScriptSystem->PushFuncParam(msg);
	bool ok=pScriptSystem->EndCall(fmsg);
	if(ok && fmsg){
		unhook(SendChatMessageAddr);
		ret=func(ecx,unk,type,sourceId,targetId,fmsg);
		hook(SendChatMessageAddr,SendChatMessage);
	}
	return ret;
}
int __fastcall RenamePlayer(void *ecx,void *unk,IActor *pActor, const char *name){
	PFNRENAMEPLAYER func=(PFNRENAMEPLAYER)RenamePlayerAddr;
	int ret=0x8;
	char *to=0;
	SmartScriptTable pScript=pGameRules->GetEntity()->GetScriptTable();
	pScriptSystem->BeginCall(pScript,"OnPlayerRename");
	pScriptSystem->PushFuncParam(pScript);
	pScriptSystem->PushFuncParam(pActor->GetEntityId());
	pScriptSystem->PushFuncParam(name);
	bool ok=pScriptSystem->EndCall(to);
	if(ok && to){
		unhook(RenamePlayerAddr);
		ret=func(ecx,unk,pActor,to);
		hook(RenamePlayerAddr,RenamePlayer);
	}
	return ret;
}
int __fastcall OnClientConnect(void *ecx,void *unk,int channelId,bool reset){
	PFNONCLIENTCONNECT func=(PFNONCLIENTCONNECT)OnClientConnectAddr;
	unhook(OnClientConnectAddr);
	int ret=func(ecx,unk,channelId,reset);
	hook(OnClientConnectAddr,OnClientConnect);
	INetChannel *pNetChannel=pGameFramework->GetNetChannel(channelId);
	//MemScan(pNetChannel, 256);
	if(pNetChannel){
		int n_ip = *(int*)(((const char*)pNetChannel) + 0x78);
		char profid[33];
		itoa(pNetChannel->GetProfileId(),profid,10);
		char ip[255];
		char justHost[255];
		strncpy(justHost,pNetChannel->GetName(),255);
		
		char *pos=strchr(justHost,':');
		if(pos)
			*pos=0;
		//Network::GetIP(justHost,ip);
		sprintf(ip, "%d.%d.%d.%d", (n_ip >> 24) & 255, (n_ip >> 16) & 255, (n_ip >> 8) & 255, (n_ip & 255));
		SmartScriptTable pScript=pGameRules->GetEntity()->GetScriptTable();
		pScriptSystem->BeginCall(pScript,"GatherClientData");
		pScriptSystem->PushFuncParam(pScript);
		pScriptSystem->PushFuncParam(channelId);
		pScriptSystem->PushFuncParam(pNetChannel->GetName());
		pScriptSystem->PushFuncParam(profid);
		pScriptSystem->PushFuncParam(ip);
		pScriptSystem->EndCall();
		PFNSENDTEXTMESSAGE info=(PFNSENDTEXTMESSAGE)SendTextMessageAddr;
		info(ecx,SendTextMessageAddr,eTextMessageConsole,::msg,eRMI_ToClientChannel,channelId,0,0,0,0);
	}
	return ret;
}
int __fastcall Hook_CWeaponOnShoot(void*base,void*unk,EntityId shooterId, EntityId ammoId, IEntityClass* pAmmoType, const Vec3& pos, const Vec3& dir, const Vec3& vel){
	int ret=8;
	IFireMode *fm=0;
	float fireRate=0;
	int Ver=GetGameVersion(0);
	if(Ver==5767){
		__asm{
			push ebx
			push ecx
			push eax
			push edx
			mov ebx,ecx
			mov ecx,dword ptr ds:[ebx+358h]
			mov fm,ecx
			mov eax,dword ptr ds:[ecx]
			mov edx,dword ptr ds:[eax+0BCh]
			call edx
			fstp DWORD PTR fireRate
			pop edx
			pop eax
			pop ecx
			pop ebx
		}
	} else {
		__asm{
			push ebx
			push ecx
			push eax
			push edx
			mov ebx,ecx
			mov ecx,dword ptr ds:[ebx+360h]
			mov fm,ecx
			mov eax,dword ptr ds:[ecx]
			mov edx,dword ptr ds:[eax+0C0h]
			call edx
			fstp DWORD PTR fireRate
			pop edx
			pop eax
			pop ecx
			pop ebx
		}
	}
	if(fm){
		EntityId wpnId=0;
		const char *entityClass="<unknown>";
		SmartScriptTable pScript=pGameRules->GetEntity()->GetScriptTable();
		pScriptSystem->BeginCall(pScript,"ActorOnShoot");
		pScriptSystem->PushFuncParam(pScript);
		pScriptSystem->PushFuncParam(shooterId);
		pScriptSystem->PushFuncParam(ammoId);
		pScriptSystem->PushFuncParam(pos);
		pScriptSystem->PushFuncParam(dir);
		pScriptSystem->PushFuncParam(vel);
		pScriptSystem->PushFuncParam(fireRate);
		pScriptSystem->PushFuncParam(wpnId);
		pScriptSystem->PushFuncParam(entityClass);
		pScriptSystem->EndCall();
	}
	PFNCWEAPONONSHOOT func=(PFNCWEAPONONSHOOT)CWeapon_OnShoot;
	unhook(CWeapon_OnShoot);
	func(base,unk,shooterId,ammoId,pAmmoType,pos,dir,vel);
	hook(CWeapon_OnShoot,Hook_CWeaponOnShoot);
	return ret;
}
int __fastcall Hook_CNanoSuitSetSuitEnergy(void *ecx, void *unk,float val,bool b1){
	PFNCNANOSUITSETSUITENERGY func=(PFNCNANOSUITSETSUITENERGY)CWeapon_OnShoot;
	unhook(CNanoSuit_SetSuitEnergy);
	int retval=func(ecx,unk,val,b1);
	hook(CNanoSuit_SetSuitEnergy,Hook_CNanoSuitSetSuitEnergy);
	printf("nanosuit, set energy: %f",val);
	return retval;
}

#pragma endregion

#pragma region SpoofHooks
#ifdef SPOOFS
bool VerifySpoof(EntityId sourceId,INetChannel *pNetChannel,const char *cht,bool kick){
	IActor *client=GetActorByChannel(pNetChannel);
	IActor *pretend=GetActorByEntityId(sourceId);
	if(client && client!=pretend){
		if(kick){
			SmartScriptTable pScript=pGameRules->GetEntity()->GetScriptTable();
			pScriptSystem->BeginCall(pScript,"OnCheatDetected");
			pScriptSystem->PushFuncParam(pScript);
			pScriptSystem->PushFuncParam(client->GetEntityId());
			pScriptSystem->PushFuncParam(cht);
			pScriptSystem->EndCall();
		}
		return true;
	}
	return false;
}
QuickHandle(SvRequestChatMessage,ChatMessageParams,PFNSVREQUESTCHATMESSAGE,sourceId,"chat spoof");
QuickHandle(SvRequestRename,RenameEntityParams,PFNSVREQUESTRENAME,entityId,"rename spoof");
QuickHandle(SvRequestSpectatorMode,SpectatorModeParams,PFNSVREQUESTSPECTATORMODE,entityId,"spectator spoof");
QuickHandle(SvRequestChangeTeam,ChangeTeamParams,PFNSVREQUESTCHANGETEAM,entityId,"team spoof");
int __fastcall Handle_SvRequestHit(void *ecx, void *unk, HitInfo *params, INetChannel *pNetChannel){
	int ret=0x8;
	bool spoof=false;
	if(pNetChannel){
		IActor *client=GetActorByChannel(pNetChannel);
		IActor *pretend=GetActorByEntityId(params->shooterId);
		if(client && client!=pretend){
			IEntity *ent=pSystem->GetIEntitySystem()->GetEntity(params->weaponId);
			bool ok=!ent;
			if(ent)
				ok=strcmp(ent->GetClass()->GetName(),"AACannon")!=0;
			if(ok){
				SmartScriptTable pScript=pGameRules->GetEntity()->GetScriptTable();
				pScriptSystem->BeginCall(pScript,"OnCheatDetected");
				pScriptSystem->PushFuncParam(pScript);
				pScriptSystem->PushFuncParam(client->GetEntityId());
				pScriptSystem->PushFuncParam("hit spoof");
				pScriptSystem->EndCall();
				spoof=true;
			}
		}
	}
	if(!spoof){
		unhook(Handle_SvRequestHitAddr);
		PFNSVREQUESTHIT func=(PFNSVREQUESTHIT)Handle_SvRequestHitAddr;
		ret=func(ecx,unk,params,pNetChannel);
		hook(Handle_SvRequestHitAddr,Handle_SvRequestHit);
	}
	return ret;
}
int __fastcall Handle_SvRequestDropItem(void *ecx, void *unk, DropItemParams *params, INetChannel *pNetChannel){
	if(params && params->itemId){
		IItem *pItem=pGameFramework->GetIItemSystem()->GetItem(params->itemId);
		IActor *client=GetActorByChannel(pNetChannel);
		if(pItem && client){
			IActor *pretend=GetActorByEntityId(pItem->GetOwnerId());
			if(pretend==client){	//OK
				unhook(Handle_SvRequestDropItemAddr);
				PFNSVREQUESTDROPITEM func=(PFNSVREQUESTDROPITEM)Handle_SvRequestDropItemAddr;
				func(ecx,unk,params,pNetChannel);
				hook(Handle_SvRequestDropItemAddr,Handle_SvRequestDropItem);
			}
		}
	}
	return 8;
}
#endif

#pragma endregion

#pragma region Unlock
#ifdef UNLOCK
long __stdcall Hook_Disconnect(long a,long cause,long b,char *reason){
	long ret=cause^4;
	printf("Disconnect %d,%s",cause,reason);
	PFNDISCONNECT orig=(PFNDISCONNECT)INETCHNL_DISCONNECT;
	if(
		!(cause==8 && reason && !strcmp(reason,"CD Key in use"))
	){
		unhook(INETCHNL_DISCONNECT);
		ret=orig(a,cause,b,reason);
		hook(INETCHNL_DISCONNECT,Hook_Disconnect);
	}
	return ret;
}
#endif
#pragma endregion

#pragma region AdditionalHooks
extern char SvMaster[255];
enum AttackTypes { DoS, Log, Crash, FakePlayers } type;

int __stdcall Hook_RecvFrom(
	_In_    SOCKET                             s,
	_Inout_ LPWSABUF                           lpBuffers,
	_In_    DWORD                              dwBufferCount,
	_Out_   LPDWORD                            lpNumberOfBytesRecvd,
	_Inout_ LPDWORD                            lpFlags,
	_Out_   struct sockaddr                    *lpFrom,
	_Inout_ LPINT                              lpFromlen,
	_In_    LPWSAOVERLAPPED                    lpOverlapped,
	_In_    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine ) {
	static std::map<std::string, int> history;
	static std::map<std::string, AttackTypes> hackers;
	std::string remote = std::string(inet_ntoa(((sockaddr_in*)lpFrom)[0].sin_addr)) + ":" + std::to_string(((sockaddr_in*)lpFrom)[0].sin_port);
	//FILE *f = fopen("network.log", "a");
	//if (hackers.find(remote) != hackers.end()) fprintf(f, "hacker %s broadcast!!\n", remote.c_str());
	unhook(WSARecvFrom);
	int retlen = WSARecvFrom(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine);
	hook(WSARecvFrom, Hook_RecvFrom);
	if (dwBufferCount>0 && lpBuffers[0].buf[0] && (lpBuffers[0].buf[0]&255)!=0xfe) {
		bool isAttack = false;
		if (hackers.find(remote) != hackers.end()) isAttack = true;
		for (int j = 0; j < dwBufferCount; j++) {
			char*& buf = lpBuffers[j].buf;
			ULONG& recvlen = lpBuffers[j].len;
			//fprintf(f, "recvd/%d/%d/%p from %s: ",j, isAttack, remote.c_str(),lpCompletionRoutine);
			//for (int i = 0; i < recvlen; i++) fprintf(f, "%02X ", buf[i]&255);
			//fprintf(f, "\n");
			if (buf[0] == 0x3C) {
				if (history.find(remote) == history.end()) {
					isAttack = true;
					type = DoS;
				}
			} else if (buf[0] == 0x28) {
				if (history.find(remote) == history.end()) {
					isAttack = true;
					type = Log;
				}
			} else if(buf[0] == 0x3E){
				for (int i = 0; i < recvlen; i++) {
					if ((buf[i]&255) == 0x25) {
						buf[i] = 0x23;
						isAttack = true;
						type = Crash;
					}
					//printf("%02X", buf[i]);
				}
			}
			if(!isAttack) {
				if (buf[0] != 0x3E) {
					history[remote] = buf[0];
				}
			} else {
				//fprintf(f, "hack detected, type: %d\n", type);
				hackers[remote] = type;
				for (int i = 0; i < recvlen; i++) {
					buf[i] = 0;
				}
			}
			//fflush(f);
		}
	}
	//fclose(f);
	return retlen;
}
void* __stdcall Hook_GetHostByName(const char* name){
	unhook(gethostbyname);
	hostent *h=0;
	if(strcmp(SvMaster,"gamespy.com")){
		int len=strlen(name);
		char *buff=new char[len+255];
		strcpy(buff,name);
		int a,b,c,d;
		bool isip = sscanf(SvMaster,"%d.%d.%d.%d",&a,&b,&c,&d) == 4;
		if(char *ptr=strstr(buff,"gamespy.com")){
			if(!isip)
				memcpy(ptr,SvMaster,strlen(SvMaster));
		}
		else if(char *ptr=strstr(buff,"gamesspy.eu")){
			if(!isip)
				memcpy(ptr,SvMaster,strlen(SvMaster));
		}
		h=gethostbyname(buff);
		delete [] buff;
	} else {
		h=gethostbyname(name);
	}
	hook(gethostbyname,Hook_GetHostByName);
	return h;
}
#pragma endregion