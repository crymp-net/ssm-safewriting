#include "SafeWritingAPI.h"
#pragma region SafeWritingAPI
SafeWritingAPI::SafeWritingAPI(){retCtr=0;}
SafeWritingAPI::~SafeWritingAPI(){}

void SafeWritingAPI::RegisterLuaGlobal(char *name,int num){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->SetGlobalValue(name,num);
}
void SafeWritingAPI::RegisterLuaGlobal(char *name,float num){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->SetGlobalValue(name,num);
}
void SafeWritingAPI::RegisterLuaGlobal(char *name,char *str){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->SetGlobalValue(name,str);
}
void SafeWritingAPI::RegisterLuaGlobal(char *name,bool num){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->SetGlobalValue(name,num);
}
void SafeWritingAPI::PushFuncParam(int val){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->PushFuncParam(val);
}
void SafeWritingAPI::PushFuncParam(bool val){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->PushFuncParam(val);
}
void SafeWritingAPI::PushFuncParam(float val){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->PushFuncParam(val);
}
void SafeWritingAPI::PushFuncParam(char* val){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->PushFuncParam(val);
}

void SafeWritingAPI::BeginFuncCall(char *name){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	int res=ss->BeginCall("_G",name);
}
void SafeWritingAPI::BeginFuncCall(char *table,char *name){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	int res=ss->BeginCall(table,name);
}
void SafeWritingAPI::CallFunction(){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	int res=ss->EndCall();
}

void SafeWritingAPI::StartReturn(){
	retCtr=0;
}
void SafeWritingAPI::EndReturn(){
	RegisterLuaGlobal("__CPP__CNT__",retCtr);
	retCtr=0;
}
void SafeWritingAPI::ReturnToLua(int num){
	char n[20];
	sprintf(n,"__CPP__RET__%d",retCtr++);
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->SetGlobalValue(n,num);
}
void SafeWritingAPI::ReturnToLua(float num){
	char n[20];
	sprintf(n,"__CPP__RET__%d",retCtr++);
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->SetGlobalValue(n,num);
}
void SafeWritingAPI::ReturnToLua(char *str){
	char n[20];
	sprintf(n,"__CPP__RET__%d",retCtr++);
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->SetGlobalValue(n,str);
}
void SafeWritingAPI::ReturnToLua(bool num){
	char n[20];
	sprintf(n,"__CPP__RET__%d",retCtr++);
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->SetGlobalValue(n,num);
}
void SafeWritingAPI::RegisterLuaFunc(char *name,APIFunc func){
	(*SafeWritingAPI::funcMap)[name]=func;
}
void SafeWritingAPI::GetLuaGlobal(char *name,int& num){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->GetGlobalValue(name,num);
}
void SafeWritingAPI::GetLuaGlobal(char *name,float& num){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->GetGlobalValue(name,num);
}
void SafeWritingAPI::GetLuaGlobal(char *name,char*& str){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	int ref=0;
	ss->GetGlobalValue(name,ref);
	str=(char*)ref;
}
void SafeWritingAPI::GetLuaGlobal(char *name,bool& num){
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->GetGlobalValue(name,num);
}
void SafeWritingAPI::GetArg(int idx,int& num){
	char n[20];
	sprintf(n,"__CPP__ARG__%d",idx);
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->GetGlobalValue(n,num);
}
void SafeWritingAPI::GetArg(int idx,bool& num){
	char n[20];
	sprintf(n,"__CPP__ARG__%d",idx);
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->GetGlobalValue(n,num);
}
void SafeWritingAPI::GetArg(int idx,float& num){
	char n[20];
	sprintf(n,"__CPP__ARG__%d",idx);
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->GetGlobalValue(n,num);
}
void SafeWritingAPI::GetArg(int idx,char*& str){
	char n[20];
	sprintf(n,"__CPP__ARG__%d",idx);
#ifdef IS6156DLL
	IScriptSystem *ss=gEnv->pSystem->GetIScriptSystem();
#else
	IScriptSystem *ss=pScriptSystem;
#endif

	ss->GetGlobalValue(n,str);
}
void* SafeWritingAPI::GetIGameFramework(){
#ifdef IS6156DLL
	return (void*)gEnv->pGameFramework;
#else
	return (void*)pGameFramework;
#endif
}

FunctionRegisterer::FunctionRegisterer(ISystem *pSystem,IGameFramework *pGameFramework)
	:	m_pSystem(pSystem),
		m_pSS(pSystem->GetIScriptSystem()),
		m_pGameFW(pGameFramework)
{
	Init(m_pSS, m_pSystem);
	SetGlobalName("DLLAPI100");
	RegisterMethods();
}
void FunctionRegisterer::RegisterMethods(){
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &FunctionRegisterer::
	SetGlobalName("DLLAPI");
	SCRIPT_REG_TEMPLFUNC(RunFunc,"name");
	SCRIPT_REG_TEMPLFUNC(LoadDLL,"path");
	SCRIPT_REG_FUNC(Is64Bit);
}
int FunctionRegisterer::RunFunc(IFunctionHandler *pH,char *name){
	if(g_API->funcMap->find(name)!=g_API->funcMap->end()){
		APIFunc f=(*g_API->funcMap)[name];
		f();
		return pH->EndFunction(true);
	}
	return pH->EndFunction(false);
}
int FunctionRegisterer::LoadDLL(IFunctionHandler *pH,char *name){
	HMODULE lib=LoadLibraryA(name);
	typedef void(*PFINIT)(void *);
	if(lib==NULL){
		printf("$6[SafeWritingAPI] Failed to load DLL! Error %d (%s)",GetLastError(),name);
		return pH->EndFunction(false);
	}
	PFINIT f=(PFINIT)GetProcAddress(lib,"Init");
	if(!f){
		printf("$6[SafeWritingAPI] Address of Init inside DLL is unknown! Error %d",GetLastError());
		return pH->EndFunction(false);
	} else f(g_API);
	return pH->EndFunction(true);
}
int FunctionRegisterer::Is64Bit(IFunctionHandler *pH){
#ifdef IS64BIT
	return pH->EndFunction(true);
#else
	return pH->EndFunction(false);
#endif
}
#pragma endregion