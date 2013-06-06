import java.io.*;
import java.util.*;

/*
 * Name: Ka Seng Chou
 * EID: kc32547
 * CS337 Project2 RAS encryption and decryption (Cryptograph)
 * This is the decryption part
 */

public class Decrypt {
	
	public static void main(String[] args)
	{
		FileInputStream keys = null;
		FileInputStream input = null;
        FileOutputStream output = null;
        
        try
        {
	        input = new FileInputStream(args[0]); //get the input file
			keys = new FileInputStream(args[1]); //get the keys
	        output = new FileOutputStream("decrypted"); //create the output file
	        
	        Scanner keyScanner = new Scanner(keys);
	        long n = keyScanner.nextInt(); //get n
	        keyScanner.nextInt(); //pass e, the public key, we don't need that in this part
	        long d = keyScanner.nextInt(); //get d, the private key
	        long block = 0; //for holding a block of data (up to 4 bytes)
	        byte buffer[] = new byte[4]; //for holding the bytes read from the input file
	        long decryptedBlock; //for holding the decrypted blocks
	        
	        Arrays.fill(buffer, (byte)0); //initialize the bytes to 0
	        while(input.read(buffer) != -1) //if there is still 4 bytes to read
	        {
	        	//"appends" the bytes to the block to perform encryption
	        	block = (((int)buffer[0] & 0xFF) << 24) | (((int)buffer[1] & 0xFF) << 16 ) | 
	        			(((int)buffer[2] & 0xFF) << 8 ) | ((int)buffer[3] & 0xFF);
	        	//for debug
	        	/*
	        	for(byte b: buffer)
	        	{
	        		System.out.println("byte: " + Integer.toBinaryString((int)b));
	        	}
	        	
	        	System.out.println("block: " + Long.toBinaryString(block));
	        	*/
	        	
	        	//perform the decryption
	        	decryptedBlock = 1;
	        	for(int i = (Long.toBinaryString(d)).length(); i > 0; i--)
	        	{
	        		if((d >>> (i - 1) & 1) == 0)
	        			decryptedBlock = (decryptedBlock * decryptedBlock) % n;
	        		else
	        			decryptedBlock = (((decryptedBlock * decryptedBlock) % n) * block) % n;
	        	}
	        	//write bytes to output file
	        	for(int i = 2; i >= 0; i--)
	        		output.write(((byte)(decryptedBlock >>> (i * 8)) & 0xFF));
	        	
	        	Arrays.fill(buffer, (byte)0); //"renew" the bytes to 0
	        }
	        
	        
	        
	        //trim the null bytes at the end of the decrypted file
	        File decrypted = new File("decrypted");
	        RandomAccessFile file = new RandomAccessFile(decrypted,"rwd");
	        file.seek(decrypted.length() - 1); //get to the end of the file
	        byte c;
	        while((c = file.readByte())== 0x00) //if the last byte is a null byte, trim the byte
	        {
	             file.setLength(decrypted.length()-1);
	             file.seek(decrypted.length() - 1); //get to the end of the file
	        }
	        
	        file.close();
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
