#pragma once

#include "Shared.h"
#include <IGameFramework.h>
#include <ISystem.h>
#include <IScriptSystem.h>
#include <IConsole.h>
#include <ILevelSystem.h>
#include <I3DEngine.h>
#include <Windows.h>
#include "Mutex.h"
#include "NetworkStuff.h"
#include "Atomic.h"

#pragma region CPPAPIDefinitions
class CPPAPI : public CScriptableBase {
public:
	CPPAPI(ISystem*, IGameFramework*);
	~CPPAPI();
	int ConnectWebsite(IFunctionHandler* pH, char * host, char * page, int port, bool http11 = false, int timeout = 15, bool methodGet = true, bool alive = false);
	int FSetCVar(IFunctionHandler* pH, const char * cvar, const char *val);
	int AsyncConnectWebsite(IFunctionHandler* pH, char * host, char * page, int port, bool http11 = false, int timeout = 15, bool methodGet = true, bool alive = false);
	int GetIP(IFunctionHandler* pH, char* host);
	int GetMapName(IFunctionHandler* pH);
	int Random(IFunctionHandler* pH);
	int GetLocalIP(IFunctionHandler* pH);
	int DoAsyncChecks(IFunctionHandler *pH);
	int SHA256(IFunctionHandler *pH, const char *text);
	int GetLocaleInformation(IFunctionHandler *pH);
	int SetExplosiveRemovalTime(IFunctionHandler *pH, float fval);
	int FileEncrypt(IFunctionHandler *pH, const char *file, const char *out);
	int LoadScript(IFunctionHandler *pH, const char *file);
	int LoadSSMScript(IFunctionHandler *pH, const char *file);
	int SendMessageToClient(IFunctionHandler *pH, const char *id, const char *method, SmartScriptTable params);
	int CheckRPCID(IFunctionHandler *pH, const char *id, int channelId);
	int CloseRPCID(IFunctionHandler *pH, const char *id);
	int GetTime(IFunctionHandler *pH, int future=0);
	int ReloadCoreScripts(IFunctionHandler *pH);
protected:
	void RegisterMethods();
	ISystem				*m_pSystem;
	IScriptSystem		*m_pSS;
	IGameFramework		*m_pGameFW;
	HANDLE				thread;
};
extern IConsole *pConsole;
extern IGameFramework *pGameFramework;
extern ISystem *pSystem;
#pragma endregion

#pragma region AsyncStuff
struct AsyncData;

static void AsyncThread();
void AsyncConnect(int id, AsyncData *obj);
inline void GetClosestFreeItem(int *out);

struct AsyncData {
	int id;
	bool finished;
	bool executing;
	Mutex *mutex;
	virtual void lock() { if (mutex) mutex->Lock(); }
	virtual void unlock() { if (mutex) mutex->Unlock(); }
	virtual void exec() {}
	virtual void onUpdate() {}
	virtual void postExec() {}
	virtual int callAsync(IFunctionHandler *pH = 0) {
		extern unsigned int g_objectsInQueue;
		extern Mutex g_mutex;
		this->mutex = &g_mutex;
		g_objectsInQueue++;
		g_mutex.Lock();
		extern HANDLE gEvent;
		extern std::deque<AsyncData*> asyncQueue;
		extern int asyncQueueIdx;
		GetClosestFreeItem(&asyncQueueIdx);
		this->id = asyncQueueIdx;
		this->finished = false;
		this->executing = false;
		SetEvent(gEvent);
		asyncQueue.push_back(this);
		g_mutex.Unlock();
		if (pH) {
			return pH->EndFunction(asyncQueueIdx);
		}
		return 0;
	}
	void ret(ScriptAnyValue val) {
		extern Mutex g_mutex;
		extern IScriptSystem *pScriptSystem;
		extern std::map<std::string, std::string> asyncRetVal;
		char outn[255];
		sprintf(outn, "AsyncRet%d", (int)id);
		pScriptSystem->SetGlobalAny(outn, val);
#ifdef DO_ASYNC_CHECKS
		g_mutex.Lock();
		asyncRetVal[std::string(outn)] = what;
		g_mutex.Unlock();
#endif
	}
	AsyncData() :
		finished(false),
		executing(false) {}
};
#define AsyncReturn(what)\
	extern IScriptSystem *pScriptSystem;\
	char outn[255];\
	sprintf(outn,"AsyncRet%d",(int)id);\
	pScriptSystem->SetGlobalAny(outn,what);\
	asyncRetVal[std::string(outn)] = what
#define GetAsyncObj(type,name) type *name=(type*)asyncQueue[id]
#define CreateAsyncCallLua(data)\
	GetClosestFreeItem(&asyncQueueIdx);\
	data->id=asyncQueueIdx;\
	data->finished=false;\
	data->executing=false;\
	asyncQueue.push_back(data);\
	SetEvent(gEvent);\
	return pH->EndFunction(asyncQueueIdx)
#define CreateAsyncCall(data)\
	GetClosestFreeItem(asyncQueue,&asyncQueueIdx);\
	data->id=asyncQueueIdx;\
	data->finished=false;\
	data->executing=false;\
	SetEvent(gEvent);\
	asyncQueue[asyncQueueIdx]=data

struct ConnectStruct : public AsyncData {
	char *host;
	char *page;
	Network::INetMethods method;
	Network::INetMethods http;
	unsigned short port;
	unsigned int timeout;
	std::string response;
	bool alive;
	virtual void exec() {
		AsyncConnect(this->id, (AsyncData*)this);
	}
	virtual void postExec() {
		ret(response.c_str());
	}
};
struct RPCEvent : public AsyncData {
	std::string sender;
	std::vector<std::string> arguments;
	RPCEvent(std::string from, std::vector<std::string>& args) {
		arguments = args;
		sender = from;
	}
	~RPCEvent() {
		
	}
	virtual void postExec() {
		extern IScriptSystem *pScriptSystem;
		if (pScriptSystem) {
			if (pScriptSystem->BeginCall("OnRPCEvent")) {
				pScriptSystem->PushFuncParam(sender.c_str());
				for (auto& it : arguments) {
					pScriptSystem->PushFuncParam(it.c_str());
				}
				pScriptSystem->EndCall();
			}
		}
	}
};
#pragma endregion
