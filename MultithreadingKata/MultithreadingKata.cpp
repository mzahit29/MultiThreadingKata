// MultithreadingKata.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "Examples.h"


int main()
{
	std::cout << "Main begin\n";


	Examples::simple_thread_lambda();

	std::cout << "Main end\n";
}
