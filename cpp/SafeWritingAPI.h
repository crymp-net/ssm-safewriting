#pragma once
#include <IGameFramework.h>
#include <IScriptSystem.h>
#include <ISystem.h>
#include <Windows.h>
#pragma region SafeWritingAPIDefinitions
#define SAFEWRITING_API __declspec(dllexport)   
typedef void (*APIFunc)(void); 
class FunctionRegisterer : public CScriptableBase{ 
public: 
    FunctionRegisterer(ISystem*,IGameFramework*); 
    int RunFunc(IFunctionHandler *pH,char *name); 
    int LoadDLL(IFunctionHandler *pH,char *path); 
    int Is64Bit(IFunctionHandler *pH); 
protected:
	void RegisterMethods();
	ISystem				*m_pSystem;
	IScriptSystem		*m_pSS;
	IGameFramework		*m_pGameFW;
}; 
class SAFEWRITING_API SafeWritingAPI{ 
public: 
    SafeWritingAPI(); 
    virtual ~SafeWritingAPI(); 
  
    virtual void RegisterLuaFunc(char *name,void func());    
  
    virtual void RegisterLuaGlobal(char *name,int num); 
    virtual void RegisterLuaGlobal(char *name,bool num); 
    virtual void RegisterLuaGlobal(char *name,char* str); 
    virtual void RegisterLuaGlobal(char *name,float num); 
  
  
    virtual void GetLuaGlobal(char *name,int& num); 
    virtual void GetLuaGlobal(char *name,bool& num); 
    virtual void GetLuaGlobal(char *name,char*& str); 
    virtual void GetLuaGlobal(char *name,float& num); 
  
  
    virtual void GetArg(int idx,int& num); 
    virtual void GetArg(int idx,bool& num); 
    virtual void GetArg(int idx,char*& str); 
    virtual void GetArg(int idx,float& num); 
  
    virtual void StartReturn(); 
    virtual void EndReturn(); 
    virtual void ReturnToLua(int num); 
    virtual void ReturnToLua(bool num); 
    virtual void ReturnToLua(char* str); 
    virtual void ReturnToLua(float num); 
  
    virtual void PushFuncParam(int p); 
    virtual void PushFuncParam(bool p); 
    virtual void PushFuncParam(float p); 
    virtual void PushFuncParam(char* p); 
  
    virtual void BeginFuncCall(char *name); 
    virtual void BeginFuncCall(char *table,char *name); 
    virtual void CallFunction(); 

	virtual void *GetIGameFramework();
    std::map<std::string,APIFunc> *funcMap; 
private: 
    int retCtr; 
};

extern ISystem *pSystem;
extern IGameFramework *pGameFramework;
extern IScriptSystem *pScriptSystem;
extern SafeWritingAPI *g_API; 

#pragma endregion