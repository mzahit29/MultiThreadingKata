// MultithreadingKata.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "Examples.h"

using namespace std;

int main()
{
	std::cout << "Main begin\n";

	//Examples::simple_thread_lambda();
	//try
	//{
	//	Examples::simple_thread_function();
	//} catch(std::exception& e)
	//{
	//	cout << e.what() << endl;
	//}
	//Examples::simple_thread_functor();
	//Examples::simple_thread_functor_pass_value();
	//Examples::race_condition_on_cout();
	//Examples::race_condition_on_cout_with_cond_var();
	//Examples::race_condition_on_cout_without_cond_var();
	//Examples::dead_lock();
	Examples::dead_lock_solution();

	std::cout << "Main end\n";
}
