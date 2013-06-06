Name: Ka Seng Chou
EID: kc32547
e-mail: jacky.kschou@yahoo.com.hk
CS337 53420 Spring 2013
Project 2 RSA Encryption and decryption (Cryptography)
inculded files: Encrypt.java, Decrypt.java key.txt readme.txt
*******************************************************

Compilation instructions:
Compile Encrypt.java by entering the command: javac Encrypt.java
COmpile Decrypt.java by entering the command: javac Decrypt.java

*******************************************************

Run instructions:
Run Encrypt.java by entering java Encrypt <name of the file to be encrypted> <file name of the key>
Run Decrypt.java by entering java Decrypt <name of the file to be decrypted> <file name of the key>

*******************************************************
Part1: key generation

For this part we just generate they keys and put into the text file with the required format,
here is the detailed information of calculating the keys:
p = 6007, q = 4001, n = 24034007, Phi(n) = 24024000, e = 17, d = 2826353

*******************************************************
Part2: encryption

report:
This program takes two input files (keys and the input data to be encrypted) as FileInputStream and produces
an encrypted output file named "encrypted" through FileOutputStream. The encryption is accomplished by reading
the file 3 bytes by 3 bytes. For each block (the three bytes) of the data, we perform the encryption algorithm
to produce a four byte block. This four bytes block is then "decomposed" back to individual bytes and write to
the output encrypted file.

functionality:
The program works as well as expected, producing an encrypted file named "encrypted".

******************************************************
Part3: decryption

report:
This program is very similar to the encryption program. It takes two input files (keys and the input data to be decrypted) 
as FileInputStream and produces an encrypted output file named "decrypted" through FileOutputStream. The decryption is 
accomplished by reading the file 4 bytes by 4 bytes. For each block (the four bytes) of the data, we perform the 
decryption algorithm to produce a three byte block. This three bytes block is then "decomposed" back to individual bytes 
and write to the output decrypted file.

functionality:
The program works as well as expected, producing an decrypted file named "decrypted", which is exactly same as the file
to be encrypted.
********************Specified question to answer********************
How do you go about doing this(the encryption)?
	Ans: This is answered in the report of part 1.

Why the number formed by concatenating the three bytes block (in encryption) is less than n?
	Ans: The number of bits of three bytes is 24, so the maximum positive number it can represent is 2^23.
Since n is required to be between 2^24 and 2^30, it is sure that the number represented by the block is less 
than n.

Why the topmost byte of the number represented by the four bytes block (in decryption) will always be 0?
	Ans: If the decryption process is correct, the decrypted file produced should be exactly same as the
encrypted file. Since the 4 byted block is prodced by encrypting a 3 byte block, the decrypted 4 byte block
should have the same bits as the original 3 byte block, so the topmost byte will always be 0.
	
To see the detail of the implementations, check out the comments of the sourse code.