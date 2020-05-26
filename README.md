# MIPS-Processor-and-Cache-Simulator

Designed a MIPS Processor Simulator using C++ and Object-Oriented Programming on Linux with GCC Compiler
Created a complex Parent cache class and implemented a L1 and L2 cache simulator using inheritance.

Implemented an instruction-level simulator for a single cycle MIPS processor in C++ step by step. The simulator supports a subset of the MIPS instruction set and can model the execution of each instruction.

The MIPS program is provided to the simulator as a text file “imem.txt” file which is used to initialize the Instruction Memory. Each line of the file corresponds to a Byte stored in the Instruction Memory in binary format, with the first line at address 0, the next line at address 1 and so on. Four contiguous lines correspond to a whole instruction. Note that the words stored in memory are in “Big-Endian” format, meaning that the most significant byte is stored first.

We have defined a “halt” instruction as 32’b1 (0xFFFFFFFF) which is the last instruction in every “imem.txt” file. As the name suggests, when this instruction is fetched, the simulation is terminated. We will provide a sample “imem.txt” file containing a MIPS program. You are encouraged to generate other “imem.txt” files to test your simulator.

The Data Memory is initialized using the “dmem.txt” file. The format of the stored words is the same as the Instruction Memory. As with the instruction memory, the data memory addresses also begin at 0 and increment by one in each line.


Classes
We have defined four C++ classes that each implement one of the four major blocks in a single cycle MIPS, namely RF (to implement the register file), ALU (to implement the ALU), INSMem (to implement instruction memory), and DataMem (to implement data memory).


1.RF class: contains 32 32-bit registers defined as a private member. Remember that register $0 isalways 0. 

2.ALU class: implements the ALU.Implemented ALUOperation() member functionthat performs the appropriate operation on two 32 bit operands based on ALUOP. 


3.INSMem class: a Byte addressable memory that contains instructions. The constructor InsMem()initializes the contents of instruction memory from the file imem.txt (this has been done for you). Implemented the member function ReadMemory() that provides read access toinstruction memory. An access to the instruction memory class returns 4 bytes of data; i.e., thebyte pointed to by the address and the three subsequent bytes.

4.DataMem class: is similar to the instruction memory, except that it provides both read and writeaccess.


Main Function
The main function defines a 32 bit program counter (PC) that is initialized to zero. The MIPS simulation routine is carried out within a while loop. In each iteration of the while loop, you will fetch one instruction from the instruction memory, and based on the instruction, make calls to the register file, ALU and data memory classes (made two calls to the register file class, once to read and a second time to write back). Finally updated the PC so as to fetch the next instruction. When the halt instruction is fetched, break out of the while loop and terminate the simulation.

Made sure that the architectural state is updated correctly after execution of each instruction. The architectural state consists of the Program Counter (PC), the Register File (RF) and the Data Memory (DataMem). We will check the correctness of the architectural state after each instruction.

Specifically, the OutputRF() function is called at the end of each iteration of the while loop, and will add the new state of the Register File to “RFresult.txt”. Therefore, at the end of the program execution “RFresult.txt” contains all the intermediate states of the Register File. Once the program terminates, the OutputDataMem() function will write the final state of the Data Memory to “dmem.txt”.
