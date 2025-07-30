import subprocess
import os

SCRIPTS_DIR = "./scripts"
LOX_PATH = "./bin"

SUCCESS_CODE = 0

def execute_script(script):
    print(f"Running script: {script}")
    exit_code = subprocess.call([LOX_PATH + "/" + "loxpp", SCRIPTS_DIR + "/" + script])
    return exit_code


if __name__ == "__main__":
    lox_scripts = os.listdir(SCRIPTS_DIR)

    failed = []

    for script in lox_scripts:
        result = execute_script(script)
        print("========================")

        if result == SUCCESS_CODE:
            print("\t\033[92mSUCCESS\033[0m")
        else:
            failed.append(script)
            print("\t\033[91mFAILURE\033[0m")

        print("========================")
        print("\n")

    print(f"Ran {len(lox_scripts)} scripts.")
    print(f"\033[92mPassed\033[0m: {len(lox_scripts) - len(failed)}")
    print(f"\033[91mFailed\033[0m: {len(failed)}")

    for script in failed:
        print(f"\033[91m{script}\033[0m")