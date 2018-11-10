#pragma once
#include <mutex>
#include <iostream>

using namespace std;

class Util
{
	static std::mutex mtx_;

	template <typename T>
	static void th_info_(T last);
	template <typename T, typename... Args>
	static void th_info_(T first, Args... args);

public:
	template <typename... Args>
	static void th_info(Args... args);

	static int factorial(int x);
	
};


template <typename T>
void Util::th_info_(T last)
{
	cout << last << endl;
}
template <typename T, typename ... Args>
void Util::th_info_(T first, Args... args)
{
	cout << first;
	th_info_(args...);
}

template <typename ... Args>
void Util::th_info(Args... args)
{
	std::lock_guard<std::mutex> lg(mtx_);
	cout << "[" << this_thread::get_id() << "] ";

	th_info_(args...);
}
