8085 Assembler + Simulator
File name : assembler.c

running in terminal:
gcc assembler.c

1. Writing Code 
Filename: "input.txt"
    i) Write in Upper Case
    Sepearte parts with space
    Example: MOV B , A -write
            MOV B,A -Wrong

    ii) Comments : start with ; till end of line

    iii) Start code with <macroname> MACR0 $...
            End with MEND
    iv) START address in hex 4 digits
    v) end with END
 Exemplar code : 

 ;macro data
SWAP MACRO $1 $2
MOV A , $1
MOV $1 , $2
MOV $2 , A
MEND
;hello 
;code starts
START 2000
LDA 2510H
MOV B , A
LDA 2511H
MOV D , A
MVI C , 87H
SWAP B C
MOV A , D
ADD B
STA 2512H
JNC X
MVI A , 01H
STA 2513H
X : RST 5
END

2. Files created
    1. "macrotab.txt"
        <Macroname> <No. of Args> <No of memorylocations required>
    2. "symtab.txt"
        <labelname> <realative loaction to start address>
    3. "memseg.txt"
    4. "objectcode.txt"
        <address> <value>
    5. "intermediate.txt" -unformatted format

3. Other Files
    1. "datasegment.txt"
        -read if previous data is needed
        -updated as per code in end of simulation
    2. "opcodeTable"
	- has memory length requiremnet of all opcode


Other Information:
Some of the opcode functions are not defined. If the code 
encounters such an opcode, the code will terminate. 
These opcodes and there operations will be added later. The code has space 
dedicated to complete them later on. 
