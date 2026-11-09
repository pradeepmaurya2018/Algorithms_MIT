#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>

int add(int a, int b) {
    return a + b;
}
int mul(int b, int c) {
    return b*c;
}

BOOST_AUTO_TEST_CASE(test_add) {
    BOOST_CHECK_EQUAL(add(2, 3), 5);
}
BOOST_AUTO_TEST_CASE(test_mul) {
    BOOST_CHECK_EQUAL(mul(3,6), 18);
}