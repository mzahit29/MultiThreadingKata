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
