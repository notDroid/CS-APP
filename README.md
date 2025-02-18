# Computer Systems: A Programmer's Perspective, 3/E (CS:APP3e)
![image](https://github.com/user-attachments/assets/08041e99-861f-464b-b664-dbbcc320fd8c)

## Description
The book material provides a programmer's view of how computer systems execute programs, store information, and communicate. It enables students to become more effective programmers, especially in dealing with issues of performance, portability and robustness. It also serves as a foundation for courses on compilers, networks, operating systems, and computer architecture, where a deeper understanding of systems-level issues is required. Topics covered include: machine-level code and its generation by optimizing compilers, performance evaluation and optimization, computer arithmetic, memory organization and management, networking technology and protocols, supporting concurrent computation, etc..

## Lab Assignments
All labs are avaliable on the [cs:app website](https://csapp.cs.cmu.edu/).

- [x] Lab 1 [**Data Lab**](https://github.com/notDroid/CS-APP/tree/main/Lab1-DataLab): Students implement simple logical, two's complement, and floating point functions, but using a highly restricted subset of C. For example, they might be asked to compute the absolute value of a number using only bit-level operations and straightline code. This lab helps students understand the bit-level representations of C data types and the bit-level behavior of the operations on data.
- [x] Lab 2 [**Bomb Lab**](https://github.com/notDroid/CS-APP/tree/main/Lab2-BombLab): A "binary bomb" is a program provided to students as an object code file. When run, it prompts the user to type in 6 different strings. If any of these is incorrect, the bomb "explodes," printing an error message and logging the event on a grading server. Students must "defuse" their own unique bomb by disassembling and reverse engineering the program to determine what the 6 strings should be. The lab teaches students to understand assembly language, and also forces them to learn how to use a debugger.
<img src = "https://github.com/user-attachments/assets/df5e4774-7642-4897-9148-d4a2c7e4f658" width="800">

- [x] Lab 3 [**Attack Lab**](https://github.com/notDroid/CS-APP/tree/main/Lab3-AttackLab): Students are given a pair of unique custom-generated x86-64 binary executables, called targets, that have buffer overflow bugs. One target is vulnerable to code injection attacks. The other is vulnerable to return-oriented programming attacks. Students are asked to modify the behavior of the targets by developing exploits based on either code injection or return-oriented programming. This lab teaches the students about the stack discipline and teaches them about the danger of writing code that is vulnerable to buffer overflow attacks.

<img src="https://github.com/user-attachments/assets/6ce4346f-8978-4d5d-b11b-529a293d8ba4" width="400">
<img src="https://github.com/user-attachments/assets/82de30c7-6e24-4390-b6f1-b5b03ddefc55" width = "400">

- [x] Lab 4 [**Architecture Lab**](https://github.com/notDroid/CS-APP/tree/main/Lab4-ArchLab): Students are given a small default Y86-64 array copying function and a working pipelined Y86-64 processor design that runs the copy function in some nominal number of clock cycles per array element (CPE). The students attempt to minimize the CPE by modifying both the function and the processor design. This gives the students a deep appreciation for the interactions between hardware and software.

![image](https://github.com/user-attachments/assets/32d5b758-6c2f-4136-bb24-7e72bdf50d9c)
![image](https://github.com/user-attachments/assets/bd3ef8cc-79cb-46a4-ad8a-e551fc8ad443)

- [ ] [Cache Lab]()
- [ ] [Shell Lab]()
- [ ] Lab 7 [**Malloc Lab**](https://github.com/notDroid/CS-APP/tree/main/Lab7-MallocLab): Students implement their own versions of malloc, free, and realloc. This lab gives students a clear understanding of data layout and organization, and requires them to evaluate different trade-offs between space and time efficiency. One of our favorite labs. When students finish this one, they really understand pointers!

- [ ] [Proxy Lab]()
