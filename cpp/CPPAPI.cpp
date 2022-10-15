#include "CPPAPI.h"
#include "AtomicCounter.h"
#include "Atomic.h"
#include "Crypto.h"
#include "Helpers.h"
#include <IEntity.h>
#include <IEntitySystem.h>
#include <IVehicleSystem.h>
#include <IGameObjectSystem.h>
#include <CryThread.h>
#include <sstream>
#include <string>
#include <Windows.h>
#include <Winnls.h>
#include <winternl.h>
//#include <mutex>
//#include <functional>

#pragma region CPPAPI

extern std::deque<AsyncData*> asyncQueue;
extern int asyncQueueIdx;
extern std::map<std::string, std::string> asyncRetVal;
extern IScriptSystem *pScriptSystem;

HANDLE gEvent;
HANDLE checkerThread = 0;

void CALLBACK CheckMemory(void *ptr) {

}

CPPAPI::CPPAPI(ISystem *pSystem, IGameFramework *pGameFramework)
	: m_pSystem(pSystem),
	m_pSS(pSystem->GetIScriptSystem()),
	m_pGameFW(pGameFramework)
{
	Init(m_pSS, m_pSystem);
	gEvent = CreateEvent(0, 0, 0, 0);
	thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)AsyncThread, 0, 0, 0);
	SetGlobalName("CPPAPI");
	RegisterMethods();
	/*if (!checkerThread) {
		checkerThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)CheckMemory, 0, 0, 0);
	}*/
}
CPPAPI::~CPPAPI() {
	if (thread)
		TerminateThread(thread, 0);
}
void CPPAPI::RegisterMethods() {
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &CPPAPI::
	SetGlobalName("CPPAPI");
	SCRIPT_REG_TEMPLFUNC(FSetCVar, "cvar, value");
	SCRIPT_REG_TEMPLFUNC(Random, "");
	SCRIPT_REG_TEMPLFUNC(ConnectWebsite, "host, page, port, http11, timeout, methodGet");
	SCRIPT_REG_TEMPLFUNC(GetIP, "host");
	SCRIPT_REG_TEMPLFUNC(GetLocalIP, "");
	SCRIPT_REG_TEMPLFUNC(GetMapName, "");
	SCRIPT_REG_TEMPLFUNC(AsyncConnectWebsite, "host, page, port, http11, timeout, methodGet");
	SCRIPT_REG_TEMPLFUNC(DoAsyncChecks, "");
	SCRIPT_REG_TEMPLFUNC(SHA256, "text");
	SCRIPT_REG_TEMPLFUNC(GetLocaleInformation, "");
	SCRIPT_REG_TEMPLFUNC(SetExplosiveRemovalTime, "ms");
	SCRIPT_REG_TEMPLFUNC(FileEncrypt, "file, out");
	SCRIPT_REG_TEMPLFUNC(LoadScript, "file");
	SCRIPT_REG_TEMPLFUNC(LoadSSMScript, "file");
	SCRIPT_REG_TEMPLFUNC(SendMessageToClient, "rpcId, method, params...");
	SCRIPT_REG_TEMPLFUNC(CheckRPCID, "rpcId, channelId");
	SCRIPT_REG_TEMPLFUNC(CloseRPCID, "rpcId");
	SCRIPT_REG_TEMPLFUNC(GetTime, "future");
	SCRIPT_REG_TEMPLFUNC(ReloadCoreScripts, "");
}
int CPPAPI::ReloadCoreScripts(IFunctionHandler *pH) {
	return pH->EndFunction(PostInitScripts(true));
}
int CPPAPI::GetTime(IFunctionHandler *pH, int future) {
	time_t t = time(0) + future;
	tm *info = localtime(&t);
	static char bf[64];
	sprintf(bf, "%04d%02d%02d%02d%02d%02d", info->tm_year + 1900, info->tm_mon + 1, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);
	return pH->EndFunction(bf);
}
int CPPAPI::CheckRPCID(IFunctionHandler *pH, const char *rpcId, int channelId) {
	return pH->EndFunction(CheckClientID(rpcId, channelId));
}
int CPPAPI::CloseRPCID(IFunctionHandler *pH, const char *rpcId) {
	return pH->EndFunction(CloseClientID(rpcId));
}
int CPPAPI::SendMessageToClient(IFunctionHandler *pH, const char *id, const char *method, SmartScriptTable params) {
	IScriptTable::Iterator it = params->BeginIteration();
	std::vector<const char*> args;
	while (params->MoveNext(it)) {
		args.push_back(it.value.str);
	}
	if(::SendMessageToClient(id, method, args)){
		return pH->EndFunction(true);
	}
	return pH->EndFunction(false);
}
int CPPAPI::FileEncrypt(IFunctionHandler *pH, const char *file, const char *out) {
	::FileEncrypt(file, out);
	return pH->EndFunction();
}
int CPPAPI::LoadScript(IFunctionHandler *pH, const char *name) {
	char *main = 0;
	int len = FileDecrypt(name, &main);
	if (len) {
		bool ok = true;
		if (pScriptSystem) {
			if (!pScriptSystem->ExecuteBuffer(main, len)) {
				ok = false;
			}
		}
		for (int i = 0; i < len; i++) main[i] = 0;
		delete[] main;
		main = 0;
		return pH->EndFunction(ok);
	}
	return pH->EndFunction(false);
}
int CPPAPI::LoadSSMScript(IFunctionHandler *pH, const char *name) {
	char path[2 * MAX_PATH];
	GetGameFolder(path);
	sprintf(path, "%s\\Mods\\SafeWriting\\%s", path, name);
	return pH->EndFunction(pScriptSystem->ExecuteFile(path, true, true));
}
int CPPAPI::SetExplosiveRemovalTime(IFunctionHandler *pH, float fval) {
	extern float *EXPLOSIVE_REMOVAL_TIME;
	DWORD old;
	VirtualProtect((void*)EXPLOSIVE_REMOVAL_TIME, 16, PAGE_READWRITE, &old);
	*EXPLOSIVE_REMOVAL_TIME = fval;
	VirtualProtect((void*)EXPLOSIVE_REMOVAL_TIME, 16, old, 0);
	return pH->EndFunction(true);
}
int CPPAPI::SHA256(IFunctionHandler *pH, const char *text) {
	unsigned char digest[32];
	char hash[80];
	sha256((const unsigned char*)text, strlen(text), digest);
	for (int i = 0; i < 32; i++) {
		sprintf(hash + i * 2, "%02X", digest[i] & 255);
	}
	return pH->EndFunction(hash);
}
int CPPAPI::GetLocaleInformation(IFunctionHandler *pH) {
	char buffer[32];
#ifndef LOCALE_SNAME
#define LOCALE_SNAME 0x5C
#endif
	GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SNAME, buffer, sizeof(buffer));
	TIME_ZONE_INFORMATION tzinfo;
	GetTimeZoneInformation(&tzinfo);
	return pH->EndFunction(buffer, tzinfo.Bias);
}
int CPPAPI::FSetCVar(IFunctionHandler* pH, const char * cvar, const char *val) {
	if (ICVar *cVar = pConsole->GetCVar(cvar))
		cVar->ForceSet(val);
	return pH->EndFunction(true);
}
int CPPAPI::Random(IFunctionHandler* pH) {
	static bool set = false;
	if (!set) {
		srand(time(0) ^ clock());
		set = true;
	}
	return pH->EndFunction(rand());
}
int CPPAPI::ConnectWebsite(IFunctionHandler* pH, char * host, char * page, int port, bool http11, int timeout, bool methodGet, bool alive) {
	using namespace Network;
	std::string content = "Error";
	content = Connect(host, page, methodGet ? INetGet : INetPost, http11 ? INetHTTP11 : INetHTTP10, port, timeout, alive);
	return pH->EndFunction(content.c_str());
}
int CPPAPI::GetIP(IFunctionHandler* pH, char* host) {
	if (strlen(host)>0) {
		char ip[255];
		Network::GetIP(host, ip);
		return pH->EndFunction(ip);
	}
	return pH->EndFunction();
}
int CPPAPI::GetLocalIP(IFunctionHandler* pH) {
	char hostn[255];
	if (gethostname(hostn, sizeof(hostn)) != SOCKET_ERROR) {
		struct hostent *host = gethostbyname(hostn);
		if (host) {
			for (int i = 0; host->h_addr_list[i] != 0; ++i) {
				struct in_addr addr;
				memcpy(&addr, host->h_addr_list[i], sizeof(struct in_addr));
				return pH->EndFunction(inet_ntoa(addr));
			}
		}
	}
	return pH->EndFunction();
}
int CPPAPI::GetMapName(IFunctionHandler *pH) {
	return pH->EndFunction(pGameFramework->GetLevelName());
}
int CPPAPI::DoAsyncChecks(IFunctionHandler *pH) {
#ifdef DO_ASYNC_CHECKS
	extern Mutex g_mutex;
	IScriptTable *tbl = pScriptSystem->CreateTable();
	tbl->AddRef();
	std::vector<IScriptTable*> refs;
	//commonMutex.lock();
	g_mutex.Lock();
	for (std::map<std::string, std::string>::iterator it = asyncRetVal.begin(); it != asyncRetVal.end(); it++) {
		IScriptTable *item = pScriptSystem->CreateTable();
		item->AddRef();
		item->PushBack(it->first.c_str());
		item->PushBack(it->second.c_str());
		tbl->PushBack(item);
		refs.push_back(item);
	}
	g_mutex.Unlock();
	int code = pH->EndFunction(tbl);
#ifdef OLD_MSVC_DETECTED
	for (size_t i = 0; i < refs.size(); i++) {
		SAFE_RELEASE(refs[i]);
	}
#else
	for (auto& it : refs) {
		SAFE_RELEASE(it);
	}
#endif
	SAFE_RELEASE(tbl);
	return code;
#else
	return pH->EndFunction(0);
#endif
}
int CPPAPI::AsyncConnectWebsite(IFunctionHandler* pH, char * host, char * page, int port, bool http11, int timeout, bool methodGet, bool alive) {
	using namespace Network;
	ConnectStruct *now = new ConnectStruct;
	if (now) {
		now->host = host;
		now->page = page;
		now->method = methodGet ? INetGet : INetPost;
		now->http = http11 ? INetHTTP11 : INetHTTP10;
		now->port = port;
		now->timeout = timeout;
		now->alive = alive;
		return now->callAsync(pH);
	}
	return pH->EndFunction();
}
#pragma endregion

#pragma endregion

#pragma region AsyncStuff
#ifdef OLD_MSVC_DETECTED
BOOL WINAPI DownloadMapStructEnumProc(HWND hwnd, LPARAM lParam) {
	DownloadMapStruct::Info *pParams = (DownloadMapStruct::Info*)(lParam);
	DWORD processId;
	if (GetWindowThreadProcessId(hwnd, &processId) && processId == pParams->pid) {
		SetLastError(-1);
		pParams->hWnd = hwnd;
		return FALSE;
	}
	return TRUE;
}
#endif
void AsyncConnect(int id, AsyncData *obj) {
	//GetAsyncObj(ConnectStruct,now);
	ConnectStruct *now = (ConnectStruct *)obj;
	std::string content = "\\\\Error: Unknown error";
	if (now) {
		now->lock();
		std::string host = now->host;
		std::string page = now->page;
		Network::INetMethods method = now->method, http = now->http;
		unsigned short port = now->port;
		int timeout = now->timeout;
		bool alive = now->alive;
		now->unlock();
		content = Network::Connect(host, page, method, http, port, timeout, alive);
	}
	if (content.length() > 2) {
		if (content[0] == 0xFE && content[1] == 0xFF) content = content.substr(2);
	}
	now->response = content;
}
static void AsyncThread() {
	extern Mutex g_mutex;
	WSADATA data;
	WSAStartup(0x202, &data);
	while (true) {
		WaitForSingleObject(gEvent, INFINITE);
		if (asyncQueue.size()) {
			for (std::deque<AsyncData*>::iterator it = asyncQueue.begin(); it != asyncQueue.end(); it++) {
				g_mutex.Lock();
				AsyncData *obj = *it;
				if (obj && !obj->finished) {
					obj->executing = true;
					g_mutex.Unlock();
					try {
						obj->exec();
					}
					catch (std::exception& ex) {
						printf("func/Unhandled exception: %s\n", ex.what());
					}
					g_mutex.Lock();
					obj->finished = true;
				}
				g_mutex.Unlock();
			}
		}
		ResetEvent(gEvent);
	}
	WSACleanup();
}
void GetClosestFreeItem(int *out) {
	static AtomicCounter idx(0);
	*out = idx.increment();
}
#pragma endregion
