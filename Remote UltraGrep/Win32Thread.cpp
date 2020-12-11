#include  "Win32Thread.hpp"


HANDLE Win32Thread::wakeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
HANDLE Win32Thread::barrierEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
unsigned Win32Thread::nThreads = thread::hardware_concurrency();
unsigned Win32Thread::barrierThreshold = nThreads + 1;
unsigned Win32Thread::barrierCount = barrierThreshold;
unsigned Win32Thread::barrierGeneration = 0;
bool Win32Thread::morePossibleWork = true;
queue<Task>  Win32Thread::tasks = {};
CriticalSection Win32Thread::consoleMtx;
CriticalSection  Win32Thread::taskMtx;