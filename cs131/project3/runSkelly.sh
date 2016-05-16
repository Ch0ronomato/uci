#!/bin/bash
#
#$ -cwd
#$ -j y
#$ -S /bin/bash
#$ -M ischweer@uci.edu
#$ -o ./OutSkelly.out
#
# Use modules to setup the runtime environment
module load sge 
module load gcc/4.8.2
#
# Execute the run
#
./lab3 jjbesavi_timing_1.txt 2
./lab3 jjbesavi_timing_1.txt 4
./lab3 jjbesavi_timing_1.txt 8
./lab3 jjbesavi_timing_1.txt 16
./lab3 jjbesavi_timing_2.txt 2
./lab3 jjbesavi_timing_2.txt 4
./lab3 jjbesavi_timing_2.txt 8
./lab3 jjbesavi_timing_2.txt 16
./lab3 jjbesavi_timing_3.txt 2
./lab3 jjbesavi_timing_3.txt 4
./lab3 jjbesavi_timing_3.txt 8
./lab3 jjbesavi_timing_3.txt 16
