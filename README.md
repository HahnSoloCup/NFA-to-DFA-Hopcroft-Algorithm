# NFA-to-DFA-Hopcroft-Algorithm
Theoretical computational model conversion and state minimization algorithm

This algorithm was designed to take data regarding nondeterministic finite automata and transform them into deterministic finite automata. 
These automata can also be optimized with a hopcroft state minimization implementation
Most of the code explaination and demonstration is included in the documentation.

The Main - MinGW 
	-Is runnable in the MinGW c++ compiler

The Main - Visual c++
	-This version includes a few minor edits like different header files to be runnable in visual studio
	-To test this version, put all the .cpp files (along with the data folder) in the same directory
	-To add and test graphs, place said graph .txts in the data folder and hardcode said file name into line 334 of Main.cpp
