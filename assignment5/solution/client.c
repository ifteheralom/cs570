
#include <stdio.h>
#include "ssnfs.h"
#include <pwd.h>
#include <rpc/rpc.h>
#include <string.h>

/*Variables generated by rpcgen template. I did not modify its declaration*/
CLIENT *clnt;
char * username;
	
	
/*Generated by rpcgen*/

/*Open <filename>*/
int Open(char *file_name){
	open_output  *result_1;
	open_input  open_file_1_arg;

	strcpy(open_file_1_arg.file_name,file_name);
	strcpy(open_file_1_arg.user_name, username);

	result_1 = open_file_1(&open_file_1_arg,clnt);
	if (result_1 == (open_output *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	return result_1->fd;
}
/*List */
void List(){
	list_output  *result_2;
	list_input  list_files_1_arg;
	
	strcpy(list_files_1_arg.user_name, username);
	result_2 = list_files_1(&list_files_1_arg, clnt);
	if (result_2 == (list_output *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	printf("%s\n",result_2->out_msg.out_msg_val);
}
/*delete <filename>*/
void Delete(char *file_name){
	delete_output  *result_3;
	delete_input  delete_file_1_arg;

	strcpy(delete_file_1_arg.file_name,file_name);
	strcpy(delete_file_1_arg.user_name, username);
	result_3 = delete_file_1(&delete_file_1_arg, clnt);
	if (result_3 == (delete_output *) NULL) {
		clnt_perror (clnt, "call failed"); 
	}
	printf("%s\n",result_3->out_msg.out_msg_val);
}

/*write <file descriptor> <buffer> <bytes> */
void Write(int fd, char *buffer, int bytes){
	write_output  *result_4;
	write_input  write_file_1_arg;

	write_file_1_arg.fd = fd;
	strcpy(write_file_1_arg.user_name, username);
	/*Checks if amount of bytes to be written are at most buffer's length*/
	if (bytes > strlen(buffer)){
		write_file_1_arg.numbytes=strlen(buffer);	
	} else {
		write_file_1_arg.numbytes = bytes;
	}
	write_file_1_arg.buffer.buffer_len = strlen(buffer);
	write_file_1_arg.buffer.buffer_val = malloc(write_file_1_arg.buffer.buffer_len);
	strcpy(write_file_1_arg.buffer.buffer_val,buffer);
	result_4 = write_file_1(&write_file_1_arg, clnt);
	if (result_4 == (write_output *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	printf("%s\n",result_4->out_msg.out_msg_val);
}

/*read <file descriptor> <bytes>*/ 
void Read(int fd,char *buffer,int numbytes){
	read_output  *result_5;
	read_input  read_file_1_arg;

	strcpy(read_file_1_arg.user_name, username);
	read_file_1_arg.fd=fd;
	read_file_1_arg.numbytes = numbytes;
	result_5 = read_file_1(&read_file_1_arg, clnt);
	if (result_5 == (read_output *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	printf("%s\n",result_5->out_msg.out_msg_val);	
	//strcpy(buffer,result_5->out_msg.out_msg_val);
}
/*close <file descriptr>*/
void Close(int *file_des){
	close_output  *result_6;
	close_input  close_file_1_arg;
	
	close_file_1_arg.fd = *file_des;
	result_6 = close_file_1(&close_file_1_arg, clnt);
	if (result_6 == (close_output *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	printf("%s\n",result_6->out_msg.out_msg_val);	
}


int main (int argc, char **argv)
{
	int i,j,k;
	int fd1=0,fd2=0,fd3=0;
	char buffer[255];
	/*Invalid input message*/
	if (argc != 2) { // If the user doesn't enter the correct number of parameters...
        fprintf(stderr, "usage: %s <hostname>\n", argv[0]); // Print out usage information.
        exit(1); // And exit with an error code.
    	}
    	clnt = clnt_create(argv[1], SSNFSPROG, SSNFSVER, "tcp"); // Create the RPC client.
  	if (clnt == NULL) { // If there was an error creating the client...
        	fprintf(stderr, "getting client handle failed"); // Report an error message.
        	exit(2); // And exit with an error code.
    	}

      	username = getpwuid(geteuid())->pw_name; // Get the user's username.
	
	fd1=Open("File1"); // opens the file
	printf("\n file descriptor we received is : %d",fd1);
	fd2=Open("File2");
	printf("\n file descriptor received is : %d",fd2);

	for (i=0; i<20;i++){
		Write(fd1, "This is a test program for cs570 assignment 5", 45);
	}
	Close(&fd1);

	for (j=0; j< 5;j++){
		Read(fd2, buffer, 10);
	}

	Close(&fd2);

	Delete("File1");
	List();
	exit (0);

}	
