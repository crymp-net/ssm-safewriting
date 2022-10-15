#pragma once
#ifndef __INTEGRITY_SERVICE_H__
#define __INTEGRITY_SERVICE_H__

#include <IGameFramework.h>
#define GameWarning(...) /* do nothing */
#include <IGameObject.h>
#include <ISerialize.h>
#include <CrySizer.h>

struct CIntegrityService :
	public CGameObjectExtensionHelper<CIntegrityService, IGameObjectExtension, 64>
{
	CIntegrityService() {

	}
	virtual bool Init(IGameObject *pGameObject);

	virtual void InitClient(int channelId) {}
	virtual void PostInit(IGameObject *pGameObject) {
		//GetGameObject()->EnableUpdateSlot(this, 0);
	}
	virtual void PostInitClient(int channelId) {
		//...
	}
	virtual void Release() {
		delete this;
	}
	virtual bool NetSerialize(TSerialize ser, EEntityAspects aspect, uint8 profile, int pflags) {
		return true;
	}
	virtual void FullSerialize(TSerialize ser) {
		//bool en = true;
		//ser.Value("enabled", en, 'bool');
	}
	virtual void PostSerialize() {}
	virtual void SerializeSpawnInfo(TSerialize ser) {}
	virtual ISerializableInfoPtr GetSpawnInfo() { return 0; }
	virtual void Update(SEntityUpdateContext &ctx, int updateSlot) {}
	virtual void PostUpdate(float frameTime) {}
	virtual void PostRemoteSpawn() {}
	virtual void HandleEvent(const SGameObjectEvent &) {}
	virtual void ProcessEvent(SEntityEvent &) {}
	virtual void SetChannelId(uint16 id) {}
	virtual void SetAuthority(bool auth) {}
	virtual void GetMemoryStatistics(ICrySizer * s) { s->Add(*this); }
};

#endif