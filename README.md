# Basic C++ Virtual Machine

## Description
This is a basic virtual machine that has its own instruction set. It is able to execute a program that is written in the custom assembly language.

## How to use
building: 
``` make all ```

running the code:
``` make run ```

cleaning the build:
``` make clean ```

## Project structure
- **include**: Contains the header files of the virtual machine and the assembler.
- **src**: Contains the source code of the virtual machine and the assembler.
    - **main.cpp**: The main file of the virtual machine.
        - The main function first loads the instructions from the shared libraries.
        - The code.asm file is compiled into the binary code.
        - The binary code is executed.
    - **assembler.cpp**: The assembler that is used to compile the assembly code into the binary code.
    - **computer.cpp**: The virtual machine that is used to execute the binary code.
- **instructions**: Contains the instructions of the virtual machine. That can be used in the assembly code. Each file will be compiled into a shared library, and loaded into the virtual machine.
- **code.asm**: The assembly code that will be executed by the virtual machine.
