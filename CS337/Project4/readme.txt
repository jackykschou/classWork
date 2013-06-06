Name: Ka Seng Chou
EID: kc32547
e-mail: jacky.kschou@yahoo.com.hk
CS337 53420 Spring 2013
Project 4 Database Querying in Haskell
inculded files: project4.hs readme.txt
*******************************************************

Compilation instructions: go to the folder where the hs file is located, then enter ghci. After that, load the file by typing :l project4.hs

*******************************************************

Path to binary used: /usr/bin/ghci /usr/share/man/man1/ghci.1.gz
Name of machine: charity

*******************************************************

Run instructions: After loading the program (see compilation instructions), just enter the function name you want to test with its coresponding argument.
		  For example: overpaid database0


*******************************************************
	Report: Every functions required work as well as expected, each of them is functioning very well.
This program make a good use of list comphresion, which is a powerful tool to generate a set (a list) with specifed elements.
Als this program use a lot of helper functions as well as recursion. Tail recursive is also used as well.

To see the detail of the implementations, check out the comments of the sourse code.

********************Specified question to answer********************

output:

*Main> overpaid database0
["Virginia Dare","Buster Keaton","Vanessa Redgrave"]
*Main> overpaid database1
[]
*Main> overpaid database2
["Carol","Eric"]
*Main> overpaid database3
["Carol","Fran","Dan","Eric"]
*Main> grossly_overpaid database0
["Vanessa Redgrave"]
*Main> grossly_overpaid database1
[]
*Main> grossly_overpaid database2
["Carol","Eric"]
*Main> grossly_overpaid database3
["Carol","Fran","Dan","Eric"]
*Main> spouse_manager database0
["Vanessa Redgrave"]
*Main> spouse_manager database1
[]
*Main> spouse_manager database2
["Alex"]
*Main> spouse_manager database3
["Alex"]
*Main> spouse_manager_super database0
["Edna Millay","Vanessa Redgrave"]
*Main> spouse_manager_super database1
[]
*Main> spouse_manager_super database2
["Dan","Alex","Carol"]
*Main> spouse_manager_super database3
["Alex"]
*Main> super_manager database0
["Virginia Dare","James Joyce","Edna Millay"]
*Main> super_manager database1
[]
*Main> super_manager database2
["Carol","Bob","Alex"]
*Main> super_manager database3
[]
*Main> nepotism database0
[("Laurence Sterne","Ingrid Joyce"),("Ingrid Joyce","Laurence Sterne")]
*Main> nepotism database1
[]
*Main> nepotism database2
[]
*Main> nepotism database3
[]
*Main> rich database0
[("Buster Keaton","Lana Turner"),("Laurence Sterne","Virginia Dare")]
*Main> rich database1
[]
*Main> rich database2
[("Eric","Carol")]
*Main> rich database3
[("Eric","Carol")]
*Main> sorted_salaries database0
["Vanessa Redgrave","Virginia Dare","Edna Millay","Ingrid Joyce","James Joyce"]
*Main> sorted_salaries database1
[]
*Main> sorted_salaries database2
["Eric","Carol","Dan","Bob","Alex"]
*Main> sorted_salaries database3
["Alex"]
*Main> sorted_rank database0
["Virginia Dare","James Joyce","Edna Millay","Ingrid Joyce","Vanessa Redgrave"]
*Main> sorted_rank database1
[]
*Main> sorted_rank database2
["Bob","Eric","Alex","Carol","Dan"]
*Main> sorted_rank database3
["Alex"]
*Main> sorted_worth database0
["Vanessa Redgrave","Edna Millay","Ingrid Joyce","Virginia Dare","James Joyce"]
*Main> sorted_worth database1
[]
*Main> sorted_worth database2
["Eric","Carol","Dan","Bob","Alex"]
*Main> sorted_worth database3
["Alex"]
*Main> normalize database0
[("James Joyce","Ingrid Joyce",60000,"Root"),("Vanessa Redgrave","Michael Redgrave",110000,"James Joyce"),("Michael Redgrave","Vanessa Redgrave",40000,"Vanessa Redgrave"),("Edna Millay","Ted Hughes",70000,"Root"),("Laurence Sterne","Virginia Dare",60000,"James Joyce"),("Virginia Dare","Laurence Sterne",100000,"Edna Millay"),("Ingrid Joyce","James Joyce",60000,"Virginia Dare"),("Lana Turner","Buster Keaton",80000,"Virginia Dare"),("Ted Hughes","Edna Millay",70000,"Virginia Dare"),("Buster Keaton","Lana Turner",80000,"Ingrid Joyce")]
*Main> normalize database1
[]
*Main> normalize database2
[("Alex","Bob",100000,"Root"),("Bob","Alex",100000,"Alex"),("Carol","Eric",200000,"Bob"),("Dan","Fran",150000,"Carol"),("Eric","Carol",300000,"Dan"),("Fran","Dan",200000,"Eric")]
*Main> normalize database3
[("Carol","Eric",200000,"Root"),("Fran","Dan",200000,"Root"),("Dan","Fran",150000,"Root"),("Alex","Bob",100000,"Root"),("Eric","Carol",300000,"Root"),("Bob","Alex",100000,"Alex")]