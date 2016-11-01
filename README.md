# ar_sccalc
Arduino scientific calculator with expression parser (not fully working yet)




Hey,
I started working on scientific calculator based on Arduino Mega 2560. It isn't working yet, but every day I add new functionality and fix bugs I know. I hope this code my help someone building expression parser, because this is the only fully working thing in code right now.

Some functions of calculator:
- mathematical operations including  "*10^i"
- 0x0F and 0b1111 - recogintion of biniary and hexadecimal represented numbers to help me in programming 
- bitwise operations (<<, >>, |, &, etc...)


Things I'd like to change:
- memory allocation is static;
  Every step of parsing expression uses 2 predefined in array float variables, it is not efficient when I try to store biniary   and hexadecimal numbers. I'd like to create my own system to maximise possibilites of Arduino, but right now it allows me to 
  get around 24 steps and that isn't worst result.
- use matrix keyboard instead of serial input
- use graphical LCD instead of alphanumerical 16x2
