import java.util.*;
import java.math.*;

/*
 * Name: Ka Seng Chou
 * EID: kc32547
 * CS337 Project1 Huffman coding (data compression)
 */
class Huff {
	
	//a class of tree node for constructing the Huffman tree, it is also involved in the calculation of the frequency
	public static class HuffTreeNode
	{
		HuffTreeNode leftChild;
		HuffTreeNode rightChild;
		char bit;
		double freq; //probability of a certain node
		char character; //hold the character if it is a leaf
		boolean isLeaf; //boolean to indicate if the node is a leaf
		
		//constructor1
		HuffTreeNode(double freq, char c, boolean isLeaf)
		{
			this.freq = freq;
			character = c;
			this.isLeaf = isLeaf;
		}
		
		//constructor2
		HuffTreeNode(double freq, HuffTreeNode leftChild, HuffTreeNode rightChild, boolean isLeaf)
		{
			this.freq = freq;
			this.leftChild = leftChild;
			this.rightChild = rightChild;
			this.isLeaf = isLeaf;
			this.bit = bit;
		}
		
	}
	
	//the Comparator class for sorting (compare the probability of 2 characters)
	public static class NodeComparator implements Comparator<HuffTreeNode> 
	{
		public int compare(HuffTreeNode node1, HuffTreeNode node2) {
		       return ((Double)(node1.freq)).compareTo((Double)(node2.freq));
		    }
	}
	
	public static void main(String[] args)
	{
		try {
			IO.Compressor comp = new IO.Compressor(args[0]); //get the input file
			char fileWords[] = comp.giveArray(); //array hold the characters of the input file
			
			//make a map for holding different characters in the file and their corresponding occurrence
			Map<Character, Integer> words = new TreeMap<Character, Integer>();
			int totalCount = 0; //count the total number of different characters of the input file
			
			//for loop to put the character in the input file to the map
			for(int i = 0; i < (fileWords.length - 1); i++)
			{
				Character c = fileWords[i];
				if(!words.containsKey(c))
				{
					//null character or a new character occur
					totalCount++; //add total count
					words.put(c, 1);
				}
				else
				{
					//increment the number of occurrence
					words.put(c, 1+words.get(c));
					totalCount++; //add total count
				}
				
				
			}
			
			/*calculate the corresponding probability of difference characters and set up node for them, put all the nodes 
			into a queue (the bag) */ 
			 Set<Map.Entry<Character, Integer>> mapEntry = words.entrySet(); //get the entry set of the map
			 PriorityQueue<HuffTreeNode> bag = new PriorityQueue<HuffTreeNode>(fileWords.length, new NodeComparator()); //the "bag"
			 for(Map.Entry<Character, Integer> e: mapEntry)
			 {
				 bag.add(new HuffTreeNode(e.getValue()/(double)totalCount, e.getKey(), true));
			 }
			 
			 //calculate the entropy
			 double entropy = calculateEntropy(bag);
			 
			 //construct the best tree
			 HuffTreeNode root = makeBestTree(bag);
			 
			 //get the actual length of the file (in bits)
			 int actualLen = 0;
			 /*a for loop to iterate through each character that occurs in the input file (each map key)
			    and add the number of bits of each character (times their number of occurrence) together */
			 for(Map.Entry<Character, Integer> e: mapEntry)
			 {
				 actualLen += findNoBits(root, e.getKey(), 0) * e.getValue();
			 }
			 
			 //print the required results
			 System.out.println("Actual length of the file by Huffman coding is");
			 System.out.println(actualLen);
			 System.out.println("and minimum achievable is");
			 System.out.println(String.format("%.2f", Math.round((entropy * totalCount) * 100) / 100.0));
			
			 //close the file
			 comp.done();
		} catch (Exception e) { //catch any possible exception thrown
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	//a method that given a bag, it construct the bes Huffman tree and return the root of the tree
	static HuffTreeNode makeBestTree(PriorityQueue<HuffTreeNode> bag)
	{
		//special case: input file is empty, return null
		if(bag.isEmpty())
			return null;
		//normal case: more than one unique character in the input file
		else
			return makeBestTreeb(bag);
	}
	
	//helper method of makeBestTree(PriorityQueue<HuffTreeNode> bag)
	private static HuffTreeNode makeBestTreeb(PriorityQueue<HuffTreeNode> bag)
	{
		//base case: only one node left
		if(bag.size() == 1)
			return bag.poll();
		HuffTreeNode node1;
		HuffTreeNode node2;
		//recursive case: there are still 2 elements in the bag (get the 2 smallest ones)
		{
			node1 = bag.poll();
			node2 = bag.poll();
			//set the "bit path"
			node1.bit = '0';
			node2.bit = '1';
			//make a new node from the 2 smallest node and add it to the bag
			bag.add(new HuffTreeNode(node1.freq + node2.freq, node1, node2, false));
			return makeBestTree(bag);
		}
			
	}
	
	//a method that returns the number of bits that represents a given character in a given Huffman
	static int findNoBits(HuffTreeNode node, char c, int bitCount)
	{
		if(node.isLeaf)
			return 1;
		else
			return findNoBitsb(node, c, bitCount);
	}
	
	//helper method for findNoBits(HuffTreeNode node, char c, int bitCount)
	private static int findNoBitsb(HuffTreeNode node, char c, int bitCount)
	{
		//base case: a leaf is reached and compare to the target character
		if(node.isLeaf && node.character == c)
			return bitCount;
		//special case, target character not in tree
		else if(node.isLeaf && node.character != c)
		{
			return -1;
		}
		//recursive case: search down the tree
		else
		{
			int temp;
			bitCount++; //one more bit
			if((temp = findNoBitsb(node.rightChild, c, bitCount)) != -1) 
				return temp;
			else
				return findNoBitsb(node.leftChild, c, bitCount);
		}
	}
	
	//method that calculates the Entropy of the input file
	static double calculateEntropy(Collection<HuffTreeNode> bag)
	{
		double entropy = 0;
		for(HuffTreeNode node: bag) //iterate through each symbol for calculating the entropy 
			entropy += -(node.freq) * (Math.log(node.freq)/Math.log(2)); //the equation
		return entropy;
	}

}
