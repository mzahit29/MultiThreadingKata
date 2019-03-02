#include "pch.h"
#include "Examples.h"
#include <iostream>
#include <fstream>
#include <thread>
#include "Util.h"
#include <queue>
#include <future>

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

void Examples::producer_consumer()
{
	mutex q_mtx_;
	condition_variable cond_q_;
	queue<int> q_;

	// t1 = producer
	// t2 = consumer 1
	// t3 = consumer 2
	thread t1{ [&]()
	{
		for (int i = 0; i < 10; ++i)
		{
			unique_lock<mutex> ul(q_mtx_);

			Util::print(__FUNCTION__, "pushing to queue: ", i);
			q_.push(i);
			ul.unlock();

			cond_q_.notify_one();
			this_thread::sleep_for(chrono::milliseconds(100));
		}
	} };

	thread t2{ [&]()
	{
		while (true)
		{
			cv_status timeout{ cv_status::no_timeout };

			unique_lock<mutex> ul(q_mtx_);
			while(q_.empty())
			{
				const auto status = cond_q_.wait_for(ul, chrono::seconds(1));
				if (status == cv_status::timeout)
				{
					timeout = cv_status::timeout;
					break;
				}
			}

			if (timeout == cv_status::no_timeout)
			{
				Util::print(__FUNCTION__, "Received data from queue: ", q_.front());
				q_.pop();
				ul.unlock();
			}
			else // timed out
			{
				Util::print(__FUNCTION__, "No data received in 1 second, exiting thread");
				break;
			}
		}
	} };

	thread t3{ [&]()
	{
		while (true)
		{
			unique_lock<mutex> ul(q_mtx_);
			const bool q_not_empty = cond_q_.wait_until(ul, 
				chrono::system_clock::now() + chrono::seconds(1), 
				[&q_]() { return !q_.empty(); });

			if (q_not_empty)
			{
				Util::print(__FUNCTION__, "Received data from queue: ", q_.front());
				q_.pop();
				ul.unlock();
			}
			else // q is empty for longer than 1 second
			{
				Util::print(__FUNCTION__, "No data received in 1 second, exiting thread");
				break;
			}
		}
	} };
	
	t1.join();
	t2.join();
	t3.join();
}

void Examples::producer_consumer_chain()
{
	queue<int> q_;
	mutex q_mtx_;
	condition_variable cond_q_1_;
	condition_variable cond_q_2_;
	int tid_last_consumer{ 1 };

	thread t1([&]()
	{
		for (int i = 0; i < 100; ++i)
		{
			unique_lock<mutex> ul(q_mtx_);
			Util::print(__FUNCTION__, " Pushing ", i, " to queue");
			q_.push(i);
			ul.unlock();
			cond_q_1_.notify_one();
			this_thread::sleep_for(chrono::milliseconds(50));
			
		}
	});

	thread t2([&]()
	{
		while(true)
		{
			int me{ 2 };
			unique_lock<mutex> ul(q_mtx_);
			if(cond_q_1_.wait_until(ul, chrono::system_clock::now() + chrono::milliseconds(100), 
				[&]() { return tid_last_consumer != me && !q_.empty(); }))
			{
				const auto data = q_.front();
				Util::print(__FUNCTION__, "Data received on queue: ", data);
				tid_last_consumer = me;
				ul.unlock();
				cond_q_2_.notify_one();
				if (data % 7 == 0)
				{
					Util::print(__FUNCTION__, data, " is divisible by 7");
				}
			}
			else
			{
				if (tid_last_consumer == me) // still last consumer is me
				{
					Util::print(__FUNCTION__, "Timeout: Data not processed by t3 during wait");
					Util::print(__FUNCTION__, "Will wait for t3");
				}
				if (q_.empty())
				{
					Util::print(__FUNCTION__, "Timeout: Queue is empty during wait");
					Util::print(__FUNCTION__, "Exiting thread");
					break;
				}
			}
		}
	});

	thread t3([&]()
	{
		while(true)
		{
			int me{ 3 };
			unique_lock<mutex> ul(q_mtx_);
			if (cond_q_2_.wait_until(ul, chrono::system_clock::now() + chrono::milliseconds(100),
				[&](){return tid_last_consumer == 2 && !q_.empty(); }))
			{
				const auto data = q_.front();
				Util::print(__FUNCTION__, "Data received on queue: ", data);
				q_.pop();
				tid_last_consumer = me;
				ul.unlock();
			}
			else
			{
				if (tid_last_consumer == me) // still last consumer is me
				{
					Util::print(__FUNCTION__, "Timeout: Data not processed by t2 during wait");
					Util::print(__FUNCTION__, "Will wait for t2");
				}
				if (q_.empty())
				{
					Util::print(__FUNCTION__, "Timeout: Queue is empty during wait");
					Util::print(__FUNCTION__, "Exiting thread");
					break;
				}
			}
		}
	});

	t1.join();
	t2.join();
	t3.join();
}

void Examples::parent_thread_dead_before_child()
{
	// Parent thread t1, creates a child thread t2 and detaches it
	// Exits before waiting for its detached child
	// Observe what happens:
	// 1- Program doesn't crash
	// 2- Since parent exits before child, child last message is lost
	// 
	thread t_parent([]()
	{
		mutex m;
		Util::print(__FUNCTION__, "Creating child thread from parent thread");
		thread t_child([]()
		{
			Util::print(__FUNCTION__, "Inside child thread");
			this_thread::sleep_for(chrono::seconds(1));
			Util::print(__FUNCTION__, "Child thread woke up after 1 seconds");
		});
		Util::print(__FUNCTION__, "Detaching child thread");
		t_child.detach();

		Util::print(__FUNCTION__, "Exiting parent thread, not waiting for child thread");
	});

	t_parent.join();
}

void Examples::parent_thread_dead_before_child_child_holds_mutex()
{
	// Parent thread t1 has a mutex, creates a child thread t2 and detaches it
	// Exits before waiting for its detached child.
	// Observe what happens:
	// 1- Program crashes:
	//    a) Problem here is child thread holds the mutex, while parent thread has joined
	//       and its resources (i.e. mutex) have been destroyed. Hence the error:
	//       "Mutex destroyed while busy".
	// 2- Since parent exits before child, child last message is lost
	thread t_parent([]()
	{
		mutex m;
		Util::print(__FUNCTION__, "Creating child thread from parent thread");
		thread t_child([&m]()
		{
			lock_guard<mutex> lg(m);
			Util::print(__FUNCTION__, "Inside child thread");
			this_thread::sleep_for(chrono::seconds(1));
			Util::print(__FUNCTION__, "Child thread woke up after 1 seconds");
		});
		Util::print(__FUNCTION__, "Detaching child thread");
		t_child.detach();

		Util::print(__FUNCTION__, "Exiting parent thread, not waiting for child thread");
	});

	t_parent.join();
}

// Parent thread creates an object that has a mutex inside it.
// Locks the mutex
// Starts a child thread passes the mutex reference to it
// Child thread sleeps for 1 second
// While child sleeps
//   a) parent unlocks mutex
//   b) parent destroys mutex
//   c) parent blocks waiting for child thread to finish
//   d) child wakes up and tries to call mtx->lock() but mutex has been destroyed
//       RESULT => Read access violation at mtx_ref
void Examples::parent_thread_destroys_mutex_whild_child_waits_to_lock()
{
	struct mtx_wrapper
	{
		mutex mtx_;
		mtx_wrapper() { Util::print("mtx_wrapper is being constructed"); }
		~mtx_wrapper() { Util::print("mtx_wrapper is being destructed"); }
	};

	//shared_ptr<mtx_wrapper> mt = shared_ptr<mtx_wrapper>(new mtx_wrapper); => make_shared is better performance wise
	shared_ptr<mtx_wrapper> mt = std::make_shared<mtx_wrapper>();
	Util::print("Parent locking mutex");
	mt->mtx_.lock();
	
	mutex& mtx_ref = mt->mtx_;  // pass the mutex inside the wrapper object to the child thread
	thread t_child([&mtx_ref]()
	{
		Util::print("Child thread entered");
		this_thread::sleep_for(chrono::seconds(1));
		mtx_ref.lock();
		Util::print("Child thread locked mutex");
		
		Util::print("Child thread exiting");
	});

	Util::print("Parent unlocking mutex");
	mt->mtx_.unlock();
	Util::print("Parent destroying mutex wrapper");
	mt.reset(); // Mutex destroyed before child thread tried to lock

	Util::print("Parent thread exiting");

	t_child.join();
}

void Examples::what_happens_to_detached_thread_when_parent_dies_before_it()
{
	thread t1{[](){
		const string file_name{"what_happens to a thread when parent exits.txt"};
		ofstream file{ file_name, ios::trunc };

		for (int i = 0; i < 60; ++i)
		{
			this_thread::sleep_for(chrono::seconds(1));
			file << "i: " << i << endl;
		}
	} };

	// Detaching child thread. Parent thread will exit before it.
	// As soon as parent exits child thread is owned by std runtime, and it is terminated
	// You can check that it is terminated from the number of outputs in the text file.
	// It was supposed to write a line per second for 60 seconds but it has stopped at line 28
	// since around that time the parent has exited.
	t1.detach();
	this_thread::sleep_for(chrono::seconds(30));
}

void Examples::testing_future_with_std_async()
{
	// Main calls factorial with async

	int x{ 5 };
	// Method 1: We do not know when runtime will actually call the factorial
	// because we haven't explicitly specified launch type. 
	std::future<int> result = std::async(&Util::factorial, x);
	// Method 2: We are deferring the runtime to call factorial only when future's get is called
	// Infact this means factorial is running in the same thread.
	//std::future<int> result = std::async(std::launch::deferred, &Util::factorial, x);
	// Method 3: We are running the factorial in a separate thread in parallel, 
	// long before the future's get is called
	//std::future<int> result = std::async(std::launch::async, &Util::factorial, x);

	this_thread::sleep_for(chrono::seconds(2));

	cout << "Factorial(" << x << ") is : " << result.get() << endl;
}

void Examples::testing_promise_with_std_async()
{
	std::promise<int> promise;
	std::future<int> future = promise.get_future();

	std::future<int> result = std::async(std::launch::async, &Util::factorial_waiting_promise, std::ref(future));

	// Main thread going to sleep, during this time the factorial_waiting_for_promise 
	// will be blocked on future.get because it is not yet set by main thread
	// After a while parent maybe does some calculations and finally gets the value to set the promise
	Util::print("Parent thread sleeps, simulating some calculation");
	this_thread::sleep_for(chrono::seconds(2));

	promise.set_value(5);
}

void Examples::testing_promise_with_std_async_exception()
{
	std::promise<int> promise;
	std::future<int> future = promise.get_future();

	std::future<int> result = std::async(std::launch::async, &Util::factorial_waiting_promise, std::ref(future));

	// Main thread going to sleep, during this time the factorial_waiting_for_promise 
	Util::print("Parent thread sleeps, simulating some calculation");
	this_thread::sleep_for(chrono::seconds(2));

	// After some time main thread fails to set the promise and notifies the child by
	// setting promise's exception
	promise.set_exception(std::make_exception_ptr(
		std::runtime_error("Main thread failed to set the promise")));
}
