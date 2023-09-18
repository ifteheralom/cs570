<a  name="readme-top"></a>
<!-- PROJECT LOGO -->
<br  />
<div  align="center">
<h3  align="center">CS 570: Modern Operating Syetems</h3>
</div>

<!-- GETTING STARTED -->

## Assignment 1

  

### Concepts:

1. Interprocess communication through shared memory

2. Synchronization using semaphores

  

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

make clean all

```

4. Followinf executables will be created:
	* load, print, query, change, save
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
* **save:** Saves the current state of shared memory to a file and dumps the shared memory space and semaphores.

4. Run each executable in order to view or modify the records

5. Run `./save` once all changes are made and wish to terminate the program.

  

<!-- CONTACT -->

## Contact

  

Ifteher Alom: ifteheralom@uky.edu

  

<p  align="right">(<a  href="#readme-top">back to top</a>)</p>