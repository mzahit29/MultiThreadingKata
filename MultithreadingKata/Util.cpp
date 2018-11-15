#include "pch.h"
#include "Util.h"

using namespace std;

std::mutex Util::mtx_;

int Util::factorial(int x)
{
	int result{ 1 };
	for (int i = x; i > 1; --i)
	{
		result *= i;
	}

	Util::print(__FUNCTION__, " Result of ", x, " factorial is ", result);
	return result;
}

void Util::simulate_work(string parent_func)
{
	for (int i = 20; i > 0; --i)
	{
		int stime{ 10 };
		Util::print(parent_func, " ", i, " will sleep for ", stime, " ms");
		this_thread::sleep_for(chrono::milliseconds(stime));
	}
}

void Util::count_up_to_limit(int limit, std::mutex& m)
{
	for (int i = 0; i < limit; ++i)
	{
		this_thread::sleep_for(chrono::microseconds(10));
		lock_guard<mutex> lg{ m };
		cout << "[" << this_thread::get_id() << "] " << i << endl;
	}
}

void Util::count_down_to_zero(int start, std::mutex& m)
{
	if (start <= 0) return;

	for (int i = start; i > 0; --i)
	{
		this_thread::sleep_for(chrono::microseconds(10));
		lock_guard<mutex> lg{ m };
		cout << "[" << this_thread::get_id() << "] " << i << endl;
	}
}

// When you start a thread with this function you give it an identifier t_id
// At first both threads enter and get blocked on cond.wait because predicate is false
// Then main thread sets predicate to true and calls notify_one() to trigger the thread with t_id=0
// t_id(0) thread runs and then switches turn to t_id(1) and calls notify_one() to trigger the other thread
// This keeps going until characte that is printed is Z
void Util::print_letter_turn_based(int t_id, bool& run_flag, int& turn, char& c, mutex& m, condition_variable& cond)
{
	char end_letter{ 'Z' };
	while (c <= end_letter)
	{
		unique_lock<mutex> ul{ m };
		
		cond.wait(ul, [&]()
		{
			// Only the thread with t_id == turn will pass through
			return run_flag && turn == t_id;
		});

		// Need this check because if last letter is printed by other thread, then c > end_letter and we have to break
		if (c > end_letter) break;
		Util::print(c);
		c++;

		// Switch t_id to the other thread, so that it runs next and not this thread again
		turn = (turn + 1) % 2;
		ul.unlock();
		cond.notify_one();
		this_thread::sleep_for(chrono::milliseconds(100));
	}
}

