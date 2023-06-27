/* Assembler code fragment for LC-2K */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);

/* Struct for lable of assembler instructions
 * Implement list of lables with linked list.
 */
typedef struct LABLE* LableList;
typedef struct LABLE* Position;

struct LABLE {
	char symbol[7];
	char instruction[7];
	int value;
	int address;
	Position next;
};

LableList makeEmptyList();
Position addLabel(LableList, char[], char[], int, int);
Position searchLabel(LableList, char *);
void freeLableList(LableList);

int formatLabels(FILE *, LableList);
int checkOpcode(char *);
void checkRegister(char *);

/* Format instructions */
int formatRtype(char *, char *, char *, char *);
int formatItype(char *, char *, char *, char *, int, LableList);
int formatJtype(char *, char *, char *);
int formatOtype(char *);


int main(int argc, char *argv[]) 
{
	char *inFileString, *outFileString;
	FILE *inFilePtr, *outFilePtr;
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
			 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

	if (argc != 3) {
		printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
				argv[0]);
		exit(1);
	}

	inFileString = argv[1];
	outFileString = argv[2];

	inFilePtr = fopen(inFileString, "r");
	if (inFilePtr == NULL) {
		printf("error in opening %s\n", inFileString);
		exit(1);
	}
	outFilePtr = fopen(outFileString, "w");
	if (outFilePtr == NULL) {
		printf("error in opening %s\n", outFileString);
		exit(1);
	}

	/* TODO: Phase-1 label calculation */
	Position header = 0;
	header = makeEmptyList();
	formatLabels(inFilePtr, header);

	/* TODO: Phase-2 generate machine codes to outfile */
	int address = 0;
	Position node = NULL;
	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
		int type = checkOpcode(opcode);
		int pc = address + 1;
		int mInst = 0;

		switch (type) {
		case 1:
			mInst = formatRtype(opcode, arg0, arg1, arg2);
			break;
		case 2:
			mInst = formatItype(opcode, arg0, arg1, arg2, pc, header);
			break;
		case 3:
			mInst = formatJtype(opcode, arg0, arg1);
			break;
		case 4:
			mInst = formatOtype(opcode);
			break;
		case 5:
			node = searchLabel(header, label);
			mInst = node->value;
			break;

		default:
			/* Error handling when type == 0 (Unrecognized opcodes) */
			fprintf(stderr, "ERROR : Unrecognized opcodes\n");
			exit(1);
		}

		fprintf(outFilePtr, "%d\n", mInst);
		++address;
	}
	/* Delete Lable List */
	freeLableList(header);
	free(header);

	/* this is how to rewind the file ptr so that you start reading from the
		 beginning of the file */
	//rewind(inFilePtr);

	/* Assembler exits without detecting any errors. */
	if (inFilePtr) {
		fclose(inFilePtr);
	}
	if (outFilePtr) {
		fclose(outFilePtr);
	}
	exit(0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
		char *arg1, char *arg2)
{
	char line[MAXLINELENGTH];
	char *ptr = line;

	/* delete prior values */
	label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

	/* read the line from the assembly-language file */
	if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL) {
		/* reached end of file */
		return(0);
	}

	/* check for line too long (by looking for a \n) */
	if (strchr(line, '\n') == NULL) {
		/* line too long */
		printf("error: line too long\n");
		exit(1);
	}

	/* is there a label? */
	ptr = line;
	if (sscanf(ptr, "%[^\t\n\r ]", label)) {
		/* successfully read label; advance pointer over the label */
		ptr += strlen(label);
	}

	/*
	 * Parse the rest of the line.  Would be nice to have real regular
	 * expressions, but scanf will suffice.
	 */
	sscanf(ptr, "%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]%*[\t\n\r ]%"
			"[^\t\n\r ]%*[\t\n\r ]%[^\t\n\r ]", opcode, arg0, arg1, arg2);
	return(1);
}

int isNumber(char *string)
{
	/* return 1 if string is a number */
	int i;
	return((sscanf(string, "%d", &i)) == 1);
}

/* Make empty list of lables. */
LableList makeEmptyList(){
	LableList header = (LableList)malloc(sizeof(struct LABLE));
	strcpy(header->symbol, "header");
	strcpy(header->instruction, "");
	header->address = -999;
	header->value = -999;
	header->next = NULL;
	return header;
}

/* Add new lable to the end of label list, then set its value with parameters. */
Position addLabel(LableList header, char label[], char instruction[], int value, int address){
	/* Find the last node */
	Position curLabel = header;
	while(curLabel->next){
		curLabel = curLabel->next;
	}

	/* Make new node */
	Position newLable = (Position)malloc(sizeof(struct LABLE));
	curLabel->next = newLable;

	strcpy(newLable->symbol, label);
	strcpy(newLable->instruction, instruction);
	newLable->value = value;
	newLable->address = address;
	newLable->next = NULL;

	return newLable;
}

/* Search the label with symbol in a label list. */
Position searchLabel(LableList header, char *symbol)
{
	Position result = header;

	while (result){
		if (!strcmp(result->symbol, symbol)){
			return result;
		}
		result = result->next;
	}

	return result;
}

/* Free lable list */
void freeLableList(Position header)
{
	Position nextLabel = NULL, tmp = NULL;
	nextLabel = header->next;
	header->next = NULL;

	while (nextLabel){
		tmp = nextLabel->next;
		free(nextLabel);
		nextLabel = tmp;
	}
}

/* Print Label info in the lable list
 * For debugging
 */
void printList(Position header)
{
	Position curLable = header;

	while (curLable){
		printf("symbol : %s\tinst : %s\taddress : %d\n", curLable->symbol, curLable->instruction, curLable->address);
		curLable = curLable->next;
	}
	printf("\n");
}

/* Format Labels
 * If opcode of the line is ".fill", set symbol's value with numeric value.
 */
int formatLabels(FILE *inFilePtr, LableList header)
{
	char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH], 
			 arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];
	
	int address = 0;
	while (readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2)){
		if (!strcmp(label, "")){
			++address;
			continue;
		}

		/* Check the label with wrong length. */
		int labelLen = strlen(label);
		if (labelLen < 0 || labelLen > 6){
			fprintf(stderr, "ERROR : label with wrong length\n");
			exit(1);
		}

		/* Check if the label starts with number */
		if (48 <= label[0] && label[0] <= 57){
			fprintf(stderr, "ERROR : label must start with a letter\n");
			exit(1);
		}

		/* Check Duplicate definition of lablels */
		if (searchLabel(header, label)){
			fprintf(stderr, "ERROR : Duplicate definition of labels\n");
			exit(1);
		}

		/* If opcode is ".fill", set symbols value as value */
		if (!strcmp(opcode, ".fill")){
			if (isNumber(arg0)){
				/* Error Checking */
				long long num = atoll(arg0);
				if (num < -2147483648 || num > 2147483647){
					fprintf(stderr, "ERROR : numeric value out of bound\n");
					exit(1);
				}
				
				addLabel(header, label, "", atoi(arg0), address);
			}
			else{
				Position destLabel = searchLabel(header, arg0);

				if (destLabel == NULL){
					fprintf(stderr, "ERROR : Use of undefined lables\n");
					exit(1);
				}

				addLabel(header, label, arg0, destLabel->address, address);
			}
		}
		/* Save symbols address to lable list when opcode != ".fill". */
		else{
			addLabel(header, label, "", 0, address);
		}

		/* Increment address */
		++address;
	}
	
	rewind(inFilePtr);

	return(0);
}

/* Check opcode of the line
 *
 * Return values : 
 *	R-type : 1		I-type : 2
 *	J-type : 3		O-type : 4
 *	.fill  : 5		Unrecognized opcodes : 0
 */
int checkOpcode(char* opcode)
{
	if (!strcmp(opcode, "add") || !strcmp(opcode, "nor")){
		return 1;
	}
	else if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq")){
		return 2;
	}
	else if (!strcmp(opcode, "jalr")){
		return 3;
	}
	else if (!strcmp(opcode, "halt") || !strcmp(opcode, "noop")){
		return 4;
	}
	else if (!strcmp(opcode, ".fill")){
		return 5;
	}

	return(0);
}

/* Check Register
 * exit if there is an invalid register (out of range)
 */
void checkRegister(char *reg)
{
	if (!isNumber(reg)){
		fprintf(stderr, "ERROR : Non-integer register arguments\n");
		exit(1);
	}

	int intReg = atoi(reg);
	if (intReg < 0 || 7 < intReg){
		fprintf(stderr, "ERROR : Registers outside the range [0, 7]\n");
		exit(1);
	}
}

/* Format R-type instructions 
 * R-type instructions (add, nor):
 *  bits 24-22: opcode
 *	bits 21-19: reg A
 *	bits 18-16: reg B
 *	bits 15-3: unused (should all be 0)
 *	bits 2-0: destReg
 */
int formatRtype(char *opcode, char *regA, char* regB, char* destReg)
{
	int inst = 0;

	checkRegister(regA);
	checkRegister(regB);

	/* Set an operation */
	if (!strcmp(opcode, "add")){
		inst = (0 << 22);
	}
	else if(!strcmp(opcode, "nor")){
		inst = (1 << 22);
	}

	/* Set registers */
	inst = inst | (atoi(regA) << 19);
	inst = inst | (atoi(regB) << 16);

	/* Set destReg */
	inst = inst | atoi(destReg);

	return inst;
}

/* Format I-type instructions
 * I-type instructions (lw, sw, beq):
 *	bits 24-22: opcode
 *	bits 21-19: reg A
 *	bits 18-16: reg B
 *	bits 15-0: offsetField (a 16-bit value with a range of -32768 to 32767)
 */
int formatItype(char *opcode, char *regA, char* regB, char *offsetField, int pc, LableList header)
{
	int inst = 0;

	checkRegister(regA);
	checkRegister(regB);
	
	/* Set an operation */
	if (!strcmp(opcode, "lw")){
		inst = (2 << 22);
	}
	else if (!strcmp(opcode, "sw")){
		inst = (3 << 22);
	}
	else if (!strcmp(opcode, "beq")){
		inst = (4 << 22);
	}

	/* Set registers */
	inst = inst | (atoi(regA) << 19);
	inst = inst | (atoi(regB) << 16);

	/* If offset is numeric value */
	int address = 0;
	if (isNumber(offsetField)){
		address = atoi(offsetField);

		if (address < -32768 || 32767 < address){
			fprintf(stderr, "ERROR : offsetField that don't fit in 16bits\n");
			exit(1);
		}
	}
	/* If offset is symbolic value */
	else{
		Position label = searchLabel(header, offsetField);

		if (label == NULL){
			fprintf(stderr, "ERROR : Use of undefined lables\n");
			exit(1);
		}

		address = label->address;
	}

	if (!strcmp(opcode, "beq") && !isNumber(offsetField)){
		address -= pc;
	}

	inst = inst | (0xFFFF & address);

	return inst;
}

/* Format J-type instructions
 * J-type instructions (jalr):
 *	bits 24-22: opcode
 *	bits 21-19: reg A
 *	bits 18-16: reg B
 *	bits 15-0: unused (should all be 0)
 */
int formatJtype(char *opcode, char *regA, char *regB)
{
	int inst = 0;

	checkRegister(regA);
	checkRegister(regB);

	/* Set an operation */
	if (!strcmp(opcode, "jalr")){
		inst = (5 << 22);
	}

	/* Set registers */
	inst = inst | (atoi(regA) << 19);
	inst = inst | (atoi(regB) << 16);

	return inst;
}

/* Format O-type instructions
 * O-type instructions (halt, noop):
 *	bits 24-22: opcode
 *	bits 21-0: unused (should all be 0)
 */
int formatOtype(char* opcode)
{
	int inst = 0;

	/* Set an operation */
	if (!strcmp(opcode, "halt")){
		inst = (6 << 22);
	}
	else if (!strcmp(opcode, "noop")){
		inst = (7 << 22);
	}

	return inst;
}