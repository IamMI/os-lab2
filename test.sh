#!/bin/bash

touch record.txt
echo  "Input/o0075:" >> record.txt
echo  "Serial Version:" >> record.txt 
{ time ./life-serial 10000 input/o0075 > /dev/null ;} 2>> record.txt
echo "Parallel Version:" >> record.txt
{ time ./life-parallel-3threads 10000 input/o0075 > /dev/null ;} 2>> record.txt 

echo "==============================================================" >> record.txt
echo "Input/23334:" >> record.txt
echo "Serial Version:" >> record.txt 
{ time ./life-serial 10 input/23334m > /dev/null ;} 2>> record.txt 
echo "Parallel Version:" >> record.txt
{ time ./life-parallel-3threads 10 input/23334m > /dev/null ;} 2>> record.txt 

