echo off
set path=../\gnupack4etrbcn\app\cygwin\local\bin;../\gnupack4etrbcn\app\cygwin\cygwin\bin;C:\WORK\../\gnupack4etrbcn\app\cygwin\cygwin\usr\sbin
                                                 
del /Q *.rxe 
make all 

IF EXIST *.rxe sh rxeflash.sh