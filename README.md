# Tiny-Language-Compiler
In this project, I was asked to write a recursive descent LL(1) parser and code generator for the tinyL language. The compiler generates RISC machine instructions. I also wrote a code optimizer that takes RISC machine instructions as input and removes redundant code. The output of the optimizer is a sequence of RISC machine instructions which produces the same results as the original input sequence but is more efficient.

## Try it Yourself

### Binaries
The included ```Make``` file can be used to compile all the needed binaries individually via: ```compile | optimize | run``` or simultaneously via ```all```

### Testing
In ``/tests`` you will find a series of sampleN.tinyL files containing well formated sample inputs for the ```compile``` command. An individual test can be ran with ```./compile tests/sampleN.tinyL```. This will result in a new file: ```tinyL.out```. You can then call the optimizer on this file with ```./optimize < tinyL.out > opt.out```. Finally you can run the optimized instructions with ```./run opt.out```

### Python
I've included a simple Python script ```script.py``` that will generate all the binaries and run the program on all the test cases. Simply run ```python script.py```

## Background

### The TinyL Language
tinyL is a simple expression language that allows assignments and basic I/O operations.

![Grammer](https://github.com/demoraeshugo/Tiny-Language-Compiler/blob/master/resources/grammer.png)

Two examples of valid tinyL programs:

%a;%b;c=&3*ab;d=+c1;$d!

%a;b=-*+1+2a58;$b!

### Target Architecture
The target architecture is a simple RISC machine with virtual registers, i.e., with an unbounded number of registers. All registers can only store integer values. A RISC architecture is a load/store architecture where arithmetic instructions operate on registers rather
than memory operands (memory addresses). This means that for each access to a memory
location, a load or store instruction has to be generated. Here is the machine instruction
set of our RISC target architecture. Rx , Ry , and Rz represent three arbitrary, but distinct
registers.

![Architecture](https://github.com/demoraeshugo/Tiny-Language-Compiler/blob/master/resources/architecture.png)

