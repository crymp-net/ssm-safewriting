#pragma once
#pragma region Addresses
#define SPOOFS

//#define ALLOW_WARS
#ifdef ALLOW_WARS

#define _SendChatMessageAddr ((void*)0x391E9690)
#define _RenamePlayerAddr ((void*)0x391E8B50)
#define _OnClientConnectAddr ((void*)0x391E88E0)
#define _SendTextMessageAddr ((void*)0x391E9600)
#define _Handle_SvRequestChatMessageAddr ((void*)0x391F8580)
#define _Handle_SvRequestHitAddr ((void*)0x391F33D0)
#define _Handle_SvRequestSpectatorModeAddr ((void*)0x391F3370)
#define _Handle_SvRequestChangeTeamAddr ((void*)0x391F3340)
#define _Handle_SvRequestRadioMessageAddr ((void*)0x391F3300)
#define _Handle_SvRequestRenameAddr ((void*)0x391F8550)

#define _SendChatMessageAddrW ((void*)0x391F8F10)
#define _RenamePlayerAddrW ((void*)0x391F8120)
#define _OnClientConnectAddrW ((void*)0x391F7EA0)
#define _SendTextMessageAddrW ((void*)0x391F8E80)
#define _Handle_SvRequestRenameAddrW ((void*)0x39209AA0)
#define _Handle_SvRequestChatMessageAddrW ((void*)0x39209AD0)
#define _Handle_SvRequestHitAddrW ((void*)0x392091C0)
#define _Handle_SvRequestSpectatorModeAddrW ((void*)0x392090E0)
#define _Handle_SvRequestChangeTeamAddrW ((void*)0x392090B0)
#define _Handle_SvRequestRadioMessageAddrW ((void*)0x39209070)

void *SendChatMessageAddr=_SendChatMessageAddr;
void *RenamePlayerAddr=_RenamePlayerAddr;
void *OnClientConnectAddr=_OnClientConnectAddr;
void *SendTextMessageAddr=_SendTextMessageAddr;
void *Handle_SvRequestChatMessageAddr=_Handle_SvRequestChatMessageAddr;
void *Handle_SvRequestHitAddr=_Handle_SvRequestHitAddr;
void *Handle_SvRequestSpectatorModeAddr=_Handle_SvRequestSpectatorModeAddr;
void *Handle_SvRequestChangeTeamAddr=_Handle_SvRequestChangeTeamAddr;
void *Handle_SvRequestRadioMessageAddr=_Handle_SvRequestRadioMessageAddr;
void *Handle_SvRequestRenameAddr=_Handle_SvRequestRenameAddr;
float *EXPLOSIVE_REMOVAL_TIME = (float*)0x39267E60s;

#else
#define SendChatMessageAddr5767 ((void*)0x391E9690)
#define RenamePlayerAddr5767 ((void*)0x391E8B50)
#define OnClientConnectAddr5767 ((void*)0x391E88E0)
#define SendTextMessageAddr5767 ((void*)0x391E9600)
#define Handle_SvRequestChatMessageAddr5767 ((void*)0x391F8580)
#define Handle_SvRequestHitAddr5767 ((void*)0x391F33D0)
#define Handle_SvRequestSpectatorModeAddr5767 ((void*)0x391F3370)
#define Handle_SvRequestChangeTeamAddr5767 ((void*)0x391F3340)
#define Handle_SvRequestRadioMessageAddr5767 ((void*)0x391F3300)
#define Handle_SvRequestRenameAddr5767 ((void*)0x391F8550)
#define Handle_SvRequestDropItemAddr5767 ((void*)0x39033090)
#define CWeapon_OnShoot5767 ((void*)0x39143D90)
#define CNanoSuit_SetSuitEnergy5767 ((void*)0x390603D0)
#define EXPLOSIVE_REMOVAL_TIME5767 ((float*)0x392549BC)

#define SendChatMessageAddr6156 ((void*)0x391ED200)
#define RenamePlayerAddr6156 ((void*)0x391EC6C0)
#define OnClientConnectAddr6156 ((void*)0x391EC440)
#define SendTextMessageAddr6156 ((void*)0x391ED170)
#define Handle_SvRequestChatMessageAddr6156 ((void*)0x391FC530)
#define Handle_SvRequestHitAddr6156 ((void*)0x391FBCA0)
#define Handle_SvRequestSpectatorModeAddr6156 ((void*)0x391FBC40)
#define Handle_SvRequestChangeTeamAddr6156 ((void*)0x391FBC10)
#define Handle_SvRequestRadioMessageAddr6156 ((void*)0x391FBBD0)
#define Handle_SvRequestRenameAddr6156 ((void*)0x391FC500)
#define Handle_SvRequestDropItemAddr6156 ((void*)0x39035360)
#define CWeapon_OnShoot6156 ((void*)0x39146BE0)
#define CNanoSuit_SetSuitEnergy6156 ((void*)0x3904D3E0)
#define EXPLOSIVE_REMOVAL_TIME6156 ((float*)0x39267E60)

extern void *SendChatMessageAddr;
extern void *RenamePlayerAddr;
extern void *OnClientConnectAddr;
extern void *SendTextMessageAddr;
extern void *Handle_SvRequestChatMessageAddr;
extern void *Handle_SvRequestHitAddr;
extern void *Handle_SvRequestSpectatorModeAddr;
extern void *Handle_SvRequestChangeTeamAddr;
extern void *Handle_SvRequestRadioMessageAddr;
extern void *Handle_SvRequestRenameAddr;
extern void *Handle_SvRequestDropItemAddr;
extern void *CWeapon_OnShoot;
extern void *CNanoSuit_SetSuitEnergy;
extern float *EXPLOSIVE_REMOVAL_TIME;
#endif

#define Offset(a,o) (*(uint32*)(((char*)a)+(o)))

#pragma endregion