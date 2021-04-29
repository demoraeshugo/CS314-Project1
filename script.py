import os
import subprocess
from subprocess import Popen, PIPE, STDOUT
from os import path

WORKING_DIRECTORY = os.path.dirname(os.path.abspath(__file__))
TEST_DIRECTORY = WORKING_DIRECTORY + "/tests"
TINY_L_PATH = WORKING_DIRECTORY+"/tinyL.out"
OPT_OUT_PATH = WORKING_DIRECTORY+"/opt.out"

def prepareBinaries():
    if not path.exists("compile"):
        subprocess.call(["make","compile"])
    if not path.exists("optimize"):
        subprocess.call(["make","optimize"])
    if not path.exists("run"):
        subprocess.call(["make","run"])

def runAll():
    for filename in os.listdir(TEST_DIRECTORY):
        if filename.endswith(".tinyL"): 
            test_file_path = os.path.join(TEST_DIRECTORY, filename)
            subprocess.call(["./compile", test_file_path])
            tinyL_file = open(TINY_L_PATH)
            opt_file = open("opt.out", "w")
            Popen(["./optimize"], stdout=opt_file, stdin=tinyL_file, stderr=None)
            subprocess.call(["./run", OPT_OUT_PATH])

prepareBinaries()
runAll()

