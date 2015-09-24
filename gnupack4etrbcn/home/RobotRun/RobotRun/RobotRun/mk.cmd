
echo off 
set path=D:\ROBOT\et_robocon_devenv\gnupack4etrbcn\app\cygwin\local\bin;D:\ROBOT\et_robocon_devenv\gnupack4etrbcn\app\cygwin\cygwin\usr\sbin;D:\ROBOT\et_robocon_devenv\gnupack4etrbcn\app\cygwin\cygwin\bin
del /Q *.rxe 
make all 

IF EXIST *.rxe sh rxeflash.sh