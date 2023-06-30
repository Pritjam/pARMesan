import sys
import os
import os.path as path
import colorama
from termcolor import colored
import filecmp


help_message = "To run a single test, use 'python3 run_tests.py TEST_NAME\nTo run all tests, run python3 run_tests.py --all"

test_dirs = []


if len(sys.argv) == 1:
  print(help_message)
  quit(0)

if "--all" in sys.argv:
  # we need to run all tests
  test_dirs = os.listdir("tests")
else:
  test_dirs = [sys.argv[1]]

colorama.init()

for test_name in test_dirs:
  test_dir_path = path.join("tests", test_name)
  if not path.exists(test_dir_path):
    print(colored("[ERROR] Test directory for test %s not found" % test_name, "yellow"))
    continue

  ok_file_path = path.join(test_dir_path, test_name + ".ok")
  if not path.exists(ok_file_path):
    print(colored("[ERROR] Correct output file for test %s not found" % test_name, "yellow",))
    continue

  memdump_path = path.join(test_dir_path, test_name + ".feta")
  
  if not path.exists(memdump_path):
    memdump_path = ""

  outfile_path = path.join("output", test_name + ".out")

  if not path.exists("output"):
    os.mkdir("output")

  cmd = './parmesan'
  if len(memdump_path):
    cmd += ' -m '
    cmd += memdump_path

  cmd += ' -q -v 1 '
  cmd += path.join(test_dir_path, test_name)
  cmd += ' > '
  cmd += outfile_path
  cmd += ' 2>&1 '
  os.system(cmd)

  # Now to compare the output
  if filecmp.cmp(ok_file_path, outfile_path, shallow=False):
    # If they're the same:
    print(colored("[PASS] %s" % test_name, 'green'))
  else:
    print(colored("[FAIL] %s" % test_name, 'red'))
  
		