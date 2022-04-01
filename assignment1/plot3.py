#!/usr/bin/python3
from mailbox import linesep
import time
import matplotlib as mpl
import matplotlib.pyplot as plt
import subprocess
import random
import collections
import numpy as np

PROGRAMNAME = "./task2"
OPTIONS = [["-r0"], ["-r1"], ["-r2"]]    #Add more lists to run several times
PROGRAMS = [["./task2"], ["./task2"], ["./task2"]]
GRAPHX = "O_RDONLY, O_SYNC, O_DIRECT"
GRAPHY = "Waiting Time (ms)"
PLOTNAME = "Task3_Threaded_Waiting_Time_BFQ_Boxplot"
FILENAME = PLOTNAME + ".png"

def run_tests(iterations):
    output = list() 

    for option in OPTIONS:
        out_lines = list()

        for i in range(0, iterations):
            command = [PROGRAMNAME] + option 
            pipe = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            stdout, stderr = pipe.communicate()
            out_lines = out_lines + stdout.decode("utf-8").splitlines()
            err_lines = stderr.decode("utf-8").splitlines()
            print(out_lines)
            out_lines.sort()
            del out_lines[-1]
        
        output.append([float(x) for x in out_lines])

    return output, err_lines
    
def make_and_save_plot(data,  plotfilename):
    mpl.style.use('seaborn-whitegrid')
    #print(data)
    columns = [data[0], data[1], data[2]]
    fig, ax = plt.subplots()

    #ax.plot(data[0], data[1])
    ax.boxplot(columns)

    ax.set_title(PLOTNAME)
    plt.ylabel(GRAPHY)
    plt.xlabel(GRAPHX)
    plt.savefig(plotfilename)


out, err = run_tests(1)
print(out)
#out.sort()
print(err)
make_and_save_plot(out, FILENAME)
