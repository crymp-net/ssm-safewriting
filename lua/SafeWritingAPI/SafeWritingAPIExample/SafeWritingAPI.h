#ifndef __SAFEWRITING_API__
#define __SAFEWRITING_API__

#include <string.h>
#include <vector>
#include <map>

#define SAFEWRITING_API __declspec(dllimport)

typedef void (*APIFunc)(void);

class SafeWritingAPI{
public:
	SafeWritingAPI();
	virtual ~SafeWritingAPI()=0;

	virtual void RegisterLuaFunc(char *name,void func())=0;	

	virtual void RegisterLuaGlobal(char *name,int num)=0;
	virtual void RegisterLuaGlobal(char *name,bool num)=0;
	virtual void RegisterLuaGlobal(char *name,char* str)=0;
	virtual void RegisterLuaGlobal(char *name,float num)=0;


	virtual void GetLuaGlobal(char *name,int& num)=0;
	virtual void GetLuaGlobal(char *name,bool& num)=0;
	virtual void GetLuaGlobal(char *name,char*& str)=0;
	virtual void GetLuaGlobal(char *name,float& num)=0;


	virtual void GetArg(int idx,int& num)=0;
	virtual void GetArg(int idx,bool& num)=0;
	virtual void GetArg(int idx,char*& str)=0;
	virtual void GetArg(int idx,float& num)=0;

	virtual void StartReturn()=0;
	virtual void EndReturn()=0;
	virtual void ReturnToLua(int num)=0;
	virtual void ReturnToLua(bool num)=0;
	virtual void ReturnToLua(char* str)=0;
	virtual void ReturnToLua(float num)=0;

	virtual void PushFuncParam(int p)=0;
	virtual void PushFuncParam(bool p)=0;
	virtual void PushFuncParam(float p)=0;
	virtual void PushFuncParam(char* p)=0;

	virtual void BeginFuncCall(char *name)=0;
	virtual void BeginFuncCall(char *table,char *name)=0;
	virtual void CallFunction()=0;
	
	virtual void* GetIGameFramework()=0;
	std::map<std::string,APIFunc> *funcMap;
private:
	int retCtr;
};

#endif