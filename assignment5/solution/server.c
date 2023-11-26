#include <stdio.h> // For printf, etc.
#include <rpc/rpc.h> // For RPC facilities.
#include <string.h> // For strcpy, strcmp, strdup, strlen, etc.
#include "ssnfs.h" // Automatically generated rpc service header.
#include <unistd.h> // Needed for access, etc.
#include <fcntl.h> // Needed for ftruncate, etc.
#include <errno.h> // Need for errno, strerror, etc.

#define MAX_FILES 100 // The maximum number of files in the filesystem.
#define MAX_PAGES 64 // The maximum number of pages per file.
#define BLOCK_SIZE 512 // Blocks are 512 bytes.
#define PAGE_SIZE (BLOCK_SIZE*64) // Pages are 8 blocks.

struct file_info { // Holds the file information.
    char username[10]; // Username of the file's owner.
    char filename[10]; // Name of the file.
    int used; // Number of pages used.
    int pages[MAX_PAGES]; // A list of the page numbers used by the file.
}; // End of struct file_info.

struct state_table
{
  int fd;
  int write_offset;
  int read_offset;
  char *filename1;
} state[11];
int counter=0;
int store;
int result;
int j=0;
int i;
typedef struct file_info file_info; // Define as a type.

// Initializes the virtual disk files if they do not all exist already.(from previouus code)
int init_disk() {
    if ((access("files.dat", F_OK) == 0) && (access("pages.dat", F_OK) == 0) && (access("disk.dat", F_OK) == 0)) {
    } else {
        ftruncate(creat("disk.dat", 0666), 1024*1024*16);
        ftruncate(creat("pages.dat", 0666), 2560);
        ftruncate(creat("files.dat", 0666), sizeof(file_info) * MAX_FILES);
        printf("ftruncate ok\n");
    }
}

// Obtain a free page for a file to use.  Returns page number.(from previouus code)

int get_free_page() {
    
    int fd;
    int i;
    char flag;
    fd = open("pages.dat", O_RDWR);
    if (fd == -1)
        printf("%s\n", strerror(errno));
    read(fd, &flag, 1);
    for (i = 0; flag; i++) {
        if (read(fd, &flag, 1) == -1) {
            printf("%s\n", strerror(errno));
            break;
        }
    }
    lseek(fd, -1, SEEK_CUR);
    write(fd, "\xff", 1);
    close(fd);
    return i;
}

// Checks if a file exists.  Return 0 on false, 1 on true.(code modified)
int file_exists(char *username, char *filename) {
    int fd, exists,i;
    file_info fi;
    fd = open("files.dat", O_RDONLY);
    for(i=0;i<counter;i++)
    {
      if(strcmp(state[i].filename1,filename)==0)
     {
      state[counter].fd=counter+1;
      result=state[counter].fd;
      state[counter].read_offset=0;
      state[counter].write_offset=state[i].write_offset;
      state[counter].filename1=(char *)malloc(sizeof(filename));
      strcpy(state[counter].filename1,filename);
      counter++;
      return result;
     }
    }
    for (exists = 0; read(fd, &fi, sizeof(fi)) > 0;) 
    {
       if ((strcmp(username,fi.username) == 0) && (strcmp(filename, fi.filename) == 0)) {
            state[counter].fd=counter+1;
            state[counter].filename1=(char *)malloc(sizeof(filename));
            strcpy(state[counter].filename1,filename);
            state[counter].read_offset=0;
            state[counter].write_offset=0;
            result=state[counter].fd;
            //printf("%d is the counter value\n",counter);
            counter++;
            return result;
        }
    }
    close(fd);
    return exists;
}

// Get information on a file.  Takes the username, filename, and a pointer to a file_info struct.(from previouus code)

void get_file_info(char *username, char *filename, file_info *fi) {
    int fd, exists;
    fd = open("files.dat", O_RDONLY);
    for (exists = 0; read(fd, fi, sizeof(file_info)) > 0;) {
        if ((strcmp(username, fi->username) == 0) && (strcmp(filename, fi->filename) == 0)) {
            exists = 1;
            break;
        }
    }
    close(fd);
}

// Commit changes to a file_info struct.  Takes only a file_info struct pointer as input.(from previouus code)

void change_file_info(file_info *fi) {
    int fd, exists;
    file_info block;
    fd = open("files.dat", O_RDWR);
    for (exists = 0; read(fd, &block, sizeof(file_info)) > 0;) {
        if ((strcmp(fi->username, block.username) == 0) && (strcmp(fi->filename, block.filename) == 0)) {
            exists = 1;
            break;
        }
    }
    if (exists) {
        lseek(fd, -sizeof(file_info), SEEK_CUR);
        printf("used %d\n", fi->used);
        printf("write %zd\n", write(fd, fi, sizeof(file_info)));
    }
    close(fd);
}

// Inserts a new file_info struct into the file table.(from previouus code)

int add_file(file_info fi) {
    int fd,fd1;
    int found;
    file_info block;
    // search for an empty block
    fd = open("files.dat", O_RDWR);
    //fd1=open(
    for (found = 0; read(fd, &block, sizeof(block)) > 0;) 
     {
        
        if (block.username[0] == 0) 
        {
            state[counter].fd=counter+1;
            printf("%d is fd\n",state[counter].fd);
            state[counter].filename1=fi.filename;
            state[counter].read_offset=0;
            state[counter].write_offset=0;
            //printf("%d is the offset value\n",state[counter].offset);
          //  strcpy(state[counter].state,"OPEN");
        
            counter++;
            found = 1;
            break;
        }
    }
    // insert file
    if (found) {
        
        lseek(fd, -sizeof(fi), SEEK_CUR);
        printf("write %zd\n", write(fd, &fi, sizeof(fi)));
    }
    close(fd);
    // return success
    return state[counter].fd;
}

// Write a string with a list of files owned by a user to the buffer.
// Buffer must be at least 512 bytes!(from previouus code)

void file_list(char *username, char *buffer) {
    memset(buffer, 0 , 512);
    int fd;
    file_info fi;
    fd = open("files.dat", O_RDONLY);
    while (read(fd, &fi, sizeof(fi)) > 0) {
        if (strcmp(username, fi.username) == 0) {
            strcat(buffer, " ");
            strcat(buffer, fi.filename);
        }
    }
    close(fd);
}

// Free a page with given page number by setting its entry in the page table to 0.(from previouus code)

void free_page(int page_index) {
    int fd;
    fd = open("pages.dat", O_RDWR);
    lseek(fd, page_index, SEEK_SET);
    write(fd, "\x00", 1);
    close(fd);
}

// Delete a file owned by a user with filename as its name.(from previouus code)

int file_delete(char *username, char *filename) {
    int fd;
    int found, i;
    file_info fi;
    // search for file's file info block
    fd = open("files.dat", O_RDWR);
    for (found = 0; read(fd, &fi, sizeof(fi)) > 0;) {
        if ((strcmp(username, fi.username) == 0) && (strcmp(filename, fi.filename) == 0)) {
            found = 1;
            break;
        }
    }
    // delete file
    if (found) {
        // free disk pages
        for (i = 0; i < fi.used; i++) {
            free_page(fi.pages[i]);
        }
        lseek(fd, -sizeof(fi), SEEK_CUR);
        memset(&fi, 0, sizeof(fi));
        printf("write %zd\n", write(fd, &fi, sizeof(fi)));
    }
    close(fd);
    // return success
    return found;
}

// RPC call "create".(code modified)
open_output * open_file_1_svc(open_input *inp, struct svc_req *rqstp) {
    int fd, page;
    file_info fi;
    //char *file;
    char message[512];
    int desc;
    printf("open_file_1_svc: (user_name = '%s', file_name = '%s')\n", inp->user_name, inp->file_name);
    init_disk();
    desc=file_exists(inp->user_name,inp->file_name);
    
    if(desc!=0)
    {
      static open_output out;
       if(counter==10)
       {
        out.fd=-1;
        return &out;
       }
       out.fd = desc;
       return &out;
      
   }
   // printf("file Descriptor is %d\n", state[i].fd);
     else
     {
        static open_output out;
        printf("THE COUNTER VALUE IS %d",counter);
        if(counter==10)
        {
         out.fd=0;
         return &out;
        }
        else
        {
        strcpy(fi.username, inp->user_name);
        strcpy(fi.filename, inp->file_name);
        fi.used = 1;
        state[counter].fd=counter+1;
        result=state[counter].fd;
        state[counter].read_offset=0;
        state[counter].write_offset=0;
        //printf("%d\n",state[counter].offset);
        state[counter].filename1=(char *)malloc(sizeof(fi.username));
        strcpy(state[counter].filename1,inp->file_name);
        //   strcpy(state[counter].username.fi.username);
        counter++;
        page = get_free_page();
        if (page != -1)
        {
            printf("%d is the page number\n",page);
            fi.pages[0] = page;
        }
        else
            printf("no pages left");
        desc=add_file(fi);
        if (desc) {
            snprintf(message, 512, "%s created for user %s", inp->file_name, inp->user_name);
        } else {
            strcpy(message, "Error: Max number of files reached.");
        }
      }
    }
    static open_output out;
    out.fd = result;;
    return &out;

// RPC call "list".
}
list_output * list_files_1_svc(list_input *inp, struct svc_req *rqstp) 
{
    char message[512];
    char buffer[512];

    init_disk();

    printf("list_file_1_svc: (usrname = '%s')\n", inp->user_name);
    file_list(inp->user_name, buffer);
    printf("files: %s\n", buffer);
    static list_output out;
    snprintf(message, 512, "The files are:%s", buffer);
    out.out_msg.out_msg_len = strlen(message) + 1;
    out.out_msg.out_msg_val = strdup(message);
    return &out;
}

delete_output * delete_file_1_svc(delete_input *inp, struct svc_req *rqstp) {
    char message[512];

    init_disk();

    printf("delete_file_1_svc: (user_name = '%s', file_name = '%s')\n", inp->user_name, inp->file_name);
    static delete_output out;
    if (file_exists(inp->user_name, inp->file_name)) {
        file_delete(inp->user_name, inp->file_name);
        snprintf(message, 512, "%s deleted", inp->file_name);
    } else {
        // file doesn't exist
        snprintf(message, 512, "Error: file %s does not exist.", inp->file_name);
    }
    out.out_msg.out_msg_len = strlen(message) + 1;
    out.out_msg.out_msg_val = strdup(message);
    return &out;
}

//Code Modified
write_output * write_file_1_svc(write_input *inp, struct svc_req *rqstp) {
    char message[512];
    char *buffer;
    file_info fi;
    int j,store,flag=0;
    int fd, numbytes, offset, size, at, page_index, page_num, len;
    init_disk();
    printf("write_file_1_svc: (user_name = '%s', file_descriptor = '%d' numbytes = %d)\n",
           inp->user_name, inp->fd, inp->numbytes);
    printf("write buffer: %s\n", inp->buffer.buffer_val);
    static write_output out;
    for(j=0;j<counter;j++)
    {
     if(inp->fd==state[j].fd)
     store=j;
     flag=1;
    }
    if(flag==0)
   {
     
     snprintf(message, 512, "Cannot write to the file");
     out.out_msg.out_msg_len = strlen(message) + 1;
     out.out_msg.out_msg_val = strdup(message);
        return &out;
   }
 
    if (flag!=0) {
        numbytes = inp->numbytes < strlen(inp->buffer.buffer_val) ? inp->numbytes : strlen(inp->buffer.buffer_val);
        buffer = inp->buffer.buffer_val;
        get_file_info(inp->user_name, state[store].filename1, &fi);
        if (state[store].write_offset> (fi.used * PAGE_SIZE - 1)) {
            printf("used = %d\n", fi.used);
            snprintf(message, 512, "Error: writing past end of file.");
        } else if ((state[store].write_offset + numbytes) > (PAGE_SIZE * MAX_PAGES)) {
            snprintf(message, 512, "Error: write is too large.");
        } else {
            offset = state[store].write_offset;
            at = 0;
            while (at < numbytes) {
                page_index = offset / PAGE_SIZE;
                if (page_index == fi.used) {
                    page_num = get_free_page();
                    fi.pages[fi.used] = page_num;
                    fi.used++;
                    change_file_info(&fi);
                } else {
                    page_num = fi.pages[page_index];
                }
                if ((numbytes - at) < (PAGE_SIZE - (offset % PAGE_SIZE))) {
                    len = numbytes - at;
                } else {
                    len = PAGE_SIZE - (offset % PAGE_SIZE);
                }
                fd = open("disk.dat", O_RDWR);
                printf("lseek to %d\n", (PAGE_SIZE * page_num) + (offset % PAGE_SIZE));
                printf("%d %d %d\n", PAGE_SIZE, page_num, offset%PAGE_SIZE);
                lseek(fd, (PAGE_SIZE * page_num) + (offset % PAGE_SIZE), SEEK_SET);
                write(fd, buffer+at, len);
                close(fd);
                at += len;
                offset += len;
            }
            snprintf(message, 512, "%d characters written to file %s", numbytes, state[store].filename1);
        }
    } else {
        // file doesn't exist
        snprintf(message, 512, "Error: file %s does not exist.", state[store].filename1);
    }
    state[store].write_offset=state[store].write_offset + inp->numbytes;
     printf("FD\tFileName\tRead Offset\tWrite Offset\n");
    for(j=0;j<counter;j++)
    {
     printf("%d\t%s\t\t%d\t\t%d\t",state[j].fd,state[j].filename1,state[j].read_offset,state[j].write_offset);
     printf("\n\n");
    }
 
    out.out_msg.out_msg_len = strlen(message) + 1;
    out.out_msg.out_msg_val = strdup(message);
    return &out;
}
//Code modified
read_output * read_file_1_svc(read_input *inp, struct svc_req *rqstp) {
    static read_output out;
    char *message, *buffer;
    int offset, numbytes, at, page_index, page_num, len, fd, buffer_size, message_size, read_fail;
    file_info fi;
    int i,store1,flag=0;
    printf("entered the read function\n");
    for(i=0;i<counter;i++)
    {
     if(state[i].fd==inp->fd)
     {
      store1=i;
      flag=1;
      break;
     }
    }
   if(flag==0)
   {
     message = malloc(512);
     snprintf(message, 512, "Cannot read from the file");
     out.out_msg.out_msg_len = strlen(message) + 1;
     out.out_msg.out_msg_val = strdup(message);
        return &out;
   }
   
    init_disk();
    printf("read_file_1_svc: (user_name = '%s', fd= %d offset = %d numbytes = %d)\n",
           inp->user_name, inp->fd, state[store1].read_offset, inp->numbytes);
    //static read_output out;

    if (flag!=0) {
        offset = state[store1].read_offset;
        numbytes = inp->numbytes;
        at = 0;
        buffer_size = numbytes + 1;
        buffer = malloc(buffer_size);
        memset(buffer, 0, buffer_size);
        char *reply = "Content read: ";
        message_size = strlen(reply) + buffer_size;
        get_file_info(inp->user_name, state[store1].filename1, &fi);
        read_fail = 0;
        while (at < numbytes) {
            printf("at = %d\n", at);
            page_index = offset / PAGE_SIZE;
            if (page_index >= fi.used) {
                read_fail = 1;
                break;
            }
            page_num = fi.pages[page_index];
            if ((numbytes - at) < (PAGE_SIZE - (offset % PAGE_SIZE))) {
                len = numbytes - at;
            } else {
                len = PAGE_SIZE - (offset % PAGE_SIZE);
            }
            fd = open("disk.dat", O_RDONLY);
            lseek(fd, (PAGE_SIZE * page_num) + (offset % PAGE_SIZE), SEEK_SET);
            printf("len = %d\n", len);
            read(fd, buffer+at, len);
            close(fd);
            at += len;
            offset += len;
            printf("%s\n", buffer);
            state[store1].read_offset=state[store1].read_offset+inp->numbytes;
        }
        buffer[numbytes] = '\x00';
        if (read_fail) {
            message = malloc(512);
            snprintf(message, 512, "Error: writing past end of file.");
            printf("%s\n", message);
        } else {
            message = malloc(message_size);
            memset(message, 0, message_size);
            snprintf(message, message_size, "Content read: %s", buffer);
            printf("%s\n", message);
        }
    } else {
        message = malloc(512);
        snprintf(message, 512, "Cannot read from the file");
        out.out_msg.out_msg_len = strlen(message) + 1;
        out.out_msg.out_msg_val = strdup(message);
        return &out;
    }
     printf("FD\tFileName\tRead Offset\tWrite Offset\n");

    for(i=0;i<counter;i++)
    {
     
     printf("%d\t%s\t\t%d\t\t%d\t",state[i].fd,state[i].filename1,state[i].read_offset,state[i].write_offset);
     printf("\n\n");
    }

    out.out_msg.out_msg_len = strlen(message) + 1;
    out.out_msg.out_msg_val = strdup(message);
   // free(buffer);
   // free(message);
    printf("%s (%d)\n", out.out_msg.out_msg_val, out.out_msg.out_msg_len);
    return &out;
}
//removes the file entry from the state table
close_output * close_file_1_svc(close_input *inp, struct svc_req *rqstp)
{
 static close_output out;
 char *message;
 char *buffer;
 int store;
 for(i=0;i<counter;i++)
 {
  if(inp->fd==state[i].fd)
  {
   store=i;
   break;
  }
 }
 if(counter=1)
 counter=0;
 else
 {
 for(i=store;i<counter-1;i++)
 {
  state[i].fd=state[i+1].fd;
  strcpy(state[i].filename1,state[i+1].filename1);
  state[i].read_offset=state[i+1].read_offset;
  state[i].write_offset=state[i].write_offset;
 }
 counter--;
 }
 printf("File Descriptor\tFile name\tRead_offset\tWrite_offset\n");
 for(i=0;i<counter;i++)
 {
   printf("%d\t%s\t\t%d\t\t%d\t\n\n",state[i].fd,state[i].filename1,state[i].read_offset,state[i].write_offset);
 }
 message = malloc(512);
 snprintf(message, 512, "Closed the file with the file descriptor %d",inp->fd);
 out.out_msg.out_msg_len = strlen(message) + 1;
 out.out_msg.out_msg_val = strdup(message);
 //free(buffer);
 //free(message);
 printf("%s (%d)\n", out.out_msg.out_msg_val, out.out_msg.out_msg_len);
 return &out;
}

 
seek_output *
seek_position_1_svc(seek_input *argp, struct svc_req *rqstp)
{
	static seek_output  result;


	return &result;
}

