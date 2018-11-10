#pragma once
#include <mutex>
#include <iostream>

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
