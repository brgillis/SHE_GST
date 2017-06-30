""" @file /disk2/brg/Program_Files/workspace/Generate_GalSim_Images/utility/timing.py

    Created 5 Feb 2016

    @TODO: File docstring

    ---------------------------------------------------------------------

    Code by Paul McGuire on Stack Overflow
    (http://stackoverflow.com/a/1557906/5099457)
"""

import atexit
from time import clock

def secondsToStr(t):
    return "%d:%02d:%02d.%03d" % \
        reduce(lambda ll, b : divmod(ll[0], b) + ll[1:],
            [(t * 1000,), 1000, 60, 60])

line = "="*40
def log(s, elapsed=None):
    print line
    print secondsToStr(clock()), '-', s
    if elapsed:
        print "Elapsed time:", elapsed
    print line
    print

def endlog():
    end = clock()
    elapsed = end - start
    log("End Program", secondsToStr(elapsed))

def now():
    return secondsToStr(clock())

start = clock()
atexit.register(endlog)
log("Start Program")
