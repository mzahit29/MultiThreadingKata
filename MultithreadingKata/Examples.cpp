#include "pch.h"
#include "Examples.h"
#include <iostream>
#include <thread>
#include "Util.h"

using namespace std;

void Examples::simple_thread_lambda()
{
	Util::print(__FUNCTION__);

	cout << "You can start a tread with a lambda expression" << endl;
	thread t1{[]()
	{
		Util::print(__FUNCTION__);
	} };

	// Wait for t1 to finish in main thread
	t1.join();


	thread{ []()
	{
		Util::print(__FUNCTION__);
	} }.detach();

	thread&& t2 = thread{ []()
	{
		Util::print(__FUNCTION__);
	} };
	t2.join();
}

void Examples::simple_thread_function()
{
	thread t1{ Util::factorial, 4 };

	// Main tread doing some stuff in parallel with t1 and considering main might throw exception
	// in which case t1 wouldn't be joined before main exits and program will crash.
	// Second option Wrapper w(t1); class where in destructor we join ~Wrapper(){t1.join();}
	try
	{
		for (int i = 0; i < 10; ++i)
		{
			int stime{ 10 };
			Util::print(__FUNCTION__, " ", i, " Main thread will sleep for ", stime, " ms");
			this_thread::sleep_for(chrono::milliseconds(stime));
		}
		Util::print(__FUNCTION__, " Simulating main thread throwing an exception");
		throw(exception("Algorithm exception"));
	} catch(...)
	{
		t1.join();
		throw;
	}

	t1.join();
}

class Algo
{
public:
	void operator()()
	{
		for (int i = 10; i > 0; --i)
		{
			int stime{ 10 };
			Util::print(__FUNCTION__, " ", i, " will sleep for ", stime, " ms");
			this_thread::sleep_for(chrono::milliseconds(stime));
		}
	}
};
void Examples::simple_thread_functor()
{
	Algo alg;
	thread t1(alg);
	// thread t1{ Algo() }; => Works with rvalue Algo
	// thread t1(Algo()); => This doesn't work: interpreted as a function named t1 that returns a thread
	// and the parameter is a function object that doesn't take any parameters and returns an Algo
	for (int i = 20; i > 0; --i)
	{
		int stime{ 10 };
		Util::print(__FUNCTION__, " ", i, " will sleep for ", stime, " ms");
		this_thread::sleep_for(chrono::milliseconds(stime));
	}

	Util::print(__FUNCTION__, " joining algo thread");
	t1.join();
}
