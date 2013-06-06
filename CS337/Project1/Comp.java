
/*
 * Name: Ka Seng Chou
 * EID: kc32547
 * CS337 Project1 LZ Compression (data compression)
 */

import java.util.*;

public class Comp {

	//the tree node class for constructing the sibling list tree for LZ Compression
	public static class LZTreeNode
	{
		int parentIndex; //the index of the node's parent
		int index;
		Character character;
		LZTreeNode child;
		LZTreeNode next;

		//constructor for the root
		LZTreeNode(int index)
		{
			this.index = index;
			character = null;
			child = null;
			next = null;
		}

		//constructor for other nodes other than the root node
		LZTreeNode(int index, char c, int parentIndex)
		{
			this.index = index;
			this.parentIndex = parentIndex;
			character = c;
			child = null;
			next = null;
		}
	}

	//the Comparator class for encoding the tree in ascending order of the index
	public static class LZNodeComparator implements Comparator<LZTreeNode>
	{
		public int compare(LZTreeNode node1, LZTreeNode node2) {
		       return ((Integer)(node1.index)).compareTo((Integer)(node2.index));
		    }
	}

	public static void main(String[] args)
	{
		if(args[0].equals("c")) //compression option
		{
			try {
				IO.Compressor comp = new IO.Compressor(args[1]); //get the input file
				char[] words = comp.giveArray(); //get the words from the input file as an array

				LZTreeNode root = new LZTreeNode(0); //make the root of the tree (the 0 index of the dictionary)
				int index = 0; //keep tract of the index
				//write all the possible combination to the tree (construct the tree)
				for(int i = 0; i < words.length - 1; i++)
				{
					String word = ((Character)words[i]).toString();
					//if the combination already exits in the tree, add the next character of the input file to form a new combination
					while(StringInTree(word, root.child))
					{
							word += words[++i];
					}
					addNewNode(word, root, ++index); //append new node
				}

				//make a priority queue for encoding the elements of dictionary in ascending order of the index
				Queue<LZTreeNode> queue = new PriorityQueue<LZTreeNode>(index, new LZNodeComparator());

				//add nodes to the queue
				encodeTree(root, comp, queue);

				//encode each combination of the tree (form and dictionary and make the compressed file)
				while(!queue.isEmpty())
				{
					LZTreeNode temp = queue.poll();
					comp.encode(temp.parentIndex, temp.character);
				}

				comp.done();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		else if(args[0].equals("d")) //decompression option
		{
			try {
				IO.Decompressor decomp = new IO.Decompressor(args[1]); //get the input compressed file

				IO.pair p; //for holding pair returned by decode()
				Map<Integer, IO.pair> map = new HashMap<Integer, IO.pair>(); //map for holding the pairs
				int index = 1; //keep track of the index

                //decoding process
				do
				{
					p = decomp.decode(); //get a pair
					map.put(index++, p); //put the new pair into the map
					decomp.append(decodePair(p, map)); //add the new String represented by the new pair
				}while(p.valid == true);

				decomp.done();
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

		}
		else //invalid command option
			System.out.println("Invalid Command!");
	}

	//method that tests if a given string exists in a given sibling tree
	static boolean StringInTree(String s, LZTreeNode node)
	{
		//base case: String exists in the tree
		if(s.isEmpty())
		{
			return true;
		}
		LZTreeNode current = node;
		while(current != null)
		{
			if(current.character.equals((Character)s.charAt(0)))
			{
				//recursive case: the character in the node match the string, travel down the child
				return StringInTree(s.substring(1), current.child);
			}
			current = current.next; //get the next sibling
		}
		return false; //String not found
	}

	//method that add a new node (add a new pair to the dictionary) to a given tree
	static void addNewNode(String s, LZTreeNode currentNode, int index)
	{
		//base case: append to node (currentNode.child == null is for the base case in the root)
		if(s.length() == 1 || currentNode.child == null)
		{
			addChild(currentNode, new LZTreeNode(index, s.charAt(0) , currentNode.index)); //append new node
			return;
		}
		else
		{
			LZTreeNode current = currentNode.child;
			while(current != null)
			{
				if(current.character.equals((Character)s.charAt(0)))
				{
					//recursive case: travel down the child
					addNewNode(s.substring(1), current, index);
					return;
				}
				current = current.next;
			}
		}
	}

	//method that add a new child node to a given node
	static void addChild(LZTreeNode oldNode, LZTreeNode newNode)
	{
		//special case: the node has no child yet
		if(oldNode.child == null)
			oldNode.child = newNode;
		//normal case: the node has a child, append to the end of the "child list"
		else
			addChildb(oldNode.child, newNode);

	}

	//helper method for addChild(LZTreeNode oldNode, LZTreeNode newNode)
	private static void addChildb(LZTreeNode oldNode, LZTreeNode newNode)
	{
		//base case: add the new node
		if(oldNode.next == null)
			oldNode.next = newNode;
		//recursive case: travel to the next node
		else
			addChildb(oldNode.next, newNode);
	}

	//method that adds all combination into the dictionary
	static void encodeTree(LZTreeNode node, IO.Compressor comp, Queue<LZTreeNode> queue)
	{
		LZTreeNode current = node.child;
		while(current != null)
		{
			try {
				queue.add(current); //add the node to the priority for encoding
				//recursively travel through the tree
				encodeTree(current, comp, queue);
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			current = current.next;
		}

	}

	//method that return a String that is represented by a pair
	static String decodePair(IO.pair p, Map<Integer, IO.pair> map)
	{
		//base case: index reach 0
		if(p.index == 0)
			return ((Character)p.extension).toString();
		//recursive case: go to another pair from which the index linked to
		else
			return decodePair(map.get(p.index), map) + ((Character)p.extension).toString();
	}

}


