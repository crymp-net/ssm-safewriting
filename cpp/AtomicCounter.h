#pragma once

#include "Mutex.h"
struct AtomicCounter {
	Mutex *pMutex;
	int value;
	int limit;
	AtomicCounter(int lmt = 0) {
		pMutex = new Mutex;
		value = 0;
		limit = lmt;
	}
	~AtomicCounter() {
		if (pMutex) {
			delete pMutex;
		}
	}
	int increment() {
		int n = 0;
		pMutex->Lock();
		value++;
		if (limit > 0)
			value %= limit;
		n = value;
		pMutex->Unlock();
		return n;
	}
	int getValue() {
		int val = 0;
		pMutex->Lock();
		value = val;
		pMutex->Unlock();
		return val;
	}
};
