Name: Ka Seng Chou
EID: kc32547
e-mail: jacky.kschou@yahoo.com.hk
CS337 53420 Spring 2013
Project 1 Experiments in Compression
inculded files: Huff.java, Comp.java, readme.txt, smaller1.txt, smaller2.txt, larger.txt, IO.java
*******************************************************

Compilation instructions:
Compile Huff.java by entering the command: javac Huff.java IO.java
COmpile Comp.java by entering the command: javac Comp.java IO.java

*******************************************************

Part 1:

report:
This program makes use of several data structure to fullfil its goal. The IO class provided is used to 
get characters (array) of the input file. It calculates the probability (or frequency) of each characters in the text file by 
first construting a map (key is different unique symbol, value is the number of occurrence), then
constuct a Tree node (there is a tree node class made) of each symbol and put them into a 
priority queue (the bag). After that, a best Huffman tree is constructed by popping out the node from
the bag (the property of priority queue with the help of a custom made Comparator provides a easy implementation
of the Huffman Algorithm). The bag is also used to calculate the entropy. The actual length is then calculated
by "travelling" through the tree and counting the number of bits.

functionality:
This program works fine, it also handles the "special case" in which there is only 
one symbol in the input file (the actual length will be 1)

*******************************************************

Part2:

Compression:

report:
After getting characters (array) of the input file by using the IO class provided, the programs starts
construcing a tree (there is a tree node class made) with node representing each "pair" that will be added to
the dictionary. In order to add the "pair" to the dictionary in ascending order of the index, a priority queue is
used. All the node in the tree except the root first added to the queue, the encoding occurs by popping each "pair"
one by one from the queue.

functionality:
The compression works as good as expected. Although if the input file is small enough, the compressed file
can result in a larger file size.

******************************************************
Decompression:

report:
The decompression process is fairly simply. It also uses the IO class provided.
It makes use of a map by putting each deocded "pair" (in order) as a value of the map and
the index (the order they are encoded) as the key. By doing this, the string represented by each pair
can be easily determined (with a method) and then added to the output file by using the method in IO class.

functionality:
The decompression works as fine as the compression.

********************Specified question to answer********************
1.Why some compressed file is larger than the input file:
	When there are many unique symbol (or unique combination of symbols) in the input file, each "pairs" of the dictory
are much less "dependent" on other "pairs" of the dictionary, thus making a larger size of the dictionary. In other words, the more
dependent of the "pairs" on the other "pairs", the better compression can be done. For example, in mbone of the input file (smaller1.txt),
there are a huge number of the same symbol (the file has only one unique symbol), so the "pairs" can be very dependent on the other
"pairs", thus appcomplishing a great compression (the compressed file size is much smaller).

To see the detail of the implementations, check out the comments of the sourse code.