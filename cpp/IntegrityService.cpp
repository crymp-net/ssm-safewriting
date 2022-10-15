#include "IntegrityService.h"
#include <IScriptSystem.h>
#include <Windows.h>

bool CIntegrityService::Init(IGameObject *pGameObject) {
	if (!pGameObject) return false;
	SetGameObject(pGameObject);
	if (!GetGameObject()->BindToNetwork()) {
		MessageBoxA(0, "Failed to bind CIntegrityService to network!!", 0, 0);
		return false;
	}
	//GetGameObject()->EnablePostUpdates(this);
	return true;
}