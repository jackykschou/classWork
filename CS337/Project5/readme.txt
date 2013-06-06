Name: Ka Seng Chou
EID: kc32547
e-mail: jacky.kschou@yahoo.com.hk
CS337 53420 Spring 2013
Project5 String Matching Algorithms
inculded files: StrMatch.java, readme.txt
*******************************************************

Compilation instructions: go to the folder where the the java file is located, then enter command javac StrMatch.java
*******************************************************

Path to binary used: /usr/bin/java
Name of machine: charity

*******************************************************

Run instructions: java StrMatch <pattern file> <text source to be searched> <file where you want to output to put in>

*******************************************************
	Report: The program runs as well as expected, producing the information about which strings were found in the source file. But it cannot handle invalid format of the pattern input file. 
And an empty string is not considered as a valid pattern. Both the algorithms have their own unique method that can eliminate some of the inefficient and unnecessary elements of the typically 
brute-force linear string search, thus making the pattern searching faster. RK algorithm does this by creating a function to express the patterns and the search strings to a simplier form (in 
this case, a hash function to produce an integer). KMP algorithm does this by shortening any possible "path" in a search by creating "cores" for each patterns(in this case, a "core table").

Theoretically, the average runtime of both of the algorithms should be linear. For the given test case and some normal (medium size) strings and source text, the runtime of both algorithm is 
similar. But for some very extreme cases, they algorithm might get to its worst case. For KMP algorithm, the time taken greatly depends on how many cores of the string (pattern) has (so the worst case is like 
a string "aaaaaaaaaaaaaaaaaaaaaaaaaa" and the text source is also almost fills with 'a's). For best case, KMP algorithm's efficiency is O(n). When gets to the worst case, the effeciency is O(n+m). But in average, 
I would consider the effeciency is O(n+m) (Through it takes O(m) additional time for constructing the "core table). Although in normal cases these algorithm takes similar time, RK algorithm can 
perform worst than KMP algorith in its worst case. The more hash value collision occurs, the more time it needs to use becuase of the extra string comparison.For example, the worst 
case would be a text sourse in which all the strings hash to the same value. For best case, RK alforithm gets O(n+m) time. When gets to the worst case, the effeciency is O(n*m). In average, I would consider
it takes O(n+m). So in conclusion, I would say the efficiency of these two algorthms are similar in most of the cases. But I would perfer KMP since its relatively consistent.

When comparing with Java's built in search routine, (I would suggest that using Scanner and Regulat expression and Pattern is the fastest way to do this) these two implementations of algorthm would be relatively 
less effecienct than that taking the full advantages of Java's built in search and matching routine. One of the reason it that the abstracted view (the objects: workStation, machinepatter, and different Machines) 
that I have created increases the complexity of the program. Another possible reason would be some possible redundant and uncessary code (due to bad code design and bad style) that causes some extra time.

To see the detail of the implementations, check out the comments of the sourse code.