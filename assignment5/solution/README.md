<a  name="readme-top"></a>
<!-- PROJECT LOGO -->
<br  />
<div  align="center">
<h3  align="center">CS 570: Modern Operating Syetems</h3>
</div>

<!-- GETTING STARTED -->

## Assignment 1

  

### Concepts:

1. Implement a Simple Stateful Network File Server (SSNFS) that supports remote file service model (just to make things easy, data caching is not supported). 

2. The file server will use a Linux file as a virtual disk to store the files created by the clients. The server and client should be implemented as Sun RPC server and client. 

3. For simplicity, we can think of the virtual disk as a sequence of blocks, each block containing 512 bytes. we also assume that the capacity of virtual disk is 16MB. Each user should be assigned by the server a home directory. Users do not have the ability to create subdirectories within their home directory.

### System calls:

* shmget, shmctl, shmat, shmdt, semget, semctl, semop, etc

  

### Compile and Run

1. Clone the repo:

```sh

git clone https://github.com/ifteheralom/cs570.git

```

2. Inside the git project root:

```sh

cd assignment1/solution/

```

3. Compile with CMake

```sh

make reset all

```

4. Followinf executables will be created:
	* load, print, query, change, clean
* **load:**  Loads the student information from file to shared memory.
* **print:**  Prints the current information available on shared memory.
* **query:** Query a record in shared memory using command: `./query <studentID>`
* **change:** Perform any modification in the student information.
	* **Update:** Update an attribute in student info using command `./change 1 ABCD <studentID> <attributeNumber> <Change>`
		* Update Name: `./change 1 ABCD <studentID> 1 <New Name>`
		* Update Address: ` ./change 1 ABCD <studentID> 2 <New Address>`
		* Update Telephone: ` ./change 1 ABCD <studentID> 3 <New Telephone>`
	* **Delete:** Delete a student record. `./change 2 ABCD <studentID>`
	* **Add:** Add a new student record. `./change 3 ABCD <9-digit-studentID> <Name> <Address> <Telephone>`. Pls note, for simplicity of implementation, I did not consider spaces. So *Name* and *Address* must be one word only. 
* **clean:** Saves the current state of shared memory to a file and dumps the shared memory space and semaphores.

4. Run each executable in order to view or modify the records

5. Run `./clean` once all changes are made and wish to terminate the program.

  

<!-- CONTACT -->

## Contact

  

Ifteher Alom: ifteheralom@uky.edu

  

<p  align="right">(<a  href="#readme-top">back to top</a>)</p>