/* This class implements  Cpp11 threading functionality. 
*/
#pragma once
#include "Threading.cpp"


class CppThread :public Threading{
    queue<Task> tasks = {};
    vector<thread> threads = {};
    mutex consoleMtx;
    mutex wakeMtx;
    mutex barrierMtx;
    mutex taskMtx;
    condition_variable wakeCond;
    condition_variable barrierCond;
    unsigned nThreads = thread::hardware_concurrency();
    unsigned barrierThreshold = nThreads + 1;
    unsigned barrierCount = barrierThreshold;
    unsigned barrierGeneration = 0;    
    bool morePossibleWork = true;
    
public:
    CppThread() {}
    void wake_all()override {
        morePossibleWork = false;
        for (size_t i = 0; i < nThreads; ++i)
            wakeCond.notify_all();

    }
    void clean()override {
        for (auto& t : threads)
            t.join();
    }
    void barrier() {
        unique_lock<mutex> lock(barrierMtx);
        unsigned gen = barrierGeneration;
        if (--barrierCount == 0) {
            ++barrierGeneration;
            barrierCond.notify_all();
        }
        else {
            while (gen == barrierGeneration)
                barrierCond.wait(lock);
        }
    }
    void perform_task() {
       
        barrier();
        while (morePossibleWork)
        {
            {unique_lock<mutex> lk(wakeMtx);
            wakeCond.wait(lk); }

            while (!tasks.empty()) {
                
                Task task;
                bool haveTask = false;
                { // DCLP - Double Check Lock Pattern
                    lock_guard<mutex> lk(taskMtx);
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
    }
    void findExpression(directory_entry const& d, string& expr, map <path, vector<string>>& report, short& totalMatches, bool v) {
        path file = absolute(d.path());
        ifstream iff(file);

        if (iff.is_open()) {
            string line;
            short numOfline = 1;
            vector<string> matches;

            if (v) {
                lock_guard<mutex> lk(consoleMtx);
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
                            lock_guard<mutex> lk(consoleMtx);
                            cout << "Matched " << numofMatches << ": " << d.path() << " [" << numOfline << "] " << originLine << "\n"; }
                    }

                    {
                        lock_guard<mutex> lk(taskMtx);
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
                lock_guard<mutex> lk(consoleMtx);
                cout << '\n';
            }
              
            if (matches.size() != 0) {
                lock_guard<mutex> lk(taskMtx);
                report.insert({ d.path(),matches });
            }
        }
        else
            throw exception("Unable to open file!");
    }
    void create_thread()override {
       // cout << "Create a Cpp11 thread pool.\n";
        for (unsigned i = 0; i < nThreads; ++i) {
            threads.push_back(thread([this] {this->perform_task(); }));
        }

        barrier();
    }
    void wake_one()override {
        wakeCond.notify_one();
    }
    void print_scanning(directory_entry const& d)override {
        {
            lock_guard<mutex> lk(consoleMtx);
            cout << "Scanning: " << d.path() << '\n'; }
    }
    void push_task(Task t)override {
        lock_guard<mutex> lk(taskMtx);
        tasks.push(t);
    }
};

