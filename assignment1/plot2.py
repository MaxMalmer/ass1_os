#!/usr/bin/python3
import time
import matplotlib as mpl
import matplotlib.pyplot as plt
import subprocess
import numpy as np

import random
import collections

PROGRAMNAME = "./task2"
GRAPHX = "Thread-x"
GRAPHY = "Time(ms)"
PLOTNAME = "Task_2_plot_sync"
FILENAME = PLOTNAME + ".png"

#iteration param not used
def run_tests(iterations):
    print("Running program: " + PROGRAMNAME + " with options ")
    #command = ["/usr/bin/time", "-f", "%e", PROGRAMNAME]
    command = [PROGRAMNAME]
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
print(err)
make_and_save_plot([range(0,600, 100), out], FILENAME)
