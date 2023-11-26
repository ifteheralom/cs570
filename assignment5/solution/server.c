#include "ssnfs.h"

#include <stdio.h>	 // For printf, etc.
#include <stdlib.h>  //for atoi()
#include <rpc/rpc.h> // For RPC facilities.
#include <string.h>  // For strcpy, strcmp, strdup, strlen, etc.
#include <unistd.h>  // Needed for access, etc.
#include <fcntl.h>   // Needed for ftruncate, etc.
#include <errno.h>   // Need for errno, strerror, etc.


#define NUM_USERS 10
#define MAX_FILES_PER_USER 10
#define FILE_TABLE_SIZE 10 /* i.e the maximum number of files currently opened by all users. 
							  If a file has been opened twice, it is counted as two open files */

/*struct for the filetable*/
struct element_ 
{
	char username[10]; 	  //user name. max of 9 char, the last is \0.
    char filename[10];    //file name. max of 9 char, the last is \0
    int current_position; //the position of a file pointer
    int fd; 			  //file descripter
	int bytesStart;		  //starting position of file
	int bytesEnd;		  //ending position of file
};
struct file_table_ 
{
	int entries_in_use; //number of row in file table that are filled (with info)
	struct element_ content[FILE_TABLE_SIZE];
};

typedef struct file_table_ file_table; //(struct file_table_) = file_table

file_table FileTable = {.entries_in_use = 0}; //global file table.

/*struct for storing space information of the virtual disk (so details may be changed by other programmers in future)*/
struct disk_info
{
    char * filename;
    unsigned int sizeofdisk;        //in bytes
    unsigned int blocksperfile;
    unsigned int sizeofablock;      //in bytes
    unsigned int maxusers;
    unsigned int bytesperfile;      //in bytes
    unsigned int totalfilesindisk;
    unsigned int filesperdirectory;
    unsigned int bytestouse;        //in bytes
    unsigned int bytesleftover;     //in bytes  
};
typedef struct disk_info disk_info;

//global variable, with a few initializations
disk_info theDisk = {.sizeofdisk = 10000000, .blocksperfile = 64, .sizeofablock = 512, .maxusers = 10};


void set_diskinfo()
{
	theDisk.bytesperfile = theDisk.blocksperfile * theDisk.sizeofablock;
    
    float totalnumblocks = theDisk.sizeofdisk / theDisk.sizeofablock;
    
    theDisk.totalfilesindisk = totalnumblocks / theDisk.blocksperfile;
    
    theDisk.filesperdirectory = theDisk.totalfilesindisk / theDisk.maxusers;
    
    theDisk.bytestouse = theDisk.totalfilesindisk * theDisk.bytesperfile;
    
    theDisk.bytesleftover = theDisk.sizeofdisk - theDisk.bytestouse;
}

// Initializes the virtual disk file (if it does not exist already).
int init_disk() 
{   
	//from sample code

	//access(): determines accessibility of a file. Returns 0 if it can be accessed.

	if ( (access("disk.dat", F_OK) != 0) && (access("metadata.dat", F_OK) != 0) )  //if the virtual disk file does not exist
	{   
		set_diskinfo(); //set up disk info in theDisk struct.
	
		//creat(): creates a new file or rewrites an existing one
		//ftruncate(): truncates a file to a specified length
        ftruncate(creat("disk.dat", 0666), theDisk.sizeofdisk); //permission 0666 gives owner, group and world permission to read and write the file.
		ftruncate(creat("metadata.dat", 0666), 1000); 		//1KB for metadata
    }
}

int create_file (open_input *inp) //returns 1 on success, 0 on failure
{
	char filename[10];
	char username[10];
	
	strcpy (filename, inp->file_name);
	strcpy (username, inp->user_name);
	
	//Must change only metadata.dat (to keep track of file placements for later read/write to disk.
	
	FILE * fp;
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
	
	int j = 0;
	int k = 0;
	int numfiles;
	char* token; 
	long wheretoappend; //position in which to append new file info
	int file1[2]; //[start, end]
	int file2[2]; //[start, end]
	char* concatenation;
	
	//open metadata
	fp = fopen("metadata.dat", "a+"); //open for reading and appending
	
	//go through lines to find username, then to file positions
	while ((read = getline(&line, &len, fp)) != -1)//getline returns -1 on failure to read a line (including end-of-file condition).
	{
		//complete line
		token = strtok(line, " "); //gets username in file
		
		if(strcmp(token, inp->user_name) == 0) //if they are equal / if username found in metadata
		{
			//check to see if user's directory has the maximum amount of files (theDisk.filesperdirectory is the max)
			
			//token will be number of files in user directory
			numfiles = atoi(strtok(0, " "));
			
			if(numfiles == theDisk.filesperdirectory) //max number of files in directory
			{
				fclose(fp); //close file before leaving function
				return 0;   //creation did NOT work because there is no space for it in the disk/user directory.
			}
			
			//go through files to see if we can fit the new file in between (instead of end) (just in case files have been deleted before)
			while(j < numfiles)
			{
				//check to see the bytes end of one file is consecutive to bytes start of the next file.
				//files should be in ascending order of bytes.
			
				//token will be file name
				token = strtok(0, " (");
				
				if(k == 0)
				{
					//token will be start byte
					file1[0] = atoi(strtok(0, " ,")); //start
					
					//token will be end byte
					file1[1] = atoi(strtok(0, " ,)"));	//end
					
					k = k + 1;
				}
				else
				{   
					//store position after last , and before filename
					wheretoappend = ftell(fp); //there may be problem with this due to how strtok() works. may not change pointer pos.
				
					//token will be start byte
					file2[0] = atoi(strtok(0, " ,")); //start
					
					//token will be end byte
					file2[1] = atoi(strtok(0, " ,)"));	//end
					
					//now compare file1[1] (end) with file2[0] (start)
					
					if(file2[0] != (file1[1] + 1)) //if start of second file is NOT the same as end of first file + 1...
					{
						//Add new file name in between file1 and file2
						
						char buffer1[8];
						char buffer2[8];
						snprintf(buffer1, 8,"%d",(file1[1] + 1)); //itoa((file1[1] + 1),buffer1,8);
						snprintf(buffer2, 8,"%d",(file1[1] + theDisk.bytesperfile)); //itoa((file1[1] + theDisk.bytesperfile),buffer1,8);
						
						concatenation = "";
						
						strcpy(concatenation, " ");
						
						strcat(concatenation, filename);
						strcat(concatenation, "(");
						strcat(concatenation, buffer1);//store start. use end of first file + 1
						strcat(concatenation, " ,");
						strcat(concatenation, buffer2);
						strcat(concatenation, "), ");
						
						fwrite(concatenation , 1 , sizeof(concatenation) , fp); //file pointer already pointing at location we want
						
						//Done! No need to keep checking bytes of files, the file has been created!
						
						fclose(fp); //close file before leaving function
						return 1; //successful creation of file
					}
					
					//File2's start = file1's end, they are consecutive, so continue checking the rest of the file byte's starts/ends.
					
					k = 0;
				}

				j = j + 1;
			}
			
			//We are outside of the file loop. All byte end/start pairs are consecutive. So, we must add the new file to the end.
			
			wheretoappend = ftell(fp);
			
			char buffer3[8];
			char buffer4[8];
			snprintf(buffer3, 8,"%d",(file2[1] + 1)); //itoa((file2[1] + 1),buffer3,8);
			snprintf(buffer4, 8,"%d",(file2[1] + theDisk.bytesperfile)); //itoa((file2[1] + theDisk.bytesperfile),buffer4,8);
			
			concatenation = "";
						
			strcpy(concatenation, ", ");
			
			strcat(concatenation, filename);
			strcat(concatenation, "(");
			strcat(concatenation, buffer3);//use end of last seen file + 1. store start of new file.
			strcat(concatenation, ",");
			strcat(concatenation, buffer4); //store end of new file.
			strcat(concatenation, ")");
			
			fwrite(concatenation , 1 , sizeof(concatenation) , fp); //file pointer already pointing at location we want
			
			fclose(fp); //close file before leaving function
			return 1; 
		}
    }
	
	//if out here, then something went wrong. It should have returned already.

	fclose(fp); //close file before leaving function
	return -1; //file not created, for unknown reason
}

int does_file_exist (open_input *inp) //yes = 1, no = 0
{
	char filename[10];
	char username[10];
	
	strcpy (filename, inp->file_name);
	strcpy (username, inp->user_name);
	
	FILE * fp;
	char * line = NULL;
    size_t len = 0;
    ssize_t read;
	
	fp = fopen("metadata.dat", "r");
	
	char* token; 
	
	int j = 0;
	int numfiles;
	
	//go through lines to find username, then to find file.
	while ((read = getline(&line, &len, fp)) != -1)//getline returns -1 on failure to read a line (including end-of-file condition).
	{
		//complete line
		token = strtok(line, " "); //gets username in file
		
		if(strcmp(token, inp->user_name) == 0) //if they are equal / if username found in metadata
		{
			//token will be number of files in user directory
			numfiles = atoi(strtok(0, " "));
			
			if(numfiles == 0) //no files in user directory
			{
				fclose(fp); //close file before leaving function
				return 0; //file does not exist because user directory has no files
			}
			
			while(j < numfiles)
			{
				//token will be file name
				token = strtok(0, " (");
				
				if(strcmp(token, inp->file_name) == 0) //found the filename in directory of user
				{
					fclose(fp); //close file before leaving function
					return 1; //file does exist (true)
				}
				
				//token will be start byte
				token = strtok(0, " ,");
				
				//token will be end byte
				token = strtok(0, " ,)");	

				j = j + 1;
			}
			
			if(j == numfiles)
			{
				fclose(fp); //close file before leaving function
				return 0; //checked all files and none matched, so file does not exist
			}
		}
    }
	
	//return 0 (=> file does NOT exist) because if it did exist then it would have returned a 1 already.
	
	fclose(fp); //close file before leaving function
	
	return 0; //file does not exist because metadata.dat is empty
	
}

open_output * open_file_1_svc(open_input *inp, struct svc_req *rqstp)
{
	static open_output  result;
	int creation_out;
	
	char filename[10];
	char username[10];
	strcpy (filename, inp->file_name);
	strcpy (username, inp->user_name);
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	int j = 0;
	char* token;
	int file[2]; //[start, end]
	int numfiles;
	
	int filedescriptor;

	//make sure disk exists
	init_disk();
	
	//if (file does not exist)
	if(does_file_exist(inp) == 0) //if file does not exist
	{
		//create file
		creation_out = create_file(inp);
		
		//if (no space on virtual disk for new file)
		if(creation_out == 0) //no space in virtual disk / in user directory.
		{
			//print error and return -1
			printf("\nFile could not be created. There is no space in the directory of %s.", inp->file_name);
			result.fd = -1;
			return &result;
		}
		else
		{	
			//if (creation failed)
			if(creation_out == -1) //something went wrong
			{
				//print error and return -1
				printf("\nError: Creation unsuccessful for unknown reasons.");
				result.fd = -1;
				return &result;
			}
		}
	}
		
	//Check capacity of file table.
	
	//if (no space in file table)
	if(FileTable.entries_in_use == 10) //the maximum files that can be open at a time is 10
	{
		//print "error"
		printf("\nError: Could not open file. The file table is full. You must close at least one file to perform task.\n");
		result.fd = -1;
	}
	else
	{
		//***
		//figure out start bytes and end bytes of file from metadata
		//***
		
		int found = 0;
		
		//open metadata
		fp = fopen("metadata.dat", "a+"); //open for reading and appending
		
		//go through lines to find username, then to file positions
		while (((read = getline(&line, &len, fp)) != -1) && (found == 0))//getline returns -1 on failure to read a line (including end-of-file condition).
		{
			//complete line
			token = strtok(line, " "); //gets username in file
			
			if(strcmp(token, inp->user_name) == 0) //if they are equal / if username found in metadata
			{
				//check to see if user's directory has the maximum amount of files (theDisk.filesperdirectory is the max)
				
				//token will be number of files in user directory
				numfiles = atoi(strtok(0, " "));
				
				while((j < numfiles) && (found == 0))
				{
					//token will be file name
					token = strtok(0, " (");
					
					if(strcmp(filename, inp->file_name) == 0) //found the filename in directory of user
					{
						//token will be start byte
						file[0] = atoi(strtok(0, " ,"));
					
						//token will be end byte
						file[1] = atoi(strtok(0, " ,)"));	
						
						found = 1;
					}

					j = j + 1;
				}
			}
		}
		
		fclose(fp);
		
		//***
		//create a file descriptor
		//***
		
		if(FileTable.entries_in_use == 0)
		{
			filedescriptor = 1;
		}
		else
		{
			//find biggest file descriptor. new descriptor will be an addition to it.
		
			int k;
			int max = 0;
			for(k = 0; k < FileTable.entries_in_use; k++)
			{
				if(FileTable.content[k].fd > max)
				{
					max = FileTable.content[k].fd; 
				}
			}
			
			filedescriptor = max + 1;
		}
		
		//***
		//add file into file table
		//***
		
		//add to first unused entry of table
		strcpy(FileTable.content[FileTable.entries_in_use].username, inp->user_name); 
		strcpy(FileTable.content[FileTable.entries_in_use].filename, inp->file_name); 
		FileTable.content[FileTable.entries_in_use].current_position = file[0]; //use start bytes of file
		FileTable.content[FileTable.entries_in_use].fd = filedescriptor;
		FileTable.content[FileTable.entries_in_use].bytesStart = file[0];
		FileTable.content[FileTable.entries_in_use].bytesEnd = file[1];
		
		FileTable.entries_in_use = FileTable.entries_in_use + 1;
		
		//store file descriptor in output structure
		result.fd = filedescriptor;
		
	}
	
	//return output structure
	return &result; //may either be -1 from error or the file descriptor
}

read_output * read_file_1_svc(read_input *inp, struct svc_req *rqstp)
{
	/*Files are read only sequentially from the beginning to the end.  Random access is not supported.*/

	static read_output  result;

	init_disk();
	
	char username[10];
	int filedescriptor;
	int numbytes;

	int current_pos;
	int startbytes;
	int endbytes;
	
	strcpy (username, inp->user_name);
	filedescriptor = inp->fd;
	numbytes = inp->numbytes;

	//Check if the file requested is open in the file table and, if it is, store file table data into local variables
	
	int found = 0;
	int k;
	int index;
	for(k = 0; ((k < FileTable.entries_in_use) && found == 0); k++)
	{
		if(FileTable.content[k].fd == filedescriptor)
		{
			found = 1;
			index = k;
			
			current_pos = FileTable.content[k].current_position;
			startbytes = FileTable.content[k].bytesStart;
			endbytes = FileTable.content[k].bytesEnd;
		}
	}

	//if (file not open)
	if(found == 0)
	{
		//output error and exit function
			
		printf("Error: File not open. Can't read file that is not open.");
		
		//store output msg into structure
		result.out_msg.out_msg_len = strlen("Error: File not open. Can't read file that is not open.") + 1;
		result.out_msg.out_msg_val = strdup("Error: File not open. Can't read file that is not open.");
		
		//exit the function
		return &result;
	}
	
	//file is open!
	
	//check if user is trying to read past end of file
	
	if((numbytes + current_pos) > endbytes)
	{
		//output error and exit function
			
		printf("Error: Can't read past end of file.");
		
		//store output msg into structure
		result.out_msg.out_msg_len = strlen("Error: Can't read past end of file.") + 1;
		result.out_msg.out_msg_val = strdup("Error: Can't read past end of file.");
		
		//exit the function
		return &result;
	}
	
	//declare buffer variable for reading
	
	char *buffer; //to store what is read
	
	//open disk.dat
	
	FILE * fp;	
	fp = fopen("disk.dat", "r");
	
	//go to current_pos in disk file
	
	fseek(fp, current_pos, SEEK_SET);
	
	//Read from file starting at 'current position' using fread() and store in a buffer
	
	fread(buffer, numbytes, 1, fp);
		
	//close file
	fclose(fp);	
		
	//store output msg into structure
	result.out_msg.out_msg_len = strlen(buffer) + 1;
	result.out_msg.out_msg_val = strdup(buffer);
	
	//return output structure
	return &result;
}

write_output * write_file_1_svc(write_input *inp, struct svc_req *rqstp)
{
	/*Files are written only sequentially from the beginning to the end.  Random access is not supported.*/

	static write_output  result;
	
	init_disk();
	
	char username[10];
	int filedescriptor;
	int numbytes;
	char *buffer;

	int current_pos;
	int startbytes;
	int endbytes;
	
	strcpy (username, inp->user_name);
	filedescriptor = inp->fd;
	numbytes = inp->numbytes;
	strcpy(buffer, inp->buffer.buffer_val);

	//Check if the file requested is open in the file table and, if it is, store file table data into local variables
	
	int found = 0;
	int k;
	int index;
	for(k = 0; ((k < FileTable.entries_in_use) && found == 0); k++)
	{
		if(FileTable.content[k].fd == filedescriptor)
		{
			found = 1;
			index = k;
			
			current_pos = FileTable.content[k].current_position;
			startbytes = FileTable.content[k].bytesStart;
			endbytes = FileTable.content[k].bytesEnd;
		}
	}

	//if (file not open)
	if(found == 0)
	{
		//output error and exit function
			
		printf("Error: File not open. Can't write into file that is not open.");
		
		//store output msg into structure
		result.out_msg.out_msg_len = strlen("Error: File not open. Can't write into file that is not open.") + 1;
		result.out_msg.out_msg_val = strdup("Error: File not open. Can't write into file that is not open.");
		
		//exit the function
		return &result;
	}
	
	//file is open!
	
	//check if user is trying to write past end of file
	
	if((numbytes + current_pos) > endbytes)
	{
		//output error and exit function
			
		printf("Error: Can't write past end of file.");
		
		//store output msg into structure
		result.out_msg.out_msg_len = strlen("Error: Can't write past end of file.") + 1;
		result.out_msg.out_msg_val = strdup("Error: Can't write past end of file.");
		
		//exit the function
		return &result;
	}
	
	//open disk.dat
	
	FILE * fp;	
	fp = fopen("disk.dat", "r");
	
	//go to current_pos in disk file
	
	fseek(fp, current_pos, SEEK_SET);
	
	//Write into file starting at 'current position' using fwrite()
	
	fwrite(buffer, numbytes, 1, fp); //write contents of buffer which have a total size of numbytes
	
	//close file
	fclose(fp);
		
	//store output msg into structure
	result.out_msg.out_msg_len = strlen(buffer) + 1;
	result.out_msg.out_msg_val = strdup(buffer);

	//return output structure
	return &result;
}

list_output * list_files_1_svc(list_input *inp, struct svc_req *rqstp)
{
	static list_output  result;
	
	init_disk();

	//Read metadata in virtual disk.
	//Store list of file names into output structure (acquired from loop), it'll be a concatenated string.
	
	char username[10];
	strcpy (username, inp->user_name); //current user

	FILE * fp;
	char * line = NULL;
	size_t len = 0;		
	ssize_t read;		
		
	fp = fopen("metadata.dat", "r");
				
	char* token; 
	char* the_list;
			
	int j = 0;	
	int numfiles;
	int done = 0; //0 if not done, 1 if done
	
	//go through lines to find username, then to find file.
	while (((read = getline(&line, &len, fp)) != -1) && (done == 1))//getline returns -1 on failure to read a line (including end-of-file condition).
	{
		//complete line
		token = strtok(line, " "); //gets username in file
		
		if(strcmp(token, inp->user_name) == 0) //if they are equal / if username found in metadata
		{
			//token will be number of files in user directory
			numfiles = atoi(strtok(0, " "));

			while(j < numfiles)
			{
				//token will be file name
				token = strtok(0, " (");
				
				//add filename to the_list
				
				strcat(the_list, token);
				
				if(j != numfiles - 1) //not last file
				{
					strcat(the_list, ", ");
				}
				
				//token will be start byte. skip it!
				token = strtok(0, " ,");
				
				//token will be end byte. skip it!
				token = strtok(0, " ,)");	

				j = j + 1;
			}
			
			done = 1;
		}
    }

	fclose(fp); //close file
	
	result.out_msg.out_msg_len = strlen(the_list) + 1;
	result.out_msg.out_msg_val = strdup(the_list);
	
	//return output structure
	return &result;
}

delete_output * delete_file_1_svc(delete_input *inp, struct svc_req *rqstp)
{
	static delete_output  result;
	open_input inp2; //for does_file_exist() input
	
	char filename[10];
	char username[10];
	int index;
	
	strcpy (filename, inp->file_name);
	strcpy (username, inp->user_name);
	
	strcpy (filename, inp2.file_name);
	strcpy (username, inp2.user_name);
	
	init_disk();

	//check if filename is in file table.
	int found = 0;
	int k;
	for(k = 0; ((k < FileTable.entries_in_use) && found == 0); k++)
	{
		if(strcmp(FileTable.content[k].filename, filename) == 0)
		{
			found = 1;
			index = k;
		}
	}
	
	//if (file is in file table)
	if(found == 1)
	{
		//output error and exit function
			
		printf("Error: The file you are trying to delete is open. Close file first by using Close().");
		
		//store output msg into structure
		result.out_msg.out_msg_len = strlen("Error: The file you are trying to delete is open. Close file first by using Close().") + 1;
		result.out_msg.out_msg_val = strdup("Error: The file you are trying to delete is open. Close file first by using Close().");
		
		//exit the function
		return &result;
	}
	else
	{
		//check if the file exists by calling does_file_exist()
		if(does_file_exist(&inp2) == 0) //if file does not exist
		{
			//output error and exit function
			
			printf("Error: File does not exist. Can't delete an non-existent file.");
			
			//store output msg into structure
			result.out_msg.out_msg_len = strlen("Error: File does not exist. Can't delete an non-existent file.") + 1;
			result.out_msg.out_msg_val = strdup("Error: File does not exist. Can't delete an non-existent file.");
			
			//exit the function
			return &result;
		}
	}
	
	//File exists.
	
	//Remove the file info from the metadata.
		
	char *buffer;
	char *ptr;
	
	buffer = (char *)malloc(1000*sizeof(char));
	memset(buffer,0,1000*sizeof(char));
	ptr = buffer;
	
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	
	char* token;
	int numfiles;
	int j = 0;
	
	fp = fopen("metadata.dat", "r");
	
	//go through lines to find username, then to file positions
	while (((read = getline(&line, &len, fp)) != -1))//getline returns -1 on failure to read a line (including end-of-file condition).
	{
		//complete line
		token = strtok(line, " "); //gets username in file
		
		if(strcmp(token, inp->user_name) == 0) //if they are equal / if username found in metadata
		{
			//put username in buffer
			strcpy(ptr, token);
			ptr = ptr + strlen(token); 
			
			//put numfiles in buffer
			token = strtok(0, " ");
			strcpy(ptr, token);
			ptr = ptr + strlen(token); 
			
			numfiles = atoi(token);
			
			while(j < numfiles)
			{
				//token will be file name
				token = strtok(0, " (");
				
				if(strcmp(filename, inp->file_name) == 0) //found the filename we want to delete
				{
					//don't add filename nor file info to buffer
					
					//skip start byte
					token = strtok(0, " ,");
				
					//skip end byte
					token = strtok(0, " ,)");
				}
				else
				{
					//put info in buffer
					
					//filename
					strcpy(ptr, token);
					ptr = ptr + strlen(token);

					//start byte
					token = strtok(0, " ,");
					strcpy(ptr, token);
					ptr = ptr + strlen(token);
				
					//end byte
					token = strtok(0, " ,)");
					strcpy(ptr, token);
					ptr = ptr + strlen(token);
					
				}

				j = j + 1;
			}
		}
		else
		{
			//put line in buffer
			strcpy(ptr, line);
			ptr = ptr + strlen(line); 
		}
	}
	
	fclose(fp);
	
	//overwrite metadata.dat with the data put into buffer
	
	fp = fopen("metadata.dat", "w"); //overwrites file
	fprintf(fp, "%s", buffer);
	fclose(fp);
			
	//Remove the file content from the disk (that is within the blocks/bytes allocated for the file).
	
	//Store output msg into structure
	result.out_msg.out_msg_len = strlen("Successful deletion.") + 1;
	result.out_msg.out_msg_val = strdup("Successful deletion.");
	
	//Return structure.
	return &result;
}

close_output * close_file_1_svc(close_input *inp, struct svc_req *rqstp)
{
	static close_output  result;
	
	int filedescriptor;
	filedescriptor = inp->fd;
	
	init_disk();

	//Look up filename in file table
	
	int found = 0;
	int k;
	int index;
	for(k = 0; ((k < FileTable.entries_in_use) && found == 0); k++)
	{
		if(FileTable.content[k].fd == filedescriptor)
		{
			found = 1;
			index = k;
		}
	}
	
	//if (filename not found)
	if(found == 0)
	{
		//output error and exit function
			
		printf("Error: File was not found in file table. Can't close a non-opened file.");
		
		//store output msg into structure
		result.out_msg.out_msg_len = strlen("Error: File was not found in file table. Can't close a non-opened file.") + 1;
		result.out_msg.out_msg_val = strdup("Error: File was not found in file table. Can't close a non-opened file.");
		
		//exit the function
		return &result;
	}
	
	//file is in the table
	
	//remove entry/file from file table
	
	int i;
	int j;
	int l = FileTable.entries_in_use;
	
	for(i = 0; i < l; i++)
	{
		if(FileTable.content[i].fd == filedescriptor)
		{
			for(j = i; j < l; j++)
			{
				//shift up elements
				strcpy(FileTable.content[j].username, FileTable.content[j+1].username);
				strcpy(FileTable.content[j].filename, FileTable.content[j+1].filename);
				FileTable.content[j].current_position =FileTable.content[j+1].current_position;
				FileTable.content[j].fd = FileTable.content[j+1].fd;
				FileTable.content[j].bytesStart = FileTable.content[j+1].bytesStart;
				FileTable.content[j].bytesEnd = FileTable.content[j+1].bytesEnd;
			}
			
			l--;    //Decreasing the length of the array
			i--;    //check again from same index i
		}
	}
	
	//store output msg into structure
	result.out_msg.out_msg_len = strlen("Success!") + 1;
	result.out_msg.out_msg_val = strdup("Success!");

	return &result;
}
 
seek_output *
seek_position_1_svc(seek_input *argp, struct svc_req *rqstp)
{
	static seek_output  result;


	return &result;
}

