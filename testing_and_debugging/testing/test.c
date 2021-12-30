#include "../../ext/ctest_lib/include/ctest.h"

int main(void)
{
	assert_unsigned_integer_equal(10, 14, __LINE__, "Trivial test.");
}
