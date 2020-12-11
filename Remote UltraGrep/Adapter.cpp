#pragma once
#include "CppThread.cpp"


class Adapter {
	Threading& thread;
public:
	Adapter() = default;
	Adapter(Threading & thread):thread(thread){}
	void create_thread() {
		thread.create_thread();
	}
	void wake_all() {
		thread.wake_all();
	}
	void clean() {
		thread.clean();
	}
	void  wake_one() {
		thread.wake_one();
	}
	void print_scanning(directory_entry const& d) {
		thread.print_scanning(d);
	}
	void push_task(Task t) {
		thread.push_task(t);
	}
};