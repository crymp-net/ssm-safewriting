#include <Windows.h>
#include <stdio.h>
#include "SafeWritingAPI.h"

SafeWritingAPI *g_API;

void DoMath(){						//Functions those will be registered to Lua need to be void ...()!
	float num1=0,num2=0;			//Create two variables where parameters will be stored in.
	g_API->GetArg(1,num1);			//Get parameter 1 to num1.
	g_API->GetArg(2,num2);			//Get parameter 2 to num2.
									//Warning! Parameters/args are indexed from 1, not from 0!
	g_API->StartReturn();			//Tells API that we are starting to return some values.
	g_API->ReturnToLua(num1+num2);	//Return first value.
	g_API->ReturnToLua(num1-num2);	//Return second value.
	g_API->ReturnToLua(num1*num2);	//Return third value.
	g_API->ReturnToLua(num1/num2);	//Return fourth value.
	g_API->EndReturn();				//tell the API that we ended returning values! This step is IMPORTANT!
}
extern "C" {
	__declspec(dllexport) void Init(void *ptr){	//Gets called by SafeWriting.dll and receives address of API
		g_API=(SafeWritingAPI*)ptr;
		//Your code goes here:
		g_API->RegisterLuaFunc("operations",DoMath);	//Register function DoMath as "function operations" to Lua
	}
}

BOOL WINAPI DllMain(HINSTANCE hMod, DWORD dwReason, LPVOID reserve){
	return TRUE;
}