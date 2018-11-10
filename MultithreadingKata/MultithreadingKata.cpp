// MultithreadingKata.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "Examples.h"

using namespace std;

int main()
{
	std::cout << "Main begin\n";

	Examples::simple_thread_lambda();
	try
	{
		Examples::simple_thread_function();
	} catch(std::exception& e)
	{
		cout << e.what() << endl;
	}
	Examples::simple_thread_functor();

	std::cout << "Main end\n";
}
