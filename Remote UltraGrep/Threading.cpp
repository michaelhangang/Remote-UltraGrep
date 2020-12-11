/* Base class that defines functions for Cppthread and Win32thread class
*/
#pragma once
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <string>
#include <vector>
#include <locale>
#include <fstream>
#include <regex>
#include <sstream>
#include <thread>
#include <map>
#include <mutex>
#include <queue>
#include <memory>
using namespace std;
#include <filesystem>
using namespace std::filesystem;

struct Task {
    directory_entry d;
    string* expr;
    map <path, vector<string>>* report;
    short* totalMatches;
    bool v = false;
    Task() :d(), expr(nullptr), report(nullptr), totalMatches(nullptr), v(false) {};
    Task(directory_entry d, string& expr, map <path, vector<string>>& report, short& totalMatches, bool v) :d(d), expr(&expr), report(&report), totalMatches(&totalMatches), v(v) {}
    Task& operator= (Task const& t) {
        this->d = t.d;
        this->expr = t.expr;
        this->report = t.report;
        this->totalMatches = t.totalMatches;
        this->v = t.v;
        return *this;
    }
};
class  Threading
{
public:
    using pointer_type = shared_ptr<Threading>;
    virtual ~Threading() {};
    virtual void create_thread() {};
    virtual void wake_all() {};
    virtual void clean() {};
    virtual void  wake_one() {};
    virtual void print_scanning(directory_entry const& d) {};
    virtual void push_task(Task t) {};
};