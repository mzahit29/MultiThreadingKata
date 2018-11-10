// MultithreadingKata.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "Examples.h"

using namespace std;

int main()
{
	std::cout << "Main begin\n";

	try
	{
		Examples::simple_thread_lambda();
		Examples::simple_thread_function();
	} catch(std::exception& e)
	{
		cout << e.what() << endl;
	}

	std::cout << "Main end\n";
}
