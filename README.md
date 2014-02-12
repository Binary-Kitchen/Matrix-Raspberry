Matrix-Raspberry
================

Driver Software for Binary's Kitchen LED Matrix

Compilation
-------------

* Install wiringPi
* mkdir build
* cd build
* cmake ../Matrix-Raspberry/
* make

Usage
-------------

Binary reads from stdin.

* while true; do ;nc -p 1337 -l | ./Matrix-Raspberry; done 

