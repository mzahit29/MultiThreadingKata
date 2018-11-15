#pragma once
#include <mutex>
#include <iostream>
#include <string>

using namespace std;

class Util
{
	static std::mutex mtx_;

	template <typename T>
	static void print_(T last);
	template <typename T, typename... Args>
	static void print_(T first, Args... args);

public:
	template <typename... Args>
	static void print(Args... args);

	static int factorial(int x);

	static void simulate_work(string parent_func);

	static void count_up_to_limit(int limit, std::mutex& m);
	static void count_down_to_zero(int start, std::mutex& m);

	static void print_letter_turn_based(int t_id, bool& run_flag, int& turn, char& c, mutex& m, condition_variable& cond);
};


template <typename T>
void Util::print_(T last)
{
	cout << last << endl;
}
template <typename T, typename ... Args>
void Util::print_(T first, Args... args)
{
	cout << first;
	print_(args...);
}

template <typename ... Args>
void Util::print(Args... args)
{
	std::lock_guard<std::mutex> lg(mtx_);
	cout << "[" << this_thread::get_id() << "] ";

	print_(args...);
}
