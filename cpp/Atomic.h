#pragma once

#include "Mutex.h"

template<typename T>
struct Atomic {
	Mutex *pMutex;
	T val;
	Atomic(T initial) {
		pMutex = new Mutex;
		val = initial;
	}
	~Atomic() {
		if (pMutex) {
			delete pMutex;
			pMutex = 0;
		}
	}
	void set(T n) {
		pMutex->Lock();
		val = n;
		pMutex->Unlock();
	}
	void get(T& ref) const {
		pMutex->Lock();
		ref = val;
		pMutex->Unlock();
	}
};