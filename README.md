# ass1_os
The first assignment of the OS course at Umeå University

# Operating Systems Course - Assignment 1

The purpose of this assignment is to familiarize you with the Linux kernel and scheduler theory by experimentally evaluating how different scheduling algorithms perform in the Linux kernel context. How I/O operations impact performance will also be investigated.

The assignment is to be done in **groups of two**, it is suggested but not required that these groups remain the same for the course project.

A passing grade on this assignment is mandatory to pass the course.

# Linux Scheduler (introduction to Task 1)

The highlight of any OS is the scheduler. Linux comes with a number of different
scheduling algorithms (policies in Linux terminology) that can be set.
See the [man pages](http://man7.org/linux/man-pages/man7/sched.7.html).
The supported schedulers include deadline, Round Robin, FIFO, CFS, among
others.
There are even more algorithms available from hackers and linux gurus, such as the [Linux Brain-Fuck Scheduler](https://en.wikipedia.org/wiki/Brain_Fuck_Scheduler) (see [also](http://ck.kolivas.org/patches/bfs/)).
The scheduling policy has impact on the performance of an application.
Depending on the application type, you can gain/lose a lot when it comes to performance.


## Performance evaluation

When comparing different scheduling policies for a given application, there are multiple metrics that are usually considered, for example:

1. Throughput: How many units of work/threads/processes finish per unit time?
2. Latency: On average, how long does it take for an application to finish?
3. Tail latency: Sorting applications by how long they take, how long does it take for, e.g., the application that is slower than 95% of all other applications, to finish?
4. Waiting time: How long does an application stay in the Waiting queue on average?
5. Distributions: Plotting the distributions of the above numbers, instead of providing single numbers, e.g., the average or the 95th percentile latency.

The above set are just an example, but there are many more that can be considered.


# Task 1

Write a multi-threaded program to test and compare two or more of the Linux CPU scheduling policies available in the main Linux kernel or by others.
Your program should run in user space.
You should include performance metrics that make sense for your choice of the benchmarked algorithms, and you do not need to confine yourself to the ones above.
Your program should be adequate to what you are trying to test and should be capable of running for time sufficient to draw conclusions.
For inspiration, take a look at [http://cs.unm.edu/~eschulte/classes/cs587/data/bfs-v-cfs_groves-knockel-schulte.pdf](http://cs.unm.edu/~eschulte/classes/cs587/data/bfs-v-cfs_groves-knockel-schulte.pdf), you may not use latt.c.

# Task 2

Write a program to test and compare how I/O operations are impacted by the presence or absence of synchronization and caching.
You should include performance metrics that make sense for the task.
Your program should run in user space and be adequate to what you are trying to test and should be capable of running for time sufficient to draw conclusions.

# Linux I/O Scheduling (introduction to Task 3)

You have seen how process scheduling algorithms affect your performance in task 1, let us now look at a different part of the kernel that can significantly affect your performance, the I/O Scheduler!

Simply sending out requests to the block devices in the order that the kernel issues them, as soon as it issues them, results in poor performance.
One of the slowest operations in a modern computer is disk seeks.
Each seek—positioning the hard disk’s head at the location of a specific block—takes many milliseconds.
Minimizing seeks is absolutely crucial to the system’s performance.

The I/O scheduler divides the resource of disk I/O among the pending block I/O requests in the system. It does this through the merging and sorting of pending requests in the request queue.

An I/O scheduler works by managing a block device’s request queue.
It decides the order of requests in the queue and at what time each request is dispatched to the block device. It manages the request queue with the goal of reducing seeks, which results in greater global throughput.
The modifier “global” here is important.
An I/O scheduler, very openly, is unfair to some requests at the expense of improving the overall performance of the system.
I/O schedulers perform two primary actions to minimize seeks: merging and sorting.
Merging is the coalescing of two or more requests into one.

# Task 3

Describe three of the different Linux I/O schedulers.
For at least two of the three policies, describe how you think we can improve the policy and optimize the performance even more. Make sure to provide a specific use-case where such an improved policy would be valuable.
Run the test suite in **Task 2** using at least two different I/O schedulers.
Explain any observed differences or reason about the lack thereof.
Describe scenarios where a certain scheduler may be preferable over another.

# Useful information

Note that changing a scheduling policy in a virtual machine does not necessarily change the scheduling policy of the machine where the virtual machine is running. (Rhetorically: If this was the case, what kind of problems could arise?)

# Deliverables

Deliver a well written report with the following information:
- Theory.
- Hypothesis.
- Method.
- Results.

These sections should contain information covering the solution of all tasks.

You are not expected to drown the reader in words or figures, rather you are expected to answer the presented questions precisely and to provide additional information that you find interesting.

Make sure to include information about what hardware you ran the tests on.

The code should be available through a Git repository, via the [departments Gitlab](https://git.cs.umu.se) or other Git hosting services such as Github. Include a link in your report, and make sure to grant me read permissions on the repository.

The assignment is to be done *in pairs of two*.

Please note that being in groups for this assignment will not necessarily mean that you will write half as much on the report as you would have if you were alone. The expectations on the quality of the report will simply be higher. Once again quality does **not** equal quantity.

# Deadline

See examination tab in Cambro for deadlines.

Using [labres](https://webapps.cs.umu.se/labresults/v2/handin.php?courseid=494) for submissions.
