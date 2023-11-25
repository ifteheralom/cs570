<a  name="readme-top"></a>
<!-- PROJECT LOGO -->
<br  />
<div  align="center">
<h3  align="center">CS 570: Modern Operating Syetems</h3>
</div>

<!-- GETTING STARTED -->

## Assignment 5

  

### Project:

1. Implement a Simple Stateful Network File Server (SSNFS) that supports remote file service model (just to make things easy, data caching is not supported). 

2. The file server will use a Linux file as a virtual disk to store the files created by the clients. The server and client should be implemented as Sun RPC server and client. 

3. For simplicity, we can think of the virtual disk as a sequence of blocks, each block containing 512 bytes. we also assume that the capacity of virtual disk is 16MB. Each user should be assigned by the server a home directory. Users do not have the ability to create subdirectories within their home directory.

### Functions description:

* Open_file: Opens the file with the given name in the user’s directory and returns a file descriptor (a positive integer) to the client. The file descriptor is used in subsequent reads and writes.

* Read_file: It reads the file by taking the file descriptor and the number of bytes to be read as arguments. The contents that are read are displayed on the client’s side.

* Write_file: It is used to write the contents into the file. The file is specified using the file descriptor as the argument. It also takes the text to be written and the number of bytes to be wriiten into the file as arguments.

* List_files: It lists the name of all files in the user’s directory.

* Delete_file: It deletes a file from the virtual disk. The function takes the name of the file to be deleted as an argument. A message is displayed on the client’s side after deleting the specified file.

* Close_file: It is used to close a file. No operation can be performed on the file after it is closed. A message is displayed on the client’s side after the file is closed.
  

### Compile and Run

1. Clone the repo:

```sh

git clone https://github.com/ifteheralom/cs570.git

```

2. Inside the git project root:

```sh

cd assignment5/solution/

```

3. Compile with CMake

```sh

make

```
4. Run Server on one terminal

```sh

./server

```
4. On another terminal window, run client

```sh

./client

```
 

<!-- CONTACT -->

## Contact

  

Ifteher Alom: ifteheralom@uky.edu

  

<p  align="right">(<a  href="#readme-top">back to top</a>)</p>