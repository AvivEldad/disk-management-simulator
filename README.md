#disk management simulator



==Description==
The program simulate a disk managmnet by indexed allocation method. In this method, the disk divided (in virtual way) to blocks, and for each file, the system keep a managment block, that contain pointers to the information block of the file. the information blocks dont have to be continuous, and with the managment block the system can find all the inforamtion of a file from the disk.
The program manages the information about the files in the directory, write the inforamtion to the disk, reads from the disk, and also can delete files from the system.



==Classes and functions==

In the program we have 3 classes: 

1. fsFile - simulate a file in the system - getters ans setters functions
2. fileDescriptor - keep information on file  - getters ans setters functions
3. fsDisk - simulate the disk and the function of the disk:

8 main functions in fsDisk class:

1.fsFormat - The function format the disk - allocated all the data structures for the disk management and update the class fields. if the disk was formated before, the function delete the allocated structures and clear the disk file, before the action mentioned before.
  
2. createFile - The function create a new file on the disk and opens it.The function return the index of the file in the open file descriptor vector

3. openFile - The function open an existing file, in case the file is not already open

4. closefile - The function close an existing and open file, in case the file is not close

5. WriteToFile - The function write file information to the disk. if its the first time, an index block for the file is allocated on the disk. (side functions are: writeToNewBlock, writeToLastBlock, getBlock, decToBinary)

6.delFile - The function delete file from the disk and clear its information as well. In sucsses, the function return 1

7. ReadFromFile - The function read file information from the disk

8. listAll - The function prints all the existing files on the disk, if they are in use,  and the disk content (include manegmnets blocks)


==program database and data structers==

1.MainDir - a vector that present a directory on the computer. the vector contains all the file in the directory

2. OpenFileDescriptor - an array that contains all the files that are open now (limited for the size of the block in the disk)

3.bitVector - an array that points on the blocks in the disk. indicate which block is avalible



==Program Files==
ex7_final_proj.2021.cpp (including all the classes and main)

DISK_SIM_FILE.txt - the file that simulate the disk (must be exist before lunching the program)

==How to compile?==
compile: g++ diskManagementSimulator.cpp -o diskManagementSimulator.2021
run: ./diskManagementSimulator.2021


==Input:==
no input


==Output:==
The program prints the files on the disk and the contents of the disk


