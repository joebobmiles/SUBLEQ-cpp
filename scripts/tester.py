"""
@file tester.py
@author Joseph Miles <josephmiles2015@gmail.com>
@date 2019-07-21

This file is for performing automated tests of the SUBLEQ assembler and
emulator.
"""


import os, subprocess, struct, enum


BUILD_DIR = "build"
DATA_DIR  = "data\\tests"

ASSEMBLER = "build\\subleqc.exe"
EMULATOR  = "build\\subleq.exe"

TESTS_PASSED = 0
TESTS_ERRORED = 0


def build(input_file, output_file):
    """ Takes in a SUBLEQ filename as input and invokes the assembler on it,
    producing an output binary of similar name in the build directory. The
    function returns whether or not an error occurred and an output value.  If
    no error occurred, then the output value is the path to the generated
    binary. If an error occurred, then the output value is the message output
    by the assembler.
    """
    result = subprocess.run([ ASSEMBLER,
                              input_file,
                              output_file
                            ],
                            shell=True,
                            capture_output=True)

    return result.stdout, result.returncode


##
## ASSEMBLER TESTS
##

# TODO[joe] Create tests that confirm assembler error messages?
# These would include usage errors and syntax errors. This functionality would
# be desirable at some point, when we get around to implementing syntax error
# checking. However, this is sufficient for now.
assembler_tests = {
    'basic': [ 0, 0, -1 ],
    'next_address': [ 1, 2, 3 ],
    'complex': [ 0, 1, 3, 0, 1, 6, 0, 0, -1 ],
    'two_address': [ 0, 1, 3 ],
    'one_address': [ 1, 1, 3 ],
}


for test, expected_result in assembler_tests.items():
    test_file = DATA_DIR+"\\"+test+".sq"
    output_file = test_file.replace(".sq", ".x").replace(DATA_DIR, BUILD_DIR)

    output, returncode = build(test_file, output_file)

    if returncode:
        if output:
            print(output.decode("utf-8"))

        print("==> ERROR: Build for \"{}\" exited with code {}".format(test_file,
                                                                       returncode))

        TESTS_ERRORED = TESTS_ERRORED + 1
        
        continue

    program = []

    with open(output_file, "rb") as program_file:
        program = [ i[0] for i in struct.iter_unpack("i", program_file.read()) ]

    if not program == expected_result:
        print("==> FAILED: \"{}\" ".format(test_file))

    else:
        print("==> PASSED: \"{}\" ".format(test_file))

        TESTS_PASSED = TESTS_PASSED + 1


##
## EMULATOR TESTS
##

emulator_tests = {}


##
## Test Statistics
##

# Total tests passed vs total tests run.
print("\n  {} / {} tests passed".format(TESTS_PASSED, 
                                        len(assembler_tests)+len(emulator_tests)))

# Total tests that errored.
print("  {} tests errored".format(TESTS_ERRORED))
