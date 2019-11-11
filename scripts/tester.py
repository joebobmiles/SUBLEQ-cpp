"""
@file tester.py
@author Joseph Miles <josephmiles2015@gmail.com>
@date 2019-07-21

This file is for performing automated tests of the SUBLEQ assembler and
emulator.
"""


import os, subprocess, struct, enum, sys, functools, inspect


BUILD_DIR = "build"
DATA_DIR  = "data\\tests"

ASSEMBLER = "build\\subleqc.exe"
EMULATOR  = "build\\subleq.exe"

TESTS_PASSED = 0
TESTS_ERRORED = 0


def infile(name):
    global DATA_DIR
    return DATA_DIR + "/" + name + ".sq"


def outfile(name):
    global BUILD_DIR
    return BUILD_DIR + "/" + name + ".x"


def build(input_file, output_file):
    """ Takes in a SUBLEQ filename as input and invokes the assembler on it,
    producing an output binary of similar name in the build directory. The
    function returns whether or not an error occurred and an output value.  If
    no error occurred, then the output value is the path to the generated
    binary. If an error occurred, then the output value is the message output
    by the assembler.
    """
    global ASSEMBLER

    result = subprocess.run([ ASSEMBLER,
                              input_file,
                              output_file
                            ],
                            shell=True,
                            capture_output=True)

    return result.stdout, result.returncode


def unpack(file):
    data = None

    with open(file, "rb") as f:
        data = [ i[0] for i in struct.iter_unpack("i", f.read()) ]

    return data


class TestResult():

    def __init__(self, name, success, messages = []):
        self.test = name
        self.success = success
        self.messages = messages

    def report(self):
        status = "PASSED" if self.success else "FAILED"

        message = f"==> Test \"{self.test}\" {status}"

        if self.success is not True:
            def prepend_tab(string):
                return "\t"+string

            message = message+"\n" + "\n".join(map(prepend_tab, self.messages))

        print(message)


class TestAssertion():

    def __init__(self, expected, actual, success, message):
        self.expected = expected
        self.actual = actual
        self.success = success
        self.message = message

    def report(self):
        status = "Failed to assert" if not self.success else "Asserted"
        return f"{status} that {self.message}"


class Test():

    def __init__(self, name, test):
        self.name = name
        self._test = test
        self._asserts = []
        self._errored = False

    def run(self):
        self._test(self)

        # Get all the messages from the assertions that failed.
        messages = [ a.report() for a in self._asserts if not a.success ]

        passing = len(messages) == 0

        return TestResult(self.name, passing and not self._errored, messages)

    def error(self, message):
        print(f"==> TEST ERROR: \t{message}")

    def is_equal(self, expected, actual):
        self._asserts.append(TestAssertion(
            expected,
            actual,
            expected == actual,
            f"{expected} == {actual}"
        ))


class Tester():

    def __init__(self):
        self._tests = []

    def add_test(self, test):
        self._tests.append(Test(test.__name__, test))

    def run(self):
        results = []

        for test in self._tests:
            result = test.run()
            result.report()

            results.append(result)

        failing_tests = len([ r for r in results if r.success is False ])

        if failing_tests > 0:
            passing_tests = len(self._tests) - failing_tests
            print(f"\n\t{passing_tests} / {len(self._tests)} tests passing.")

        else:
            print("\n\tAll tests passing.")



tester = Tester()


## ASSEMBLER TESTS

@tester.add_test
def basic(test):
    _, returncode = build(infile(test.name), outfile(test.name))

    if returncode:
        test.error(f"Build for {outfile(self)} exited with code {returncode}")

    test.is_equal([ 0, 0, -1 ], unpack(outfile(test.name)))

@tester.add_test
def next_address(test):
    _, returncode = build(infile(test.name), outfile(test.name))

    if returncode:
        test.error(f"Build for {outfile(self)} exited with code {returncode}")

    test.is_equal([ 1, 2, 3 ], unpack(outfile(test.name)))

@tester.add_test
def two_address(test):
    _, returncode = build(infile(test.name), outfile(test.name))

    if returncode:
        test.error(f"Build for {outfile(self)} exited with code {returncode}")

    test.is_equal([ 0, 1, 3 ], unpack(outfile(test.name)))

@tester.add_test
def one_address(test):
    _, returncode = build(infile(test.name), outfile(test.name))

    if returncode:
        test.error(f"Build for {outfile(self)} exited with code {returncode}")

    test.is_equal([ 1, 1, 3 ], unpack(outfile(test.name)))

@tester.add_test
def label(test):
    _, returncode = build(infile(test.name), outfile(test.name))

    if returncode:
        test.error(f"Build for {outfile(self)} exited with code {returncode}")

    test.is_equal([ 0, 0, -1], unpack(outfile(test.name)))

@tester.add_test
def identifier(test):
    _, returncode = build(infile(test.name), outfile(test.name))

    if returncode:
        test.error(f"Build for {outfile(self)} exited with code {returncode}")

    test.is_equal([ 0, 0, -1 ],  unpack(outfile(test.name)))

@tester.add_test
def complex(test):
    _, returncode = build(infile(test.name), outfile(test.name))

    if returncode:
        test.error(f"Build for {outfile(self)} exited with code {returncode}")

    test.is_equal([ 0, 1, 3, 0, 1, 6, 0, 0, -1 ], unpack(outfile(test.name)))


## EMULATOR TESTS
# todo(jrm): Write the emulator tests


# Run the tests
tester.run()

# todo(jrm) have tester.run() return the number of tests failed.
# sys.exit(TOTAL_TESTS - TESTS_PASSED)
