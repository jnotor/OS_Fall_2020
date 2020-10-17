#!/bin/bash

#
# Sample shel script to run a program 10000 times.  You can pipe
# the output to a file to make a record you can examine later
#
# to capture both the standard output and the error output (where
# time sends its output to), you could do this:
#
# CHECK_MANY &> SAVEFILE
#

for i in {1..1000}
  do
    echo $i;
    time ./a.out
    echo
  done
