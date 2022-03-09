#!/usr/bin/python3
from mailbox import linesep
import time
import matplotlib as mpl
import matplotlib.pyplot as plt
import subprocess
import random
import collections
import numpy as np

PROGRAMNAME = "make run_prime"
OPTIONS = [["-p50", "-sf"], ["-p50", "-sr"], ["-p50", "-sm"]]    #Add more lists to run several times
PROGRAMS = [["make run_fifo"], ["make run_robin"], ["make run_manual"]]
#OPTIONS = [["-p50", "-sf"]]
GRAPHX = "FIFO, Round Robin, Manual"
GRAPHY = "Thread per s"
PLOTNAME = "Prime_Factor_Throughput_Boxplot"
FILENAME = PLOTNAME + ".png"

def run_tests():
    output = list() 

    for program in PROGRAMS:
        out_lines = list()

        for i in range(0, 30):
            print("Running program: " + str(program))
            #command = ["/usr/bin/time", "-f", "%e", PROGRAMNAME] + optionlist
            command = [PROGRAMNAME]
            pipe = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            stdout, stderr = pipe.communicate()
            out_lines = out_lines + stdout.decode("utf-8").splitlines()
            err_lines = stderr.decode("utf-8").splitlines()
            out_lines.sort()
            out_lines.pop()
            print(out_lines)
            #filename = optionlist + "PLOT" + ".png"
            #make_and_save_plot([list(range(0,len(out_lines))), out_lines], filename)
            #output.append(list((range(1, len(out_lines) + 1))))
        
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


out, err = run_tests()
print(out)
#out.sort()
print(err)
make_and_save_plot(out, FILENAME)
