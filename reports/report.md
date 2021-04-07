# Little Computer 3 Virtual Machine

> Gati Aher

> Caitlin Coffey

> Navi Boyalakuntla

> Zoe McGinnis

---

## The Project

---

Little Computer 3 (LC-3) is a reduced instruction set computer (RISC). This means that its architecture uses a smaller number of optimized instructions to complete tasks in comparison with modern personal computers. The fastest computer in the world, Fugaku, also uses the RISC model, as do most smartphones and microprocessors. 

LC-3 only has 15 instructions and uses 10 registers which makes in impractical for most programs. It is used to introduce computer architecture, virtual machine, and emulator concepts for educational purposes. 

The aim of this project is to create a virtual machine in C that can run compiled LC-3 assembly. Given an OBJ file, this VM will be able to execute the instructions in this file. In order to do this, it will use a memory structure defined in the LC-3 specification including program memory, program registers, and condition flags. It will be able to read and write to these memory structures and manage I/O through the terminal. 

The final goal of this virtual machine is to run games written in LC-3. 

By completion of this project, we aim to learn more about the inner mechanics of a virtual machine: how processes run in operating systems, how to implement a memory structure given a specification, and how I/O system calls are implemented.

## Implementation

---

### Memory Specification

LC-3 is a register-based virtual machine. This differs from other VM's like the Java VM which is stack-based. 

Registers are locations in memory that are easily accessible to the CPU. In our implementation, these registers are represented as an enum as it allows incrementing easily. The values stored in these registers are stored in an array. All registers in the LC-3 specification hold 16 bits, except for the condition register which holds 3 bits. 

There are 8 general purpose registers, 1 condition register, and the program counter (PC) register. The PC functions as a pointer to which 16-bit instruction should be executed next. As a result, the PC is continually incremented dependent on the previously run instruction.

There are 4 memory-mapped registers to handle user input and display to the terminal:

- Keyboard status register (KSR)
- Keyboard data register (KDR)
- Display status register (DSR)
- Display data register (DDR)

These registers help incrementally store user input and information to display. This prevents overwriting data in the general registers when handling I/O operations.

LC-3 also uses disk space in addition to registers. In our implementation, this is represented as an array where each entry can store 16 bits of data. Our implementation of disk space follows the LC-3 specification described in Fig. 1.

![Little%20Computer%203%20Virtual%20Machine%208d754acd344b4e4192bde45d9436f727/Untitled.png](Little%20Computer%203%20Virtual%20Machine%208d754acd344b4e4192bde45d9436f727/Untitled.png)

Fig 1: Memory map of LC-3 VM.

At the beginning of the program loop, all of the machine instructions are loaded into a portion of memory. The PC points to the instruction loaded in memory to execute. 

The trap vector table functions as a table of pointers to system call implementations stored in memory.

### Program Execution

LC-3 binaries are provided in big endian where most modern computer systems require little endian. When loading in these instructions, they are converted to little endian for this reason.

These instructions are then decoded to determine which operation to execute. The first four bits are unique to each operation. The break down of the available instructions is shown in Fig. 2. These instructions are broken down using bit shifting and masking.

In our implementation, each instruction takes approximately one clock cycle to compute. 

![Little%20Computer%203%20Virtual%20Machine%208d754acd344b4e4192bde45d9436f727/Untitled%201.png](Little%20Computer%203%20Virtual%20Machine%208d754acd344b4e4192bde45d9436f727/Untitled%201.png)

Fig. 2: Instruction breakdown for instruction decoding.

Once the instruction is decoded, the corresponding function that executes its functionalty is called. Below are the instructions and their corresponding functions.

[Instructions](https://www.notion.so/b1b19e2cfe954a4788b5a4d25ff46dc6)

For further information on thse instructions reference the following link: [https://justinmeiners.github.io/lc3-vm/supplies/lc3-isa.pdf](https://justinmeiners.github.io/lc3-vm/supplies/lc3-isa.pdf)

In our implementation, all instructions that share an operation code are executed using the same function. This is due to our switching logic between opcodes. An enum structure is used to represent the 16 opcodes. Any additional logic occuring beyond the 4 bit operation code is implemented in the functions themselves.

An additional avenue was explored that is closer to hardware for switching between instruction execution functions. Each seperate execution would be contained in its own function. This avenue was overwritten due to the complexity that it introduced in the code.

TRAP system calls act like an interrupt in that they save the current program position, execute the syscall, and return to the saved position once completed. In each TRAP instruction is a vector which designates which system call to execute. In the LC-3 specification, the TRAP code implementations are stored in memory. The pointers to these implementations is stored at the beginning of memory in the TRAP vector table. 

Our implementation does not follow the LC-3 specification here. Once the correct TRAP code to execute is identified, a corresponding function is called. This decision was made for simplicity of the program.

[TRAP Vector Syscalls](https://www.notion.so/b27f49e6cee74db8ae58ee79957ed293)

### I/O Handling

These TRAP codes are used to handle I/O functionality. Further, special functions specific to an operating system are required in order to get user input and write to the terminal correctly. The OS of choice for our implementation is Unix. 

We cannot read and write to memory directly, so we must handle setting the keyboard status register and getting the data from the keyboard data register in our code. We do this in our `read_from_memory()` function, which also makes use of the `check_key()` function. We must update the data stored at the keyboard status and data registers when we are done. Here is a snippet of our code from `read_from_memory()`: 

```c
uint16_t read_from_memory(uint16_t address)
{
  if (address == M_KBSR) {
    // we check to see if the address is coming from keyboard status
    if (check_key()) {
      // keeping track of status
      memory[M_KBSR] = (1 << 15);
      // accessing last char from keyboard data register because we know that we need the value,
      // as it just updated
      memory[M_KBDR] = getchar();
    }
    else {
      // updating the value at keyboard status back to 0 because the hardware won't do it
      memory[M_KBSR] = 0;
    }
  }
  return memory[address];
}
```

The `check_key()` code was taken from another LC-3 VM implementation as it is specific to an OS and is boilerplate in order to implement an LC-3 VM. 

### Main Loop

Rather than consolidating all our code into one C file, we used multiple C files and header files to speed up the compile time. We use a Makefile to link the OBJ files together when we want to run the program, and to recompile any object files that were modified. 

In our LC-3 VM's main function, we follow a classic RISC pipeline, detailed as follows:

1. Load one instruction from memory at the address stored in the program counter register.
2. Isolate the first four bits of the instruction and use a switch statement to determine which operation code it is. 
3. Call the corresponding function to execute the operation code. 
4. Access the registers and memory to load and store values from the operation code execution.
5. Update the general registers with the output and update condition registers with 100, 010, or 001 value if the final output is negative, zero, or positive, respectively.\
6. Increment the program counter.

Here is a snippet of our code that highlights our use of a classic RISC pipeline to execute the operation code:

```c
int main(int argc, const char *argv[])
{
  if (argc <= 1)
  {
	...
  }

  // make it work with unix terminal
  signal(SIGINT, handle_interrupt);
  disable_input_buffering();

  // File path to to program LC-3 should run
  const char *path_to_code = argv[1];
  read_program_code_into_memory(path_to_code);

  /* 0x3000 is the default PC position, start of memory available for user programs. */
  uint16_t PC_INIT = 0x3000;
  reg[R_PC] = PC_INIT;

  while (1)
  {
    uint16_t instruction = read_from_memory(reg[R_PC]++); // load instruction from memory
    uint16_t opcode = instruction >> 12; // look at opcode part of instruction

    switch (opcode) {
... // calling functions here depending on the opcode given
// the functions called will update the general registers + conditional registers
}
```

## Results

---

All code can be found here: [https://github.com/caitlincoffey/SoftSysGarbageEaters](https://github.com/caitlincoffey/SoftSysGarbageEaters)

The VM created is able to run two games: 2048 and Rogue.  Both of these games were written in LC-3 assembly. This means that the original goal of this project was accomplished. 

2048 running in our VM is shown in Fig. 3 and Fig. 4. 

![Little%20Computer%203%20Virtual%20Machine%208d754acd344b4e4192bde45d9436f727/Untitled%202.png](Little%20Computer%203%20Virtual%20Machine%208d754acd344b4e4192bde45d9436f727/Untitled%202.png)

Fig. 3: 2048 initial screen

![Little%20Computer%203%20Virtual%20Machine%208d754acd344b4e4192bde45d9436f727/Untitled%203.png](Little%20Computer%203%20Virtual%20Machine%208d754acd344b4e4192bde45d9436f727/Untitled%203.png)

Fig. 4: 2048 game screen

Rogue running in our VM is shown in Fig. 5 and Fig. 6. 

![Little%20Computer%203%20Virtual%20Machine%208d754acd344b4e4192bde45d9436f727/Untitled%204.png](Little%20Computer%203%20Virtual%20Machine%208d754acd344b4e4192bde45d9436f727/Untitled%204.png)

Fig. 5: Rogue initial screen

![Little%20Computer%203%20Virtual%20Machine%208d754acd344b4e4192bde45d9436f727/Untitled%205.png](Little%20Computer%203%20Virtual%20Machine%208d754acd344b4e4192bde45d9436f727/Untitled%205.png)

Fig. 6: Rogue game screen.

## Reflection

---

This project met the lower bound discussed earlier in the proposal. Although this met the lower bound, getting a working VM proved to be a significantly greater feat than intiially expected. Debugging the code took a lot more time than anticipated.

Once all of the code was written, we wrote tests for the instruction execution functions. Perhaps, if more tests were introduced incrementally, debugging might have been shorter.

The learning goals of exploring the inner mechanisms of a VM were met. Looking deeper into the LC-3 specification forced us, as a team, to gain additional knowledge of how VMs worked.

## Resources

---

Web simulator for LC-3: [http://wchargin.github.io/lc3web/](http://wchargin.github.io/lc3web/)

Auckland NZ course on LC-3: [https://www.cs.auckland.ac.nz/compsci210s2c/tutorials/tutorial.php?tutorial=4](https://www.cs.auckland.ac.nz/compsci210s2c/tutorials/tutorial.php?tutorial=4)

Really nice diagram for opcode breakdown: [https://www.cs.auckland.ac.nz/compsci210s2c/tutorials/04/InstructionSet.pdf](https://www.cs.auckland.ac.nz/compsci210s2c/tutorials/04/InstructionSet.pdf)

[https://www.cs.utexas.edu/users/fussell/courses/cs310h/lectures/Lecture_10-310h.pdf](https://www.cs.utexas.edu/users/fussell/courses/cs310h/lectures/Lecture_10-310h.pdf) — LC-3 slideshow

[http://highered.mheducation.com/sites/0072467509/student_view0/lc-3_simulator_lab_manual.html](http://highered.mheducation.com/sites/0072467509/student_view0/lc-3_simulator_lab_manual.html) — McGraw-Hill simulator + other resources written by the creators of LC-3 project

[http://highered.mheducation.com/sites/0072467509/student_view0/appendices_a__b__c__d____e.html](http://highered.mheducation.com/sites/0072467509/student_view0/appendices_a__b__c__d____e.html) — project specifications

[https://www.cs.colostate.edu/~fsieker/misc/CtoLC3.html](https://www.cs.colostate.edu/~fsieker/misc/CtoLC3.html) — C to LC3 Assembly language

[https://www.cemgokmen.com/programming/2018-10-10-compiling-C-for-LC3/](https://www.cemgokmen.com/programming/2018-10-10-compiling-C-for-LC3/) — C to LC3 Assembly

[https://felixangell.com/blogs/virtual-machine-in-c](https://felixangell.com/blogs/virtual-machine-in-c) — virtual machine in C

[https://justinmeiners.github.io/lc3-vm/](https://justinmeiners.github.io/lc3-vm/) — Write your own virtual machine

[http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/](http://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/) — Baby's First Garbage Collector

[https://david942j.blogspot.com/2018/10/note-learning-kvm-implement-your-own.html](https://david942j.blogspot.com/2018/10/note-learning-kvm-implement-your-own.html) — Learning KVM - implement your own kernel

[https://blog.holbertonschool.com/hack-virtual-memory-stack-registers-assembly-code/](https://blog.holbertonschool.com/hack-virtual-memory-stack-registers-assembly-code/) — Hack the virtual memory: the stack, registers and assembly code

[https://github.com/rby90/Project-Based-Tutorials-in-C](https://github.com/rby90/Project-Based-Tutorials-in-C) — Project Based Tutorials in C

## Useful Links

---

Trello: [https://trello.com/b/s83l2eWJ/softsys-garbagecollectors](https://trello.com/b/s83l2eWJ/softsys-garbagecollectors)

Github: [https://github.com/caitlincoffey/SoftSysGarbageEaters](https://github.com/caitlincoffey/SoftSysGarbageEaters)
