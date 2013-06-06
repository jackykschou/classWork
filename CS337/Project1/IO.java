import java.util.*;
import java.io.*; 
public class IO{
    static class pair
	{
	    public boolean valid; 
	    public int index ; 
	    public char extension;
	    pair( int _x, char _a) 
	    {
		index = _x; 
		extension = _a; 
	    }
	}
    public static class Decompressor
    {
	public boolean finish; 
	int cursor ; 
	Vector output;  
	public FileWriter oWriter ; 
	public String lastString; 
	Decompressor(String inFile) throws Exception
	{
	    lastString = "";
	    finish = false; 
	    cursor = 0;
	    output = new Vector(); 
	    FileInputStream inStream =  new FileInputStream(inFile); 
	    FileWriter fw  = new FileWriter(inFile+".unZ" ); 
	    //oWriter = new BufferedWriter(fw); 
	    oWriter = fw; 
	    while(true) 
		{
		    int indexHigh  = (byte) inStream.read(); 		    
		    if ( indexHigh  == -1) break ;
		    int indexMiddle = (byte) inStream.read();		
		    int indexLow = (byte) inStream.read();		
		    int index = indexHigh *128*128 + indexMiddle*128 +  indexLow ;
		    char c = (char) inStream.read(); 
		    pair p = new pair(index, c) ; 
		    output.add(p); 
		}
	}
	public void done()  throws Exception 
	{	    
	    oWriter.close(); 
	}
	public pair  decode()
	{
	    pair ret ;
	    if ( cursor < output.size()) 
		{
		    ret = (pair)output.get(cursor++); 
		    ret.valid = true; 
		}
	    else 
		{
		    ret = new pair(0,'a'); 
		    ret.valid = false; 
		}
	    return ret; 
	}       
	public void append(String op) throws Exception 
	{
	    //System.out.println("Generating " + op); 
	    //oWriter.write(op,0, op.length()); 
	    oWriter.write(lastString); 
	    lastString = new String(op); 
	}
    }
    static class Compressor 
    {
	Vector output;  
	File inFile, outFile; 
	public FileOutputStream oStream ; 
	Compressor (String fileName)  throws Exception 
	{
	    output = new Vector(); 
	    String outfileName = fileName + ".myZ"; 
	    outFile = new File(outfileName);
	    inFile = new File(fileName);
	    oStream = new  FileOutputStream(outFile); 
	}
	public void encode(int x, char a)  throws Exception 
	{
	    pair p = new pair(x,a); 
	    output.add(p); 
	    int xdash = x / 128; 
	    int xLowest = x % 128; 
	    int xdashdash = xdash / 128; 
	    int xMiddle =   xdash % 128;
	    oStream.write((byte) xdashdash);
	    oStream.write((byte) xMiddle);
	    oStream.write((byte) xLowest);
	    oStream.write(a) ; 
	    // This line is to help you during debugging, please comment this out for large files
	    //System.out.println(" ( " + x + " , " + a + " )");
	} 
	public void done () throws Exception
	{
	    oStream.close(); 
	}
	public char[]  giveArray() throws Exception 
	{
	    char[] ret = new char[(int) (inFile.length()+1)];
	    int retCursor = 0; 
	    FileInputStream from = new FileInputStream(inFile); 
	    while (true) 
		{
		    byte nextByte = (byte) from.read(); 
		    if ( nextByte == -1) break ; 
		    char nextChar = (char) nextByte; 
		    ret[retCursor++] = nextChar; 
		}
	    ret[(int)(inFile.length())] = '\000';
	    return ret; 
	}
    }
}









