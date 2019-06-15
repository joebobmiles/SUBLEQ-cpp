An Overview of SUBLEQ
=====================

SUBLEQ is a type of instruction set architecture (ISA) known as [OISC]()
(one-instruction set computer). Unlike other types of common ISAs, which can
have somewhere between tens to thousands of different instructions that a CPU
using that ISA can understand, SUBLEQ has only one instruction. This
instruction is the titular SUBLEQ operation, SUBLEQ being short for
"**SU**btract and **B**ranch if **L**ess than or **EQ**ual to zero".

The SUBLEQ instruction takes 3 addresses as it's operands. The first two
addresses are used to fetch the values to subtract. The result of the
subtraction is stored in the second address and then compared to zero. If the
value is less than or equal to zero, the processor branches to the third
address. If the value is greater than zero, the processor continues on without
branching. In C, SUBLEQ can be represented as:

```C
int *A = memory[PC++];
int *B = memory[PC++];
int *C = memory[PC++];

*B = *A - *B;

if (*B <= 0) PC = *C;
```

## The Advantages of One Instruction

In contrast to more sophisticated architectures like x86 (a
[_CISC_](https://en.wikipedia.org/wiki/Complex_instruction_set_computer)
architecture) or ARM (a
[_RISC_](https://en.wikipedia.org/wiki/Reduced_instruction_set_computer)
architecture), SUBLEQ has one key advantage: not having opcodes. In other ISAs,
a CPU has to have two areas of memory, one to store instructions and another to
store data (see [_modified Harvard
architecture_](https://en.wikipedia.org/wiki/Modified_Harvard_architecture)).
From the _instruction cache_ the CPU retrieves the opcode for the next
operation it must perform.  This opcode is given the CPU's control unit, which
configures the other parts of the CPU to perform the operation.

The downside of that model is hardware complexity and hardware size. For x86 or
ARM CPUs to operate, they need an ALU that can perform all the operations in
their instruction set, as well as a control unit sophisticated enough to
understand them. This makes both components, and the hardware that supports
them rather complex, which is mirrored in their size. The more complex the
operation, the more transistors are needed to perform it. And more transistors
means more real estate is needed on the die to build the CPU.

SUBLEQ, on the other hand, has no opcodes it needs to understand. Since the CPU
can only perform one operation, all it needs are the operands. This means that
the control unit can be tossed out of the CPU, and the ALU can be reduced to a
SUBLEQ implementation in hardware. This doesn't simplify many other components
of the CPU, such as the Memory Management Unit (MMU) or parts required to
communicate with peripheral devices. But it does mean that things like the
instruction cache can be removed, as well as the register file, since a SUBLEQ
machine can function perfectly fine in a contiguous region of memory.  (Despite
this, it might be more desirable to have a register file so as to avoid
incurring the access penalty that comes from directly operating on CPU cache or
RAM.) What this translates to is a CPU that is much, _much_ smaller than a CPU
with a more traditional ISA.

And with this smaller size comes several other benefits. The first is that
since a SUBLEQ machine is composed of less transistors than a more complex CPU,
it requires less power to operate. This can be ideal for embedded general
computing, where power and size are both constrained. An additional benefit is
speed. Since a SUBLEQ machine is composed of less transistors, the circuit is
smaller, which reduces propagation delay, allowing the CPU to operate faster.
On top of this, the smaller die size of a single SUBLEQ machine allows more of
them to be tiled in a multi-core processor of similar size to current gen
ISA processors.

Another, but harder to measure, benefit is the simplicity of SUBLEQ machines.
[As some would like to argue](http://gerigeri.uw.hu/DawnOS/index.html), the
simplicity of a SUBLEQ CPU makes it cheaper to produce as well as easier to
understand and develop. This is argued to make SUBLEQ machines (or other OISC
ISAs) ultimately more democratic devices than todays current consumer CPU.

## The Disadvantages to One Instruction

It would not be an overview if the disadvantages of SUBLEQ were not discussed.
The most egregious of these is time and space efficiency. Because SUBLEQ has
only one instruction, the only operation it can do in a single clock cycle is
the SUBLEQ operation itself. All other operations must require several clock
cycles to be accomplished. And while some operations, like addition, negation,
inversion, and logical operations, can be performed with trivial combinations
of SUBLEQ commands, others (such as multiplication and division) have a much
steeper complexity. This means that despite the speed increase a SUBLEQ machine
gains by being smaller than a normal CPU, it may still take as long, if not
longer, for it to complete equivalent tasks.

And the increase in instruction volume directly correlates to the amount of
memory required to encode a program. In other architectures, opcodes allow
complex operations to be encoded in hardware. While it can be argued that this
simply pushes program complexity into the hardware, having the operation
performed in hardware results in the program needing to store less
instructions, making it smaller. In a SUBLEQ machine, all the operational
complexity is pushed into the program, potentially making it much larger than
an equivalent program in a more complex instruction set. (It is hard to tell if
the absence of opcodes offsets the program size incurred by having more SUBLEQ
instructions.)
