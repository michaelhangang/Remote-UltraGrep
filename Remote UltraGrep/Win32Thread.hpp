// The class implements Win32Thread function
#pragma once
#include "CriticalSection.hpp"
#include "Threading.cpp"
#include <functional>


class Win32Thread :public Threading{
	static  queue<Task> tasks;
	vector<HANDLE> threads ;
	static  CriticalSection consoleMtx ;
	static CriticalSection taskMtx;
	static  HANDLE  barrierEvent ;
	static  HANDLE wakeEvent ;
	static unsigned nThreads;
	static unsigned barrierThreshold ;
	static unsigned barrierCount ;
	static unsigned barrierGeneration;
	static bool morePossibleWork ;

public:
	Win32Thread(){}
	static void barrier() {
		unsigned gen = barrierGeneration;
		if (--barrierCount == 0) {
			++barrierGeneration;
			barrierCount = barrierThreshold;
			SetEvent(barrierEvent);
		}
		else {
			while (gen == barrierGeneration)
				WaitForSingleObject(barrierEvent, INFINITE);
		}
	}
	static DWORD WINAPI perform_task(LPVOID) {
		barrier();
		while (morePossibleWork)
		{
			WaitForSingleObject(wakeEvent, INFINITE);

			while (!tasks.empty()) {
				Task task;
				bool haveTask = false;
				{ // DCLP - Double Check Lock Pattern
					CSLock lk(taskMtx);
					if (!tasks.empty()) {
						task = tasks.front();
						tasks.pop();
						haveTask = true;
					}
				}

				if (haveTask) {
					findExpression(task.d, *task.expr, *task.report, *task.totalMatches, task.v);
				}
			}
		}

		return 0;
	}
	static void findExpression(directory_entry const& d, string& expr, map <path, vector<string>>& report, short& totalMatches, bool v) {
		path file = absolute(d.path());
		ifstream iff(file);

		if (iff.is_open()) {
			string line;
			short numOfline = 1;
			vector<string> matches;

			if (v) {
				CSLock lk(consoleMtx);
				cout << "Grepping: " << d.path() << '\n';
			}
			while (getline(iff, line))
			{
				if (line != "")
				{
					regex reg(expr);
					smatch sm;
					string originLine;
					short numofMatches = 0;
					while (regex_search(line, sm, reg)) {
						++numofMatches;
						if (numofMatches == 1)
							originLine = line;
						line = sm.suffix();  // return the substring between end of the match and end of the string.              
					}
					if (numofMatches >= 1 && v) {
						{
							CSLock lk(consoleMtx);
							cout << "Matched " << numofMatches << ": " << d.path() << " [" << numOfline << "] " << originLine << "\n"; }
					}

					{
						CSLock lk(taskMtx);
						totalMatches += numofMatches;
					}
					if (numofMatches == 1)
						matches.push_back("[" + to_string(numOfline) + "] " + originLine);
					else if (numofMatches > 1)
						matches.push_back("[" + to_string(numOfline) + ":" + to_string(numofMatches) + "] " + originLine);


				}

				++numOfline;
			}
			if (v) {
				CSLock lk(consoleMtx);
				cout << '\n';
			}

			if (matches.size() != 0) {
				CSLock lk(taskMtx);
				report.insert({ d.path(),matches });
			}
		}
		else
			throw exception("Unable to open file!");
	}
	void clean() override {
		WaitForMultipleObjects((DWORD)threads.size(), threads.data(), TRUE, INFINITE);
		for (auto& t : threads)
			CloseHandle(t);
		CloseHandle(wakeEvent);
		CloseHandle(barrierEvent);
	}
	void wake_all() override {
		morePossibleWork = false;
		for (size_t i = 0; i < nThreads; ++i)
			SetEvent(wakeEvent);
	}
	void create_thread()override {
	//	cout << "Create a Win32 thread pool.\n";
		for (unsigned i = 0; i < nThreads; ++i)
				threads.push_back(CreateThread(NULL, 0, perform_task, NULL, 0, NULL));

		barrier();
	}
	void wake_one()override {
		SetEvent(wakeEvent);
	}
	void print_scanning (directory_entry const& d)override {
		{
			CSLock lk(consoleMtx);
			cout << "Scanning: " << d.path() << '\n'; }
	}
	void push_task (Task t) override {
		CSLock lk(taskMtx);
		tasks.push(t);
	}
};


 