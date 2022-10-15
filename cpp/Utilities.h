#pragma once
#include <IGameFramework.h>
#include <I3DEngine.h>
#include <IGameRulesSystem.h>
#include <IActorSystem.h>
#include <INetwork.h>
#include "HookDefs.h"

#pragma region UtilitiesDefinitions
#define GetActorByChannelId(c) pGameFramework->GetIActorSystem()->GetActorByChannelId(c)
#define GetActorByChannel(c) GetActorByChannelId(pGameFramework->GetGameChannelId(c))
#define GetActorByEntityId(c) pGameFramework->GetIActorSystem()->GetActor(c)
char *GetIP(char *host);
int GetGameVersion(const char *file);
#define DeclareThisFunc(cls,name,...)\
	inline int TF_##name(cls *,void *,__VA_ARGS__);\
	int __fastcall TP_##name(void *ecx,void *faddr,__VA_ARGS__)
#define CallThisFunc(cls,name,...)\
		return TF_##name((cls *)ecx,faddr,__VA_ARGS__)
#define CreateThisFunc(cls,name,...)\
	int TF_##name(cls *self,void *faddr,__VA_ARGS__)
#define GetThisFunc(name) TF_##name
extern IGameFramework *pGameFramework;
#pragma endregion