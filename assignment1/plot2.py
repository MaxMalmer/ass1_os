#!/usr/bin/python3
import time
import matplotlib as mpl
import matplotlib.pyplot as plt
import subprocess
import numpy as np
import random
import collections

PROGRAMNAME = "./task2"
GRAPHX = "Read-x"
GRAPHY = "Time(ms)"
PLOTNAME = "Task_2_Plot_BFQ"
FILENAME = PLOTNAME + ".png"

#iteration param not used
def run_tests(iterations):
    out = list()

    for i in range(0, iterations):
        start_time = time.time()
        print("Running program: " + PROGRAMNAME + " with options ")
        #command = ["/usr/bin/time", "-f", "%e", PROGRAMNAME]
        command = [PROGRAMNAME]
        pipe = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        pipe.wait()
        out.append(time.time() - start_time)
    return out
    


def make_and_save_plot(data,  plotfilename):
    mpl.style.use('seaborn-whitegrid')
    fig, ax = plt.subplots()

    ax.plot(data[0], data[1])

    ax.set_title(PLOTNAME)
    plt.ylabel(GRAPHY)
    plt.xlabel(GRAPHX)
    plt.savefig(plotfilename)


out = run_tests(10)
print(out)
out.sort()
make_and_save_plot([list(range(0,len(out))), out], FILENAME)
