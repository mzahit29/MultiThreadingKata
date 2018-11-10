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
