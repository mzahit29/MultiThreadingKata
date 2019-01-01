// MultithreadingKata.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Examples.h"
#include "Util.h"

using namespace std;

int main()
{
	Util::print(__FUNCTION__, " begin");

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
	//Examples::dead_lock_solution();
	//Examples::dead_lock_solution_with_lg();
	//Examples::call_once();
	//Examples::producer_consumer();
	//Examples::producer_consumer_chain();
	//Examples::parent_thread_dead_before_child();
	//Examples::parent_thread_dead_before_child_child_holds_mutex();
	//Examples::parent_thread_destroys_mutex_whild_child_waits_to_lock();
	Examples::what_happens_to_detached_thread_when_parent_dies_before_it();


	Util::print(__FUNCTION__, " end");
}
