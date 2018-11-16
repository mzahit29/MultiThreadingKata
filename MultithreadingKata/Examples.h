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
};

