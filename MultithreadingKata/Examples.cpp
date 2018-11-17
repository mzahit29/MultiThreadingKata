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

class MathF
{
public:
	void operator()(string& msg)
	{
		Util::print(__FUNCTION__, " ", msg);
		msg = "sen olmedin sonsuza kadar kalbimizdesin";
	}
};
void Examples::simple_thread_functor_pass_value()
{
	string msg{ "Atam izindeyiz" };
	thread t1{ MathF(), std::ref(msg) };
	
	Util::simulate_work(__FUNCTION__);

	t1.join();

	Util::print(__FUNCTION__, " ", msg);

}

void Examples::race_condition_on_cout()
{
	// t1 and t2 are synchronizing the access to shared resource cout over mutex m
	std::mutex m;
	thread t1(Util::count_down_to_zero, 100, std::ref(m));
	thread t2(Util::count_up_to_limit, 100, std::ref(m));

	t1.join();
	t2.join();

}

void Examples::race_condition_on_cout_with_cond_var()
{
	// t1 will print 'a' and notify t2 to print the next letter
	char c{ 'A' };
	bool run_flag{ false };
	mutex m;
	condition_variable cond;
	int next_thread{ 0 };
	thread t1(Util::print_letter_turn_based, 0, std::ref(run_flag), std::ref(next_thread), std::ref(c), std::ref(m), std::ref(cond));
	thread t2(Util::print_letter_turn_based, 1, std::ref(run_flag), std::ref(next_thread), std::ref(c), std::ref(m), std::ref(cond));

	// Now both threads are waiting on cond, set run_flag and wake one of them up from main thread
	// The first thread to enter synchronized section, will finish its work on the shared resource and
	// will trigger the other thread waiting on that cond var
	run_flag = true;
	cond.notify_one();

	t1.join();
	t2.join();
}

// Threads print the character C, increment C, unlock mutex and then sleep for 100 qs
// Our aim is t1 prints one letter, and then the next letter is printed by t2
// BUG: But sometimes same thread writes two consecutive characters!
void Examples::race_condition_on_cout_without_cond_var()
{
	char C{ 'A' };
	mutex m;

	thread t1{[&m, &C]()
	{
		while(C <= 'Z')
		{
			unique_lock<mutex> ul(m);
			if (C > 'Z') break;
			Util::print(C);
			C++;
			ul.unlock();
			
			this_thread::sleep_for(chrono::milliseconds(100));
		}
	} };

	thread t2{[&m, &C]()
	{
		while(C <= 'Z')
		{
			unique_lock<mutex> ul(m);
			if (C > 'Z') break;
			Util::print(C);
			C++;
			ul.unlock();

			this_thread::sleep_for(chrono::milliseconds(100));
		}
	} };

	t1.join();
	t2.join();
}

void Examples::dead_lock()
{
	mutex m1, m2;

	// Locking order of m1 and m2 has to be the same in all threads
	// o.w. deadlock occurs
	thread t1([&]()
	{
		m1.lock();
		Util::print("Locked m1");
		m2.lock();
		Util::print("Locked m2");
	});
	thread t2([&]()
	{
		m2.lock();
		Util::print("Locked m2");
		m1.lock();
		Util::print("Locked m1");
	});

	t1.join();
	t2.join();
}

void Examples::dead_lock_solution()
{
	mutex m1, m2;

	// Locking order of m1 and m2 has to be the same in all threads
	// o.w. deadlock occurs
	thread t1([&]()
	{
		m1.lock();
		Util::print("Locked m1");
		m2.lock();
		Util::print("Locked m2");
		m1.unlock();
		m2.unlock();
	});
	thread t2([&]()
	{
		m1.lock();
		Util::print("Locked m1");
		m2.lock();
		Util::print("Locked m2");
		m1.unlock();
		m2.unlock();
	});

	t1.join();
	t2.join();
}

void Examples::dead_lock_solution_with_lg()
{
	mutex m1, m2;
	thread t1([&]()
	{
		// Standard library lock function has deadlock avoidance mechanism
		// Even though we are giving the mutexes in reverse order,
		// deadlock doesn't happen
		lock(m1, m2);
		lock_guard<mutex> lg1(m1, adopt_lock);
		lock_guard<mutex> lg2(m2, adopt_lock);
		// Do stuff
	});
	thread t2([&]()
	{
		lock(m2, m1);
		lock_guard<mutex> lg2(m2, adopt_lock);
		lock_guard<mutex> lg1(m1, adopt_lock);
		// Do stuff
	});

	t1.join();
	t2.join();
}

void Examples::call_once()
{
	once_flag flag1, flag2, flag3;

	thread t1([&]()
	{
		std::call_once(flag1, &Util::print<string>, "All these functions will be called only once");
		this_thread::sleep_for(chrono::seconds(1));
		std::call_once(flag2, &Util::factorial, 4);
		std::call_once(flag3, []()
		{
			Util::print("This is called only once, even though this block is in both t1 and t2");
		});
	});

	thread t2([&]()
	{
		std::call_once(flag1, &Util::print<string>, "All these functions will be called only once");
		std::call_once(flag2, &Util::factorial, 4);
		std::call_once(flag3, []()
		{
			Util::print("This is called only once, even though this block is in both t1 and t2");
		});
	});

	t1.join();
	t2.join();
}
