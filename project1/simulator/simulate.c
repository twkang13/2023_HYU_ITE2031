/* LC-2K Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000 
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

void readAndParse(int *, int *, int *, int *, stateType *);

/* Simulators */
void simulate_add(int, int, int, stateType *);
void simulate_nor(int, int, int, stateType *);
void simulate_lw(int, int, int, stateType *);
void simulate_sw(int, int, int, stateType *);
void simulate_beq(int, int, int, stateType *);
void simulate_jalr(int, int, stateType *);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {

        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

	/* TODO: */
    state.pc = 0;
    int execCnt = 0;
    printState(&state);

    while(1){
        int opcode, arg0, arg1, arg2;
        readAndParse(&opcode, &arg0, &arg1, &arg2, &state);
        ++state.pc;
        ++execCnt;

        /* OPCODE
         * R-type : add  - 0     nor  - 1
         * I-type : lw   - 2     sw   - 3     beq  - 4
         * J-type : jalr - 5
         * O-type : halt - 6     noop - 7
         */

        /* R-type instructions */
        if (opcode == 0){
            simulate_add(arg0, arg1, arg2, &state);
        }
        else if (opcode == 1){
            simulate_nor(arg0, arg1, arg2, &state);
        }
        /* I-type instructions */
        else if (opcode == 2){
            simulate_lw(arg0, arg1, convertNum(arg2), &state);
        }
        else if (opcode == 3){
            simulate_sw(arg0, arg1, convertNum(arg2), &state);
        }
        else if (opcode == 4){
            simulate_beq(arg0, arg1, convertNum(arg2), &state);
        }
        /* J-type instructions */
        else if (opcode == 5){
            simulate_jalr(arg0, arg1, &state);
        }
        /* halt : Increment the PC (as with all instructions), then halt the machine (let the
         *        simulator notice that the machine halted).
         */
        else if (opcode == 6){
            break;
        }
        /* noop : Do Nothing */
        else if (opcode == 7){}
        /* Error */
        else{
            printf("ERROR : operation not defined\n");
            exit(1);
        }

        printState(&state);

        /* Loop breaking condition */
        if (state.pc > state.numMemory){
            break;
        }
    }

    printf("machine halted\n");
    printf("total of %d instructions executed\n", execCnt);
    printf("final state of machine:\n");
    printState(&state);

    exit(0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}

int convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Linux integer */
	if (num & (1 << 15)) {
		num -= (1 << 16);
	}
	return (num);
}

/* Read and Parse the instruction */
void readAndParse(int *opcode, int *arg0, int *arg1, int *arg2, stateType *state)
{
    int instruction = state->mem[state->pc];
    
    *opcode = (instruction >> 22) & 0x7;
    *arg0 = (instruction >> 19) & 0x7;
    *arg1 = (instruction >> 16) & 0x7;
    *arg2 = instruction & 0xFFFF;
}

/* add(0)  : Add contents of regA with contents of regB, store results in destReg. */
void simulate_add(int regA, int regB, int destReg, stateType *state)
{
    state->reg[destReg] = state->reg[regA] + state->reg[regB];
}

/* nor(1)  : Nor contents of regA with contents of regB, store results in destReg.
 *           This is a bitwise nor; each bit is treated independently.
 */
void simulate_nor(int regA, int regB, int destReg, stateType *state)
{
    state->reg[destReg] = ~(state->reg[regA] | state->reg[regB]);
}

/* lw(2)   : Load regB from memory. Memory address is formed by adding
 *           offsetField with the contents of regA.
 */
void simulate_lw(int regA, int regB, int offsetField, stateType *state)
{
    state->reg[regB] = state->mem[state->reg[regA] + offsetField];
}

/* sw(3)   : Store regB into memory. Memory address is formed by adding
 *           offsetField with the contents of regA.
 */
void simulate_sw(int regA, int regB, int offsetField, stateType *state)
{
    state->mem[state->reg[regA] + offsetField] = state->reg[regB];
}

/* beq(4)  : If the contents of regA and regB are the same, then branch to the address
 *           PC+1+offsetField, where PC is the address of this beq instruction.
 */
void simulate_beq(int regA, int regB, int offsetField, stateType *state)
{
    if (state->reg[regA] - state->reg[regB] == 0){
        state->pc += offsetField;
    }
}

/* jalr(5) : First store PC+1 into regB, where PC is the address of this jalr instruction.
 *           Then branch to the address contained in regA. Note that this implies if regA
 *           and regB refer to the same register, the net effect will be jumping to PC+1.
 */
void simulate_jalr(int regA, int regB, stateType *state)
{
    state->reg[regB] = state->pc;
    state->pc = state->reg[regA];
}