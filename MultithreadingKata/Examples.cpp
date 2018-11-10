#include "pch.h"
#include "Examples.h"
#include <iostream>
#include <thread>
#include "Util.h"

using namespace std;

void Examples::simple_thread_lambda()
{
	Util::th_info(__FUNCTION__);

	cout << "You can start a tread with a lambda expression" << endl;
	thread t1{[]()
	{
		Util::th_info(__FUNCTION__);
	} };

	// Wait for t1 to finish in main thread
	t1.join();


	thread{ []()
	{
		Util::th_info(__FUNCTION__);
	} }.detach();

	thread&& t2 = thread{ []()
	{
		Util::th_info(__FUNCTION__);
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
			Util::th_info(__FUNCTION__, " ", i, " Main thread will sleep for ", 200, " ms");
			this_thread::sleep_for(chrono::milliseconds(200));
		}
		Util::th_info(__FUNCTION__, " Simulating main thread throwing an exception");
		throw(exception("Algorithm exception"));
	} catch(std::exception& e)
	{
		t1.join();
		throw;
	}

	t1.join();
}
