#pragma once
#include <Windows.h>


class CriticalSection {
	CRITICAL_SECTION m_cs;
public:
	CriticalSection() { InitializeCriticalSection(&m_cs); }
	~CriticalSection() { DeleteCriticalSection(&m_cs); }

private:
	void Enter() { EnterCriticalSection(&m_cs); }
	void Leave() { LeaveCriticalSection(&m_cs); }

	friend class CSLock;
};

class CSLock {
	CriticalSection& m_csr;
public:
	CSLock(CriticalSection& cs) : m_csr(cs) {
		m_csr.Enter();
	}
	~CSLock() {
		m_csr.Leave();
	}
};