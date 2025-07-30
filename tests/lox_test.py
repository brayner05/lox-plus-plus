import subprocess
import inspect
import os
import tempfile

LOX_PATH = "./bin"
SUCCESS_CODE = 0

ALL_TESTS = []
FAILED_TESTS = []


def test(function):
    ALL_TESTS.append(function)
    return function


def run_tests():
    for test in ALL_TESTS:
        test()


def lox_execute(lox_code):
    lox_executable = f"{LOX_PATH}/loxpp"

    # Create a temporary file with the Lox code
    with tempfile.NamedTemporaryFile(mode="w", suffix=".lox", delete=False) as tmp:
        tmp.write(lox_code)
        tmp_path = tmp.name

    try:
        result = subprocess.run(
            [lox_executable, tmp_path],
            text=True,
            capture_output=True
        )
        return result.stdout.strip() or result.stderr.strip()
    finally:
        os.remove(tmp_path)



def lox_evaluate(lox_expr):
    return lox_execute(f"print ({lox_expr});")


def lox_assert(lox_expr, expected_output, message=""):
    real_output = lox_evaluate(lox_expr)

    if real_output == expected_output:
        print("[ \033[92mPASSED\033[0m ]")
        return
    
    current_function = inspect.currentframe().f_code.co_name
    FAILED_TESTS.append(lox_expr)

    print(f"[ \033[91mFAILED\033[0m ]")
    print(f"\tExpected: {expected_output}")
    print(f"\tFound: {real_output}")
