#pragma once
class Examples
{
public:
	Examples();
	~Examples();

	static void simple_thread_lambda();
	static void simple_thread_function();
	static void simple_thread_functor();
	static void simple_thread_functor_pass_value();
	static void race_condition_on_cout();
	static void race_condition_on_cout_with_cond_var();
	static void race_condition_on_cout_without_cond_var();
	static void dead_lock();
	static void dead_lock_solution();
	static void dead_lock_solution_with_lg();
	static void call_once();
	static void producer_consumer();
	static void producer_consumer_chain();
	static void parent_thread_dead_before_child();
	static void parent_thread_dead_before_child_child_holds_mutex();
	static void parent_thread_destroys_mutex_whild_child_waits_to_lock();
	static void what_happens_to_detached_thread_when_parent_dies_before_it();
};

