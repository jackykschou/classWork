/*
 * Name: Ka Seng Chou
 * EID: kc32547
 * CS337 Project5 String Matching Algorithms
 * This program read in formatted files that specifies a list of machinePatterns and a text file to see which machinePatterns exist in the text file using two string matching algorithm
 * It does not handle invalid input files
 */

import java.io.*;
import java.util.*;
import java.util.regex.*;

//class for a machinePattern
	class machinePattern
	{
		String string; //the actual data of a machinePattern
		Object machinePatternInfo; //the abstraction that represents a machinePattern in a SMM
		
		//constructors
		machinePattern(String s, Object info)
		{
			string = s;
			machinePatternInfo = info;
		}
		
		machinePattern(String s)
		{
			string = s;
		}
		
		//getter method for string
		String getString()
		{
			return string;
		}
		
		//getter method for machinePatternInfo
		Object getmachinePatternInfo()
		{
			return machinePatternInfo;
		}
		
	}
	
	//class for string matching machine
	abstract class SMMachine
	{
		LinkedList<machinePattern> machinePatterns = new LinkedList<machinePattern>(); //this is implemented as a queue of machinePatterns
		StringBuilder text; //the text for searching the machinePattern
		//indicate what kind of newline character is used in the machinePatterns
		char newline = 'w'; //default is windows newline ('l' stands for "\n" as newline, 'w' stands for "\r\n" as newline
		
		//these variables are used to compute the hash value of machinePatterns
		static final int charRadix = 256;
		static final int hashConstant = 47;
		
		//constructor
		SMMachine(StringBuilder ps, StringBuilder text)
		{
			this.text = text;
			getmachinePatterns(ps);
		}
		 
		public void getmachinePatterns(StringBuilder sb) //initialize the machinePatterns queue
		{
			//use machinePattern to get the machinePattern from the input
			Matcher m = Pattern.compile("&([^&]*)&").matcher(sb.toString());
			
			//if the newline character in the text is for Linux, change the type
			if(text.indexOf("\r\n") != -1)
			{
				newline = 'w';
			}
			
			String next;
			while(m.find())
			{
				next = m.group(1);
				//possibly change the newline characters in the machinePatterns to the same type of newline character
				if(newline == 'l')
				{	
					next = next.replaceAll("\r\n", "\n");
				}
				machinePatterns.add(new machinePattern(next, getmachinePatternInfo(next)));
			}
		 }
		 public abstract Object getmachinePatternInfo(String s); //get the representation of a given machinePattern
		 public abstract String nextmachinePatternInText(); //return message of whether the next machinePattern in the machinePattern queue was found in the text
		 
	}

	//SSMachine using Rabin-Karp Algorithm
	class RKMachine extends SMMachine
	{
		//constructor
		RKMachine(StringBuilder ps, StringBuilder text)
		{
			super(ps, text);
		}
		
		
		public Object getmachinePatternInfo(String s)
		{
			long hash = 0; //the hash value
			for(char c: s.toCharArray())
			{
				hash = (hash * charRadix + c) % hashConstant;
			}
			return hash;
		}
		
		public String nextmachinePatternInText()
		{		
			machinePattern machinePattern = machinePatterns.poll();
			
			//special case: no more machinePattern to process
			if(machinePattern == null)
				return null;

			if(text.length() < machinePattern.getString().length()) //special case, text length is less than the machinePattern length
				return "RK  FAILED: " + machinePattern.getString() + "\n";
			
			long hashValue = (Long)machinePattern.getmachinePatternInfo(); //the hash value of the machinePattern
			long checkValue = (Long)getmachinePatternInfo(text.substring(0, machinePattern.getString().length())); //first hash value of the text
			
			//special case for the first match
			if((checkValue == hashValue) && (machinePattern.getString()).equals(text.substring(0, machinePattern.getString().length())))
				return "RK  MATCHED: " + machinePattern.getString() + "\n";
			
			int patternLen = machinePattern.getString().length();
			long f = 1;
			for(int i = 1; i <= patternLen - 1; i++)
			{
				f = (charRadix * f) % hashConstant;
			}
			for(int i = patternLen; i < text.length(); i++)
			{
				//remove first character
				checkValue = (checkValue + hashConstant - f * text.charAt(i - patternLen) % hashConstant) % hashConstant;
				//add the next character
				checkValue = (checkValue * charRadix + text.charAt(i)) % hashConstant;
				//machinePattern found
				if((checkValue == hashValue) && (machinePattern.getString()).equals(text.substring(i - patternLen + 1, i + 1)))
					return "RK  MATCHED: " + machinePattern.getString() + "\n";
			}
			//machinePattern not found
			return "RK  FAILED: " + machinePattern.getString() + "\n";
		}
	}
	
	//SSMachine using Knuth-Morris-Pratt Algorithm
	class KMRMachine extends SMMachine
	{
		//constructor
		KMRMachine(StringBuilder ps, StringBuilder text)
		{
			super(ps, text);
		}
		
		public Object getmachinePatternInfo(String s)
		{
			//the table for indicating the position where the next search should begin on if a mismatch happens
			int table[] = new int[s.length()];
			//special cases
			table[0] = -1;
			if(table.length > 1)
			{
				table[1] = 0;
				if(table.length > 2)
				{
					int current = 2;
					int compareIndex = 0;
					
					while(current < s.length())
					{
						//the character is part of the current substring which is both a prefix and suffix of the machinePattern 
						if(s.charAt(current - 1) == s.charAt(compareIndex))
						{
							table[current++] = compareIndex++;
						}
						//the character is not part of the current substring, but we can see if it is part of a possible substring
						else if(compareIndex > 0)
						{
							compareIndex = table[compareIndex];
						}
						//the character is not part of any substring, so the next search must start from the beginning
						else
						{
							table[current++] = 0;
						}
					}
				}
			}
				return table;
		}
		
		public String nextmachinePatternInText()
		{
			machinePattern machinePattern = machinePatterns.poll();
			
			//special case: no more machinePattern to process
			if(machinePattern == null)
				return null;
			
			int indexInText = 0;
			int indexInmachinePattern = 0;
			int textLen = text.length();
			String s = machinePattern.getString(); //string of the machinePattern
			int machinePatternLen = machinePattern.getString().length(); //length of the string of the machinePattern
			int table[] = (int[])machinePattern.getmachinePatternInfo(); //the table of the machinePattern
			
			while((indexInText + indexInmachinePattern) < textLen)
			{
				if(text.charAt(indexInText + indexInmachinePattern) == s.charAt(indexInmachinePattern)) //the current character matches
				{
					if(indexInmachinePattern == (machinePatternLen - 1)) //the machinePattern was found in the text
						return "KMR MATCHED: " + s + "\n";
					else
						indexInmachinePattern++; //go to the next character in the machinePattern for comparison
				}
				else //the current character does not match 
				{
					indexInText += (indexInmachinePattern - table[indexInmachinePattern]); //update the index
					if(table[indexInmachinePattern] != -1) //there is a possible path for simplification that skips comparing part of the machinePattern for the next comparison because the suffix of the machinePattern is its prefix
					{
						indexInmachinePattern = table[indexInmachinePattern];
					}
					else //start from the first character of the machinePattern again
						indexInmachinePattern = 0;
				}
			}
			return "KMR FAILED: " + machinePattern.getString() + "\n"; //machinePattern not in text
		}
		
	}
	
	//SMMachine "work station" to produce the required output
	class SMMachineStation
	{
		KMRMachine kmr;
		RKMachine rk;
		
		//Constructor
		SMMachineStation(File machinePattern, File text) throws IOException
		{
			addMachine(machinePattern, text);
		}
		
		//method that add a KMRMachine and a RKMachine to the station given a machinePattern file and a text file
		public void addMachine(File machinePatterns, File text) throws IOException
		{
			StringBuilder sb1 = new StringBuilder(); //string builder for getting the text file
			StringBuilder sb2 = new StringBuilder(); //string builder for getting the machinePatterns file
			
			FileInputStream fis1 = new FileInputStream(machinePatterns);
			BufferedInputStream bis1 = new BufferedInputStream(fis1);
			FileInputStream fis2 = new FileInputStream(text);
			BufferedInputStream bis2 = new BufferedInputStream(fis2);
			
			byte[] bytes = new byte[1024];
            int bytesRead=0; 
            //get the text to the StringBuilder
            while( (bytesRead = bis1.read(bytes)) != -1)
            { 
            	sb1.append(new String(bytes, 0, bytesRead));               
            }
            
         	//get the machinePatterns to the StringBuilder
            while( (bytesRead = bis2.read(bytes)) != -1)
            { 
            	sb2.append(new String(bytes, 0, bytesRead));               
            }
            
			kmr = new KMRMachine(sb1, sb2);
			rk = new RKMachine(sb1, sb2);
			
			//close the streams
			bis1.close();
			bis2.close();
		}
		
		//method that produces the output text file that indicates which machinePatterns are found in the text and which are not
		public void produceMatchResults(File output) throws IOException
		{
			StringBuilder sb = new StringBuilder(); //string builder for getting the results before putting to the new file
			
			String s; //a temporally variable to hold the output string for testing for the end of output
			while((s = rk.nextmachinePatternInText()) != null)
			{
				sb.append(s);
				sb.append(kmr.nextmachinePatternInText());
			}
			
			FileWriter fw = new FileWriter(output);
			fw.write(sb.toString());
			
			fw.close(); //close the stream
		}
	}

public class StrMatch {
	
	public static void main(String[] args) throws IOException
	{
		File machinePatterns = new File(args[0]);
		File text = new File(args[1]);
		File output = new File(args[2]);
		
		SMMachineStation station = new SMMachineStation(machinePatterns, text);
		station.produceMatchResults(output);
	}
}


