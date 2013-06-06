import java.io.*;
import java.util.*;

/*
 * Name: Ka Seng Chou
 * EID: kc32547
 * CS337 Project2 RAS encryption and decryption (Cryptograph)
 * This is the encryption part
 */

public class Encrypt {
	
	public static void main(String[] args)
	{
		FileInputStream keys = null;
		FileInputStream input = null;
        FileOutputStream output = null;
        
        try
        {
	        input = new FileInputStream(args[0]); //get the input file
			keys = new FileInputStream(args[1]); //get the keys
	        output = new FileOutputStream("encrypted"); //create the output file
	        
	        Scanner keyScanner = new Scanner(keys);
	        long n = keyScanner.nextInt(); //get n
	        long e = keyScanner.nextInt(); //get e, the public key
	        long block = 0; //for holding a block of data (up to 3 bytes)
	        byte buffer[] = new byte[3]; //for holding the bytes read from the input file
	        long encryptedBlock; //for holding the encrypted blocks
	        
	        
	        Arrays.fill(buffer, (byte)0); //initialize the bytes to 0
	        while(input.read(buffer) != -1) //if there is still data to read
	        {
	        	//"appends" the bytes to the block to perform encryption
	        	block = (((int)buffer[0] & 0xFF) << 16) | (((int)buffer[1] & 0xFF) << 8) | 
	        			((int)buffer[2] & 0xFF);
	        	//for debug
	        	/*
	        	for(byte b: buffer)
	        	{
	        		System.out.println("byte: " + Integer.toBinaryString((int)b));
	        	}
	        	
	        	System.out.println("block: " + Long.toBinaryString(block));
	        	*/
	        	
	        	//perform the encryption
	        	encryptedBlock = 1;
	        	for(int i = (Long.toBinaryString(e)).length(); i > 0; i--)
	        	{
	        		if((e >>> (i - 1) & 1) == 0)
	        			encryptedBlock = (encryptedBlock * encryptedBlock) % n;
	        		else
	        			encryptedBlock = (((encryptedBlock * encryptedBlock) % n) * block) % n;
	        	}
	        	
	        	//write bytes to output file
	        	for(int i = 3; i >= 0; i--)
	        		output.write(((byte)(encryptedBlock >>> (i * 8)) & 0xFF));
	        	Arrays.fill(buffer, (byte)0); //"renew" the bytes to 0
	        }
	        
	        keyScanner.close();
		} catch (Exception e)
		{
			// TODO Auto-generated catch block
			e.printStackTrace(); //get the keys
		}
        
        //performs proper closing of files if possible
		finally
		{
         if (input != null) {
             try {
				input.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
         }
         if (keys != null) {
             try {
				keys.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
         }
         if (output != null) {
             try {
				output.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
         }
     }
        
	}
	
}
