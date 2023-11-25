/*
 * This  code modified from sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <rpc/rpc.h>
#include <pwd.h>

#include "ssnfs.h"

CLIENT *clnt;
char *filename;
char *username;

int Open(char *filename_to_open);
void Read(int fd, char buffer[100], int numbytes);
void Write(int fd, char *buffer, int numbytes);
void List();
void Delete(char *arg1);
void Close(int fd);

void
ssnfsprog_1(char *host) {
	clnt = clnt_create (host, SSNFSPROG, SSNFSVER, "udp");
	if (clnt == NULL) {
		fprintf(stderr, "getting client handle failed"); 
		clnt_pcreateerror (host);
		exit (1);
	}
}

int 
Open(char *filename_to_open) {
	
	open_output  *result_1;
	open_input  open_file_1_arg;
  	
	strcpy(open_file_1_arg.user_name, username);
  	strcpy(open_file_1_arg.file_name,filename_to_open);
  	
	result_1 = open_file_1(&open_file_1_arg, clnt);
	if (result_1 == (open_output *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	
	// printf("OPEN -> DONE\n");
	printf(" File name is %s\n", (*result_1).out_msg.out_msg_val);
		
	return ( (*result_1).fd);
}

void 
Read(int fd, char buffer[100], int numbytes) { 
	read_input in;
  	read_output *outp;

 	strcpy(in.user_name, username); 
  	
	in.fd= fd;
  	in.numbytes = numbytes; 
  	
	outp = read_file_1(&in, clnt); 
  	
	memset(buffer, 0, (outp)->out_msg.out_msg_len + 1);
  	strncpy(buffer, (outp)->out_msg.out_msg_val, (outp)->out_msg.out_msg_len + 1);
	
	// printf("READ -> DONE\n");
	// printf("%s\n",buffer);
}

void 
Write(int fd, char *buffer, int numbytes) {
	char *output;
	write_input in;
	write_output *outp;
	
	strcpy(in.user_name, username); 
	
	in.fd=fd; 
	in.numbytes = numbytes; 
	in.buffer.buffer_val=strdup(buffer);
	in.buffer.buffer_len = strlen(buffer);
	
	outp = write_file_1(&in, clnt); 
	
	output = malloc((outp)->out_msg.out_msg_len + 1);
	memset(output, 0, (outp)->out_msg.out_msg_len + 1);
	strncpy(output, (outp)->out_msg.out_msg_val, (outp)->out_msg.out_msg_len + 1);
	
	printf("%s\n",output);
}

void 
List() {
	char *output;
	list_input in; 
	list_output *outp;

	strcpy(in.user_name, username); 

	outp = list_files_1(&in, clnt); 

	output = malloc((outp)->out_msg.out_msg_len + 1);
	memset(output, 0, (outp)->out_msg.out_msg_len + 1);
	strncpy(output, (outp)->out_msg.out_msg_val, (outp)->out_msg.out_msg_len + 1);

	printf("%s\n", output);
}

void 
Delete(char *arg1) { 
	char *output;
	delete_output *outp;
	delete_input in; 

	if (arg1) 
	{
		strcpy(in.user_name, username); 
		strcpy(in.file_name, arg1); 
		outp = delete_file_1(&in, clnt); 
	}

	output = malloc((outp)->out_msg.out_msg_len + 1);
	memset(output, 0, (outp)->out_msg.out_msg_len + 1);
	strncpy(output, (outp)->out_msg.out_msg_val, (outp)->out_msg.out_msg_len + 1);
	
	printf("%s\n", output);
} 

void 
Close(int fd) {
	char *output;
	close_input in;
	close_output *outp;
	
	in.fd=fd;
	
	outp=close_file_1(&in,clnt);
	
	output = malloc((outp)->out_msg.out_msg_len + 1);
	memset(output,0,(outp)->out_msg.out_msg_len + 1);
	strncpy(output,(outp)->out_msg.out_msg_val,(outp)->out_msg.out_msg_len + 1);
	
	printf("%s\n",output);
}

/*

	read_output  *result_2;
	read_input  read_file_1_arg;
	write_output  *result_3;
	write_input  write_file_1_arg;
	list_output  *result_4;
	list_input  list_files_1_arg;
	delete_output  *result_5;
	delete_input  delete_file_1_arg;
	close_output  *result_6;
	close_input  close_file_1_arg;
        seek_output * result_7;
        seek_input seek_position_1_arg;

	
	result_2 = read_file_1(&read_file_1_arg, clnt);
	if (result_2 == (read_output *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_3 = write_file_1(&write_file_1_arg, clnt);
	if (result_3 == (write_output *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_4 = list_files_1(&list_files_1_arg, clnt);
	if (result_4 == (list_output *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_5 = delete_file_1(&delete_file_1_arg, clnt);
	if (result_5 == (delete_output *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	result_6 = close_file_1(&close_file_1_arg, clnt);
	if (result_6 == (close_output *) NULL) {
		clnt_perror (clnt, "call failed");
	}
       	result_7 = seek_position_1(&seek_position_1_arg, clnt);
	if (result_7 == (seek_output *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	

*/

int
main (int argc, char *argv[]) {
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	ssnfsprog_1 (host);

	username = getpwuid(geteuid())->pw_name;
	
	int fd;
	fd=Open("MyFile");
	printf("File descripter retuned is %d \n",fd);
	
	int i,j;
	int fd1,fd2;
	char buffer[100];
	
	fd1=Open("File1"); // opens the file "File1"
	for (i=0; i< 20;i++){
		Write(fd1,  "This is a test program for cs570 assignment 4", 15);
	}
	Close(fd1);
	
	fd2=Open("File1");
	for (j=0; j< 20;j++){
		Read(fd2, buffer, 10);  
		printf("%s\n",buffer);
	}
	
	// Seek (fd2,40);
	// Read(fd2, buffer, 20);
	// printf("%s\n",buffer);
	
	Close(fd2);
	Delete("File1");
	List();

	exit (0);
}