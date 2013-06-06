/*
 * Name: Ka Seng Chou
 * EID: kc32547
 * CS337 Project3 Finite State Machine
 * This program read in formatted files that specifies a list of finite state machine, then check if strings in a text file are accepted by those machines
 * It does not handle invalid fsm input file
 */

import java.io.*;
import java.util.*;

//class for finite state machine
class Machine
{	
	private String name; //name of the machine
	private Map<Integer, Status> statuses; //a set of statuses of the machine (key of the map is the id of the statuses)
	
	/**
	 * constructor
	 * @param name the name of the machine
	 */
	public Machine(String name)
	{
		this.name = name;
		statuses = new HashMap<Integer, Status>();
	}
	
	/**
	 * add a new status to the machine
	 * @param id the unique id of the new status
	 * @param s the new status
	 */
	public void addStatus(int id, Status s)
	{
		statuses.put(id, s);
	}
	
	/**
	 * test if a string is accepted by the machine
	 * @param s string to be tested
	 * @return boolean to indicate if the string is accepted by the machine (true accepted, false rejected)
	 */
	public boolean test(String s)
	{
		return testb(1, s);
	}
	
	//helper method of test
	private boolean testb(int id, String s)
	{
		if(id < 0) //alphabet rejected (not in the set of alphabet defined)
			return false;
		if(s.isEmpty()) //base case
			return statuses.get(id).isAcceptState();
		else //recursive case
			return testb(transit(id, s.substring(0, 1)), s.substring(1));
	}
	
	/**
	  * try to get the id of another other status from which the given status transits to given an alphabet
	  * @param id the id of the status to be transitted from
	  * @param a the alphabet to be tested
	  * @return the unique id of status that transits to, return -1 if the alphabet is rejected
	  */
	public int transit(int id, String a)
	{
		return statuses.get(id).transit(a);
	}
	
	public String toString()
	{
		return name;
	}
}

//class of status of fsm
class Status
{
	private boolean acceptState;
	private int id;
	private Map<String, Integer> transitions; //map with keys as the accept alphabets (represented by regular expressions), id of statuses to transit as the values
	
	/**
	 * constructor
	 * @param id the unique of the status
	 */
	public Status(int id)
	{
		this.id = id;
		acceptState = false;
		transitions = new HashMap<String, Integer>();
	}
	
	/**
	 * getter method of the id
	 * @return the id (integer)
	 */
	public int getId()
	{
		return id;
	}
	
	/**
	 * add an transition to the status
	 * @param s the set of accepting alphabet
	 * @param id the id of the status transition to
	 */
	public void addTransitions(String s, int id)
	{
		transitions.put(s, id);
	}
	
	/**
	 * turn the status to an acceptance status
	 */
	public void turnToAccept()
	{
		acceptState = true;
	}
	
	/**
	 * see if the status is an accept state
	 * @return true if that status is an accepting state, false otherwise
	 */
	public boolean isAcceptState()
	{
		return acceptState;
	}
	
	/**
	 * try to get the id of another other status from which the current status transits to given an alphabet
	 * @param a the alphabet to be tested
	 * @return the unique id of status that transits to, return -1 if the alphabet is rejected
	 */
	public int transit(String a)
	{
		for(String s: transitions.keySet())
		{
			if(a.matches(s))
				return transitions.get(s);
		}
		
		return -1;
	}
}
	
public class FSMachine
{
	public static void main(String args[]) throws FileNotFoundException
	{	
		//create the state machines
		Scanner machineScanner;
		List<Machine> fsms = new ArrayList<Machine>(); //machines to be stored
		int i = 1;
		String name;
		while(i < args.length)
		{	
			 name = args[i++];
			 File f = new File(name);
			 Machine m = new Machine(name);
			 machineScanner = new Scanner(f);
			 while(machineScanner.hasNextLine())
			 {
				 String line = machineScanner.nextLine();
				 if(!line.matches("^\\s*$")) //skip if it is an empty line
				 {
					 Scanner lineScanner = new Scanner(line);
					 Status s = new Status(lineScanner.nextInt());
					 String token;
					 while(lineScanner.hasNext()) //get next transition information or an accepting status indicator ('X')
					 {
						 token = lineScanner.next();
						 if(token.equals("X")) //accepting state indicator
							 s.turnToAccept();
						 else //handle a transition
						 {
							 String splitted[] = token.split(":"); //get the transitting machine id and the accepting set of alphabets
							 
							 //turn the string of accepting transition alphabet set to a regular expression
							 //replace some specific sets of alphabets defined 
							 splitted[1] = splitted[1].replaceAll("\\-", "\\\\-");
							 splitted[1] = splitted[1].replaceAll("A", "[a-z]");
							 splitted[1] = splitted[1].replaceAll("D", "[0-9]");
							 splitted[1] = splitted[1].replaceAll("N", "[123456789]");
							 splitted[1] = splitted[1].replaceAll("S", "[~!@#\\$%^&*()\\\\-+{}.,]");
							 splitted[1] = "[" + splitted[1] + "]";
							 s.addTransitions(splitted[1], Integer.parseInt(splitted[0]));
							//System.out.println(splitted[1] + "  to  " + splitted[0]);
						 }
						 m.addStatus(s.getId(), s);
					 }
				 }
			 }
			 fsms.add(m);
			 machineScanner.close();
		}
		
		//now test the strings
		
		File inputStrings = new File(args[0]); //file of Strings to be tested
		Scanner inputScanner = new Scanner(inputStrings);
		while(inputScanner.hasNext())
		{
			String s = inputScanner.next();
			for(Machine m: fsms)
			{
				if(m.test(s))
						System.out.println(m + " Accepted: " + s);
			}
		}
		inputScanner.close();
	}
}
