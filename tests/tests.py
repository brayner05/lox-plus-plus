from lox_test import test, lox_assert, run_tests

@test
def test_expressions():
    lox_assert("20 + 20", "40")
    lox_assert("true or false", "true")
    lox_assert("true ? \"hello\" : nil", "hello")
    lox_assert("false ? \"hello\" : nil", "nil")


if __name__ == "__main__":
    run_tests()
