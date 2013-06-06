Name: Ka Seng Chou
EID: kc32547
e-mail: jacky.kschou@yahoo.com.hk
CS337 53420 Spring 2013
Project 3 Finite State Machine
inculded files: FSMachine.java machine0.fsm machine1.fsm machine2.fsm machine3.fsm machine4.fsm machine5.fsm machine6.fsm strings.txt readme.txt
*******************************************************

Compilation instructions: javac FSMachine.java

*******************************************************

Path to binary used: /usr/bin/java
Name of machine: charity

*******************************************************

Run instructions: java FSMachine <name of text file of strings to be tested> [a list of fsm files] 


*******************************************************
Part1: finite state diagram drawing
	This part is just drawing state machine diagram on paper to prepare for creating the files in part 2.
*******************************************************
Part2: creating fsm files
	This part is about to create the 7 fsm files as requested, which are used as test files in part 3.
******************************************************
Part3: 

Report:
	The program loads a text file which containes a set of string (each line one string), and also loads a list
of fsm files from the command line argument. It then test and show whether the fsm(s) accept each of the strings in the file.
This is accomplished by making a class that represents state machine and a class that represents the states. The transitions of the states
and the set of states that a machine contains are represented as a map. This program also makes good use of regular expressions in java to 
"simplify" the searching and matching processes in the transitions of states.

Functionality:
	This programs run as well as expected. But notice that this program does not handle invalid fsm or test string file except it can trim
the spaces at the end of line.



********************Specified question to answer********************
none

To see the detail of the implementations, check out the comments of the sourse code.