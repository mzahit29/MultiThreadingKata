#include "pch.h"
#include "Util.h"

using namespace std;

std::mutex Util::mtx_;

int Util::factorial(int x)
{
	int result{ 1 };
	for (int i = x; i > 1; --i)
	{
		result *= i;
	}

	Util::print(__FUNCTION__, " Result of ", x, " factorial is ", result);
	return result;
}

void Util::simulate_work(string parent_func)
{
	for (int i = 20; i > 0; --i)
	{
		int stime{ 10 };
		Util::print(parent_func, " ", i, " will sleep for ", stime, " ms");
		this_thread::sleep_for(chrono::milliseconds(stime));
	}
}

void Util::count_up_to_limit(int limit, std::mutex& m)
{
	for (int i = 0; i < limit; ++i)
	{
		this_thread::sleep_for(chrono::microseconds(10));
		lock_guard<mutex> lg{ m };
		cout << "[" << this_thread::get_id() << "] " << i << endl;
	}
}

void Util::count_down_to_zero(int start, std::mutex& m)
{
	if (start <= 0) return;

	for (int i = start; i > 0; --i)
	{
		this_thread::sleep_for(chrono::microseconds(10));
		lock_guard<mutex> lg{ m };
		cout << "[" << this_thread::get_id() << "] " << i << endl;
	}
}

