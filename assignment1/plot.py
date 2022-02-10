#!/usr/bin/python3
import time
import matplotlib as mpl
import matplotlib.pyplot as plt
import subprocess
import random
import collections
import numpy as np

PROGRAMNAME = "./prime_factor"
OPTIONS = [["-p50", "-sf"]]    #Add more lists to run several times
GRAPHX = "Thread-x"
GRAPHY = "Time(ms)"
PLOTNAME = "Prime_Factor_FIFO_1_Sorted"
FILENAME = PLOTNAME + ".png"

#iteration param not used
def run_tests(iterations):
    for optionlist in OPTIONS:
        print("Running program: " + PROGRAMNAME + " with options " + str(optionlist))
        #command = ["/usr/bin/time", "-f", "%e", PROGRAMNAME] + optionlist
        command = [PROGRAMNAME] + optionlist
        pipe = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        stdout, stderr = pipe.communicate()
        out_lines = stdout.decode("utf-8").splitlines()
        err_lines = stderr.decode("utf-8").splitlines()

        return out_lines, err_lines
    


def make_and_save_plot(data,  plotfilename):
    mpl.style.use('seaborn-whitegrid')
    fig, ax = plt.subplots()

    ax.plot(data[0], data[1])

    ax.set_title(PLOTNAME)
    plt.ylabel(GRAPHY)
    plt.xlabel(GRAPHX)
    plt.savefig(plotfilename)


out, err = run_tests(10)
print(out)
out.sort()
print(err)
make_and_save_plot([list(range(0,len(out))), out], FILENAME)
