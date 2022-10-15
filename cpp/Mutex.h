#pragma once

#define WIN32_LEAN_AND_MEAN
#include "Shared.h"
#include <Windows.h>

//Just a really simple Mutex class, we don't need whole <mutex> of C++11 adding 200kB overhead to binary
struct Mutex {
	HANDLE hMutex;
	DWORD threadId;
	const char *szName;
	Mutex(const char *name = 0) {
		szName = name;
#ifdef THREAD_SAFE
		hMutex = CreateMutexA(0, false, name);
#endif
	}
	~Mutex() {
#ifdef THREAD_SAFE
		if (hMutex) {
			CloseHandle(hMutex);
		}
#endif
	}
	bool Lock(DWORD timeo = INFINITE) {
#ifdef THREAD_SAFE
		DWORD res = WaitForSingleObject(hMutex, timeo);
		threadId = GetCurrentThreadId();
		if (res == WAIT_TIMEOUT) return false;
		return res != WAIT_FAILED;
#else
		return true;
#endif
	}
	void Unlock() {
#ifdef THREAD_SAFE
		ReleaseMutex(hMutex);
#endif
	}
};
struct SimpleLock {
	Mutex *mptr;
	SimpleLock(Mutex& mtx) : mptr(&mtx) {
		mtx.Lock();
	}
	~SimpleLock() {
		if (mptr)
			mptr->Unlock();
	}
};
