#include "assemble.h"
void main(int argc, char** argv) 
{
    FILE* assp, * machp, * fopen();
    struct symbolTable* pSymTab;
    int symTabLen, j = 0, foundLabel, foundIns, noInsts, instCnt = 0;
    struct instruction* currInst;
    size_t lineSize;
    char* line, * token, * one, * two, * three;
    const char* instructions[] = { "add", "sub", "slt", "or", "nand","addi", "ori", "slti", "lui", "lw", "sw", "beq", "jalr","j", "halt" };
    const char hexTable[16] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F' };
    char lower[5];
    line = (char*)malloc(72);
    currInst = (struct instruction*)malloc(sizeof(struct instruction));
    if (argc < 3) {
        printf("***** Please run this program as follows:\n");
        printf("***** %s assprog.as machprog.m\n", argv[0]);
        printf("***** where assprog.as is your assembly program\n");
        printf("***** and machprog.m will be your machine code.\n");
        exit(1);
    }
    if ((assp=fopen(argv[1], "r")) == NULL) {
        printf("%s cannot be openned\n", argv[1]);
        exit(1);
    }
    if ((machp=fopen(argv[2], "w+")) == NULL) {
        printf("%s cannot be openned\n", argv[2]);
        exit(1);
    }
    symTabLen = findSymTabLen(assp);
    pSymTab = (struct symbolTable*)malloc(symTabLen * sizeof(struct symbolTable));
    for (int i = 0; i < symTabLen; i++)
        pSymTab[i].symbol = (char*)malloc(7);
    noInsts = fillSymTab(pSymTab, assp);
    for (int i = 0; i < symTabLen; i++)
        printf("%s %d\n", pSymTab[i].symbol, pSymTab[i].value);
    while (getline(&line, &lineSize, assp) != -1)
    {
        currInst->pc = instCnt;
        instCnt++;
        foundLabel = 0;
        foundIns = 0;
        token = strtok(line, "\t, \n\r");
        for (int i = 0; i < symTabLen; i++)
        {
            if (strcmp(pSymTab[i].symbol, token) == 0)
                token = strtok(NULL, "\t, \n\r");
        }
        strcpy(currInst->mnemonic, token);
        currInst->instType = -1;
        if (strcmp(currInst->mnemonic, ".fill") == 0)
        {
            token = strtok(NULL, "\t, \n\r");
            if (!isdigit(token[0]) && !(token[0]=='-' && isdigit(token[1])))
            {
                for (int i = 0; i < symTabLen; i++)
                {
                    if (strcmp(pSymTab[i].symbol, token) == 0)
                    {
                        currInst->intInst = pSymTab[i].value;
                        foundLabel = 1;
                    }
                }
                if (!foundLabel)
                {
                    printf("label < %s > was not found!\n", token);
                    exit(1);
                }
            }
            else currInst->intInst = atoi(token);
        }
        else
        {
            currInst->inst[0] = '0';
            for (int i = 0; i < 15; i++)
            {
                if (strcmp(currInst->mnemonic, instructions[i]) == 0)
                {
                    foundIns = 1;
                    if (i < 5) /*R format*/
                    {
                        currInst->instType = RTYPE;
                        currInst->inst[1] = hexTable[i];
                        currInst->rd = atoi(strtok(NULL, "\t, \n\r"));
                        currInst->rs = atoi(strtok(NULL, "\t, \n\r"));
                        currInst->rt = atoi(strtok(NULL, "\t, \n\r"));
                        currInst->inst[2] = hexTable[currInst->rs];
                        currInst->inst[3] = hexTable[currInst->rt];
                        currInst->inst[4] = hexTable[currInst->rd];
                        currInst->inst[5] = '0';
                        currInst->inst[6] = '0';
                        currInst->inst[7] = '0';
                        break;
                    }
                    else if (i < 13) /*I format*/
                    {
                        currInst->instType = ITYPE;
                        currInst->inst[1] = hexTable[i];
                        currInst->rt = atoi(strtok(NULL, "\t, \n\r"));
                        if (i == 8)
                        {
                            currInst->rs = 0;
                        }
                        else currInst->rs = atoi(strtok(NULL, "\t, \n\r"));
                        if (i == 12)
                        {
                            currInst->imm = 0;
                        }
                        else
                        {
                            token = strtok(NULL, "\t, \n\r");
                            if (!isdigit(token[0]) && !(token[0] == '-' && isdigit(token[1])))
                            {
                                for (int i = 0; i < symTabLen; i++)
                                {
                                    if (strcmp(pSymTab[i].symbol, token) == 0)
                                    {
                                        currInst->imm = pSymTab[i].value;
                                        foundLabel = 1;
                                    }
                                }
                                if (!foundLabel)
                                {
                                    printf("label < %s > was not found!\n", token);
                                    exit(1);
                                }
                            }
                            else currInst->imm = atoi(token);
                            if (currInst->imm > 65536)
                            {
                                printf("Immediate value in line %d is too large", instCnt);
                                exit(1);
                            }
                        }
                        if (strcmp(currInst->mnemonic,"beq")==0)
                        {
                            currInst->imm = currInst->imm - currInst->pc - 1;
                            currInst->inst[3] = hexTable[currInst->rs];
                            currInst->inst[2] = hexTable[currInst->rt];
                        }
                        else
                        {
                            currInst->inst[2] = hexTable[currInst->rs];
                            currInst->inst[3] = hexTable[currInst->rt];
                        }
                        int2hex16(&currInst->inst[4], currInst->imm);
                        break;
                    }
                    else /*J format*/
                    {
                        currInst->instType = JTYPE;
                        currInst->inst[1] = hexTable[i];
                        currInst->inst[2] = '0';
                        currInst->inst[3] = '0';
                        if (strcmp(currInst->mnemonic, "halt") == 0)
                            currInst->imm = 0;
                        else {
                            token = strtok(NULL, "\t, \n\r");
                            if (!isdigit(token[0]) && !(token[0] == '-' && isdigit(token[1])))
                            {
                                for (int i = 0; i < symTabLen; i++)
                                {
                                    if (strcmp(pSymTab[i].symbol, token) == 0)
                                    {
                                        currInst->imm = pSymTab[i].value;
                                        foundLabel = 1;
                                    }
                                }
                                if (!foundLabel)
                                {
                                    printf("label < %s > was not found!\n", token);
                                    exit(1);
                                }
                            }
                            else currInst->imm = atoi(token);
                            if (currInst->imm > 65536)
                            {
                                printf("Immediate value in line %d is too large", instCnt);
                                exit(1);
                            }
                        }
                        int2hex16(&currInst->inst[4], currInst->imm);
                        break;
                    }
                }
            }
            if (!foundIns)
            {
                printf("Invalid instruction < %s >\n", token);
                exit(1);
            }
            currInst->intInst = hex2int(currInst->inst);
        }
        printf("(address %d):\t%d\n", currInst->pc, currInst->intInst);
        fprintf(machp, "%d\n", currInst->intInst);
    }
    fclose(assp);
    fclose(machp);
}
int findSymTabLen(FILE* inputFile)
{
    int count = 0;
    size_t lineSize;
    char* line;
    line = (char*)malloc(72);
    while (getline(&line, &lineSize, inputFile) != -1)
    {
        if ((line[0] == ' ' || (line[0] == '\t')));
        else count++;
    }
    rewind(inputFile);
    free(line);
    return count;
}
int fillSymTab(struct symbolTable* symT, FILE* inputFile)
{
    int lineNo = 0, i = 0;
    size_t lineSize;
    char* line, * token;
    line = (char*)malloc(72);
    while (getline(&line, &lineSize, inputFile) != -1)
    {
        if ((line[0] == ' ') || (line[0] == '\t'));
        else
        {
            token = strtok(line, "\t, \n\r");
            for (int j = 0; j < i; j++)
            {
                if (strcmp(symT[j].symbol, token) == 0)
                {
                    printf("label < %s > is duplicate!\n", token);
                    exit(1);
                }
            }
            strcpy(symT[i].symbol, token);
            symT[i].value = lineNo;
            i++;
        }
        lineNo++;
    }
    rewind(inputFile);
    free(line);
    return lineNo;
}
int hex2int(char* hex)
{
    int result = 0;
    while ((*hex) != '\0')
    {
        if (('0' <= (*hex)) && ((*hex) <= '9'))
            result = result * 16 + (*hex) - '0';
        else if (('a' <= (*hex)) && ((*hex) <= 'f'))
            result = result * 16 + (*hex) - 'a' + 10;
        else if (('A' <= (*hex)) && ((*hex) <= 'F'))
            result = result * 16 + (*hex) - 'A' + 10;
        hex++;
    }
    return result;
}
void int2hex16(char* lower, int a)
{
    sprintf(lower, "%X", a);
    if (a < 0x10)
    {
        lower[4] = '\0';
        lower[3] = lower[0];
        lower[2] = '0';
        lower[1] = '0';
        lower[0] = '0';
    }
    else if (a < 0x100)
    {
        lower[4] = '\0';
        lower[3] = lower[1];
        lower[2] = lower[0];
        lower[1] = '0';
        lower[0] = '0';
    }
    else if (a < 0x1000)
    {
        lower[4] = '\0';
        lower[3] = lower[2];
        lower[2] = lower[1];
        lower[1] = lower[0];
        lower[0] = '0';
    }
}
