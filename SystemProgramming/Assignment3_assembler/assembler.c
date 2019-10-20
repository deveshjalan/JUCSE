#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<assert.h>

//global variables

//create variables for registers and flags
int  A,B,C,D,E,H,L,M,SP;
//av 1,2,3,4,5,6,7,8
int CY, AUCY, P, Z, S;


//declare flags

int memory[1024];
int fixedmemory[512];
void initialise_simulator()
{
    A=0;
    B=0;
    C=0;
    D=0;
    E=0;
    H=0;
    L=0;
    M=0;
    SP=0;
    CY=0;
    AUCY=0;
    P=0;
    Z=0;
    S=0;

    for(int i=0;i<512;i++)
    {
        fixedmemory[i]=0;
    }
    
}

//to create table -- aritra and mamata
typedef struct {
    char a[20];
    int size;
} opcodestruct;

opcodestruct oplentab[80];

char f[200][20];
char f2[300][20];
typedef struct {
    char a[20];
    int nargs;
    int starti;
    int endi;
    int progcount;
} macrodata;

typedef struct {
    char a[20];
    int relativeposition;
} labeldata;

macrodata macrotab[5];
labeldata symtab[10];



//functions
void setzero(int k)
{
    if(k==0) Z=1;
    else Z=0; 
}
void setparitysignA()
{
    int sum=0;
    int tempA=A;
    int temp;
    for (int i=128;i>=1;i=i/2)
    {
        temp=tempA/i;
        sum+=temp;
        tempA%=i;
        if(i==128)
        {
            S=temp;
        }
    }
    if(sum%2==0)
    {
        P=1;
    }
    else P=0;
    
}
void addopcodelength(char a[],int *pc)
{
    int i;
    for(i=0;i<80;i++)
    {
        if(!strcmp(oplentab[i].a,a))
        {
            *pc=*pc+oplentab[i].size;
        }
    }
}
void readoplentab()
{
   //anwesha
    FILE *file = fopen("./opcodeTable.txt", "r");
    assert(file != NULL);
    int j=0;
    while (j<80)
    {
        fscanf(file,"%s",oplentab[j].a);
        fscanf(file,"%d",&oplentab[j].size);
        j++;
        
    }
    fclose(file);

    //(void)fclose(file);
};
int getdecvalue(char a[], int len)
{
    int decsum=0;
    for(int i=0;i<len;i++)
    {
        char x;
        x=a[i];
        decsum*=16;
        switch(x)
        {
            case '1': decsum+=1;
                        break;
            case '2': decsum+=2;
                        break;
            case '3': decsum+=3;
                        break;
            case '4': decsum+=4;
                        break;
            case '5': decsum+=5;
                        break;
            case '6': decsum+=6;
                        break;
            case '7': decsum+=7;
                        break;
            case '8': decsum+=8;
                        break;
            case '9': decsum+=9;
                        break;
            case 'A': decsum+=10;
                        break;
            case 'B': decsum+=11;
                        break;
            case 'C': decsum+=12;
                        break;
            case 'D': decsum+=13;
                        break;
            case 'E': decsum+=14;
                        break;
            case 'F': decsum+=15;
                        break;
            case '0': decsum+=0;
                        break;
        }
    }
    return decsum;
}
int getopcodeindex(char a[])
{
    int i;
    for(i=1;i<81;i++)
    {
        if(!strcmp(oplentab[i-1].a,a))
        {
            return i;
        }
    }
    return -1;
}
int getregistersegment(char a[])
{
    if(strcmp(a,"A")==0)      return 1;
    else if(strcmp(a,"B")==0) return 2;
    else if(strcmp(a,"C")==0) return 3;
    else if(strcmp(a,"D")==0) return 4;
    else if(strcmp(a,"E")==0) return 5;
    else if(strcmp(a,"H")==0) return 6;
    else if(strcmp(a,"L")==0) return 7;
    else if(strcmp(a,"M")==0) return 8;
    else if(strcmp(a,"SP")==0) return 9;
    else
    {
        printf(" ERROR IN CODE");
        return 0;
    };
}
int searchsymtabindex(char a[],int tabsize)
{
    for(int i=0;i<tabsize;i++)
    {
        if(strcmp(symtab[i].a,a)==0)
            return i;
    }
    return -1;
}
void displayheader()
{
    printf("##OPCode  _A _B _C _D _E _H _L : CY _P _Z _S AU\n");
}
void displayregsnflag()
{
    printf("%2X %2X %2X %2X %2X %2X %2X :  %d  %d  %d  %d  %d\n",A,B,C,D,E,H,L,CY,P,Z,S,AUCY);
}
//main

int main()
{
//Initialising memory block
    for(int i=0;i<1024;i++)
    {
        memory[i]=0;
    }
  
//initialising variables
    readoplentab();
    FILE *fp;
    FILE *fpinter;
    fpinter=fopen("intermediate.txt","w");
    fp=fopen("input.txt","r");
    int locctr=0;
    char a[20];
    int macroindex=0;
    int symtabindex=0;
    int startin;
    //reading all codes;
    int i=0;
    char currentline[100];
    //printf("not READ file");
    do
    {
		fscanf(fp,"%s",f[i]);
        if(f[i][0]==';')
        {
            //
            printf("Deleted%s\n",f[i]);
            fgets(currentline, sizeof(currentline), fp);
            fscanf(fp,"%s",f[i]);
        }
		i++;  
	}while(strcmp(f[i-1],"END")!=0);
    //printf("READ file");
//PASS 1
    //finding macros
    i=0;
    do{
		strcpy(a,f[i]);
		if(strcmp(a,"START")==0) 
        {
            startin=i;
            locctr=0;
            break;
        }
		else if (strcmp(a,"MACRO")==0)
		{
            macrotab[macroindex].starti=i-1;
            char b[20];
            int nargs=0;
            int pc1=0;
            strcpy(b,f[i-1]);
            while(1)
            {
                i++;
                strcpy(a,f[i]);
                if(a[0]=='$')
                {
                    nargs++;
                }
                else break;
            }
            while(1)
            {
                strcpy(a,f[i]);
                if(strcmp(a,"MEND")==0)
                {
                    break;
                }
               addopcodelength(a,&pc1); //todefinethefunction
                i++;

            }
            macrotab[macroindex].nargs=nargs;
            macrotab[macroindex].endi=i;
            macrotab[macroindex].progcount=pc1;
            strcpy(macrotab[macroindex].a,b);
            macroindex++;
		
		}
        else if (strcmp(a,":")==0)
        {
            printf("Invalid Macro Deffination. a label is dtected in macro part. Not defined for the program\n");
            return 0;
        }///*/
        i++;
	}while(1);
    //*/
    //finding labels and locctr
    do
    {
        strcpy(a,f[i]);
		if(strcmp(a,"END")==0) 
        {
            fprintf(fpinter, "%s ",a);
            break;
        }
        else if (strcmp(a,":")==0)
        {
            fprintf(fpinter, "%s ",a);
            char b[20];
            strcpy(b,f[i-1]);
            //search for b being dupilicate in symtab and give an error
            //terminate
            //else
            strcpy(symtab[symtabindex].a,b);
            symtab[symtabindex].relativeposition=locctr;
            symtabindex++;
        }
        else
        {
            int temp=0;
            for(int k=0;k<macroindex;k++)
            {
                if(strcmp(macrotab[k].a,a)==0)
                {
                    printf("\nLog: Macro detected. \n");
                    locctr+=macrotab[k].progcount;
                    int temp3=macrotab[k].nargs;
                    int temp4=macrotab[k].starti+1;;
                    char args[temp3][20];
                    char substitute[temp3][20];
                    char temp7[20];
                    for(int g=0;g<temp3;g++)
                    {
                        temp4++;
                        i++;
                        strcpy(temp7,f[temp4]);
                        strcpy(a,f[i]);
                        strcpy(args[g],temp7);
                        strcpy(substitute[g],a);
                        printf("Log:MacroSubstitution %s - %s\n", args[g],substitute[g]);
                    }
                    int temp5=macrotab[k].endi;
                    for(int g=temp4+1;g<temp5;g++)
                    {
                        strcpy(temp7,f[g]);
                        if (temp7[0]=='$')
                        {
                            for(int q=0;q<temp3;q++)
                            {
                                if(strcmp(args[q],temp7)==0)
                                {
                                    fprintf(fpinter,"%s ",substitute[q]);
                                    break;
                                }
                            }
                        }
                        else
                            fprintf(fpinter,"%s ",temp7);
                    }
                    temp=1;
                    break;
                }
            }
            if(temp!=1)  
            {
                addopcodelength(a,&locctr);
                fprintf(fpinter, "%s ",a);
            } 
                
        }
        i++;
        

    }while(1);
    fclose(fp);
    fclose(fpinter);

//Printing Locctor
    printf("\nLog: LOCCTR : %d\n",locctr);
//make macrotab and symtab files
    FILE *fpsymtab;
    fpsymtab=fopen("symtab.txt","w");
    for(i=0;i<symtabindex;i++)
    {
        fprintf(fpsymtab,"%s\t%d\n",symtab[i].a,symtab[i].relativeposition);
    }
    fclose(fpsymtab);
    printf("\nLog: Symtab Created\n");
    FILE *fpmacrotab;
    fpmacrotab=fopen("macrotab.txt","w");
    for(i=0;i<macroindex;i++)
    {
        fprintf(fpsymtab,"%s\t%d\t%d\n",macrotab[i].a,macrotab[i].nargs,macrotab[i].progcount);
    }
    fclose(fpmacrotab);
    printf("\nLog: Macrotab Created\n");
//Pre-Pass 2
    FILE *frinter;
    frinter=fopen("intermediate.txt","r");
    i=0;
    do
    {
		fscanf(frinter,"%s",f2[i]);
		i++;  
	}while(strcmp(f2[i-1],"END")!=0);
//PASS 2
    printf("\nLog: Pass 2 Begins\n");
    int memoryindex=0;
    int startaddress=0;
    i=0;
    strcpy(a,f2[i]);
    if(strcmp(a,"START")==0)
    {
        i++;
        strcpy(a,f2[i]);
        startaddress=getdecvalue(a,4);
    }
    else
    {
        printf("ERROR: Error at beginning of pass2 . start did not get read. please check\n");
        return 0;
    }
//Pass 2- after start initialised 
    do
    {
        i++;
        strcpy(a,f2[i]);
        if(strcmp(a,"END")==0) 
        {
            printf("Log: Pass 2 : End of file reached.\n");
            break;
        }
        int opindex;
        int temps;
        opindex=getopcodeindex(a);
        if(opindex==-1)
        {
            i++;
            strcpy(a,f2[i]);
            if(strcmp(a,":")==0)
            {
                i++;
                strcpy(a,f2[i]);
                opindex=getopcodeindex(a);
            }
            else
            {   
                printf("ERROR: Stray mark detected in intermediate file. Pls Check\n");
                return 0;
            }
            
        }
        switch(opindex)
        {
            case 1 :{  //C - ACI 
                        memory[memoryindex++]=206;
                        i++;
                        strcpy(a,f2[i]);
                        temps=getdecvalue(a,2);
                        memory[memoryindex++]=temps;

                    break;
                }
            case 2 :{  //C - ADC
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 1 : {memory[memoryindex++]=143  ;break;}
                            case 2 : {memory[memoryindex++]=136  ;break;}
                            case 3 : {memory[memoryindex++]= 137 ;break;}
                            case 4 : {memory[memoryindex++]= 138 ;break;}
                            case 5 : {memory[memoryindex++]=139 ;break;}
                            case 6 : {memory[memoryindex++]= 140 ;break;}
                            case 7 : {memory[memoryindex++]=  141;break;}
                            case 8 : {memory[memoryindex++]=  142;break;}
                            
                        }

                    break;
                }
            case 3 :{  //C - ADD 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 1 : {memory[memoryindex++]=135  ;break;}
                            case 2 : {memory[memoryindex++]= 128 ;break;}
                            case 3 : {memory[memoryindex++]= 129 ;break;}
                            case 4 : {memory[memoryindex++]= 130 ;break;}
                            case 5 : {memory[memoryindex++]= 131 ;break;}
                            case 6 : {memory[memoryindex++]= 132 ;break;}
                            case 7 : {memory[memoryindex++]= 133 ;break;}
                            case 8 : {memory[memoryindex++]= 134 ;break;}
                            
                        }
                    break;
                }
            case 4 :{  //C - ADI 
                        memory[memoryindex++]=198;
                        i++;
                        strcpy(a,f2[i]);
                        temps=getdecvalue(a,2);
                        memory[memoryindex++]=temps;

                    break;
                }
            case 5 :{  //C - ANA
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a); 
                        switch(temps)
                        {
                            
                            case 1 : {memory[memoryindex++]= 167 ;break;}
                            case 2 : {memory[memoryindex++]=  160;break;}
                            case 3 : {memory[memoryindex++]=161  ;break;}
                            case 4 : {memory[memoryindex++]= 162 ;break;}
                            case 5 : {memory[memoryindex++]= 163 ;break;}
                            case 6 : {memory[memoryindex++]=  164;break;}
                            case 7 : {memory[memoryindex++]= 165 ;break;}
                            case 8 : {memory[memoryindex++]= 166 ;break;}
                            
                        }
                    break;
                }
            case 6 :{  //C - ANI 
                        memory[memoryindex++]=230;
                        i++;
                        strcpy(a,f2[i]);
                        temps=getdecvalue(a,2);
                        memory[memoryindex++]=temps;
                    break;
                }
            case 7 :{  //ND- CALL 

                    break;
                }
            case 8 :{  //ND- CC 

                    break;
                }
            case 9 :{  //ND- CM 

                    break;
                }
            case 10 :{ //C - CMA 
                    memory[memoryindex++]=47;
                    break;
                }
            case 11 :{ //C - CMC 
                        memory[memoryindex++]=63;
                    break;
                }
            case 12 :{ //C - CMP 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 1 : {memory[memoryindex++]= 191  ;break;}
                            case 2 : {memory[memoryindex++]= 184 ;break;}
                            case 3 : {memory[memoryindex++]= 185 ;break;}
                            case 4 : {memory[memoryindex++]= 186 ;break;}
                            case 5 : {memory[memoryindex++]= 187 ;break;}
                            case 6 : {memory[memoryindex++]= 188 ;break;}
                            case 7 : {memory[memoryindex++]= 189 ;break;}
                            case 8 : {memory[memoryindex++]= 190 ;break;}
                            
                        }
                    break;
                }
            case 13 :{ //ND- CNC 

                    break;
                }
            case 14 :{ //ND- CNZ 

                    break;
                }
            case 15 :{ //ND- CP 

                    break;
                }
            case 16 :{ //ND- CPE 

                    break;
                }
            case 17 :{ //ND- CPI 

                    break;
                }
            case 18 :{ //ND- CPO 

                    break;
                }
            case 19 :{ //ND- CZ 

                    break;
                }
            case 20 :{ //C - DAA 
                    memory[memoryindex++]=39;
                    break;
                }
            case 21 :{ //C - DAD 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 2 : {memory[memoryindex++]=9  ;break;}
                            case 4 : {memory[memoryindex++]= 25 ;break;}                      
                            case 6 : {memory[memoryindex++]= 41;break;}                         
                            case 9 : {memory[memoryindex++]=57;break;}
                        }

                    break;
                }
            case 22 :{ //C - DCR 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 1 : {memory[memoryindex++]= 61 ;break;}
                            case 2 : {memory[memoryindex++]=5  ;break;}
                            case 3 : {memory[memoryindex++]= 13;break;}
                            case 4 : {memory[memoryindex++]= 21;break;}
                            case 5 : {memory[memoryindex++]=29 ;break;}
                            case 6 : {memory[memoryindex++]= 37 ;break;}
                            case 7 : {memory[memoryindex++]=  45;break;}
                            case 8 : {memory[memoryindex++]=  53;break;}
                            
                        }
                    break;
                }
            case 23 :{ //C - DCX 
                         i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 2 : {memory[memoryindex++]=11;break;}
                            case 4 : {memory[memoryindex++]= 27 ;break;}                      
                            case 6 : {memory[memoryindex++]= 43;break;}                         
                            case 9 : {memory[memoryindex++]=59;break;}
                        }
                    break;
                }
            case 24 :{ //ND- DI 

                    break;
                }
            case 25 :{ //ND- EI 

                    break;
                }
            case 26 :{ //C - HLT 
                        memory[memoryindex++]=118;

                    break;
                }
            case 27 :{ //ND- IN 

                    break;
                }
            case 28 :{ //C - INR 
                    i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 1 : {memory[memoryindex++]= 60 ;break;}
                            case 2 : {memory[memoryindex++]=4;break;}
                            case 3 : {memory[memoryindex++]= 12;break;}
                            case 4 : {memory[memoryindex++]= 20;break;}
                            case 5 : {memory[memoryindex++]=28 ;break;}
                            case 6 : {memory[memoryindex++]= 36 ;break;}
                            case 7 : {memory[memoryindex++]=  44;break;}
                            case 8 : {memory[memoryindex++]=  52;break;}
                            
                        }
                    break;
                }
            case 29 :{ //C - INX 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 2 : {memory[memoryindex++]=2;break;}
                            case 4 : {memory[memoryindex++]= 19 ;break;}                      
                            case 6 : {memory[memoryindex++]= 35;break;}                         
                            case 9 : {memory[memoryindex++]=51;break;}
                        }
                    break;
                }
            case 30 :{ //C - JC 
                    memory[memoryindex++]=218;
                    i++;
                    strcpy(a,f2[i]);
                    int k=searchsymtabindex(a,symtabindex);
                    if(k==-1)
                    {
                        printf("Error in Jump Function. Label not found");
                    }
                    else
                    temps=symtab[k].relativeposition;
                    int targetaddress;
                    targetaddress=temps+startaddress;
                    memory[memoryindex++]=targetaddress%256;
                    memory[memoryindex++]=targetaddress/256;
                    break;
                }
            case 31 :{ //C - JM 
                    memory[memoryindex++]=250;
                    i++;
                    strcpy(a,f2[i]);
                    int k=searchsymtabindex(a,symtabindex);
                    if(k==-1)
                    {
                        printf("Error in Jump Function. Label not found");
                    }
                    else
                    temps=symtab[k].relativeposition;
                    int targetaddress;
                    targetaddress=temps+startaddress;
                    memory[memoryindex++]=targetaddress%256;
                    memory[memoryindex++]=targetaddress/256;
                    break;
                }
            case 32 :{ //C - JMP 
                        memory[memoryindex++]=195;
                    i++;
                    strcpy(a,f2[i]);
                    int k=searchsymtabindex(a,symtabindex);
                    if(k==-1)
                    {
                        printf("Error in Jump Function. Label not found");
                    }
                    else
                    temps=symtab[k].relativeposition;
                    int targetaddress;
                    targetaddress=temps+startaddress;
                    memory[memoryindex++]=targetaddress%256;
                    memory[memoryindex++]=targetaddress/256;
                    break;
                }
            case 33 :{ //C - JNC 
                        memory[memoryindex++]=210;
                    i++;
                    strcpy(a,f2[i]);
                    int k=searchsymtabindex(a,symtabindex);
                    if(k==-1)
                    {
                        printf("Error in Jump Function. Label not found");
                    }
                    else
                    temps=symtab[k].relativeposition;
                    int targetaddress;
                    targetaddress=temps+startaddress;
                    memory[memoryindex++]=targetaddress%256;
                    memory[memoryindex++]=targetaddress/256;
                    break;
                }
            case 34 :{ //C - JNZ 
                    memory[memoryindex++]=194;
                    i++;
                    strcpy(a,f2[i]);
                    int k=searchsymtabindex(a,symtabindex);
                    if(k==-1)
                    {
                        printf("Error in Jump Function. Label not found");
                    }
                    else
                    temps=symtab[k].relativeposition;
                    int targetaddress;
                    targetaddress=temps+startaddress;
                    memory[memoryindex++]=targetaddress%256;
                    memory[memoryindex++]=targetaddress/256;
                    break;
                }
            case 35 :{ //C - JP 
                        memory[memoryindex++]=242;
                    i++;
                    strcpy(a,f2[i]);
                    int k=searchsymtabindex(a,symtabindex);
                    if(k==-1)
                    {
                        printf("Error in Jump Function. Label not found");
                    }
                    else
                    temps=symtab[k].relativeposition;
                    int targetaddress;
                    targetaddress=temps+startaddress;
                    memory[memoryindex++]=targetaddress%256;
                    memory[memoryindex++]=targetaddress/256;
                    break;
                }
            case 36 :{ //C - JPE 
                        memory[memoryindex++]=234;
                    i++;
                    strcpy(a,f2[i]);
                    int k=searchsymtabindex(a,symtabindex);
                    if(k==-1)
                    {
                        printf("Error in Jump Function. Label not found");
                    }
                    else
                    temps=symtab[k].relativeposition;
                    int targetaddress;
                    targetaddress=temps+startaddress;
                    memory[memoryindex++]=targetaddress%256;
                    memory[memoryindex++]=targetaddress/256;
                    break;
                }
            case 37 :{ //C - JPO 
                        memory[memoryindex++]=226;
                    i++;
                    strcpy(a,f2[i]);
                    int k=searchsymtabindex(a,symtabindex);
                    if(k==-1)
                    {
                        printf("Error in Jump Function. Label not found");
                    }
                    else
                    temps=symtab[k].relativeposition;
                    int targetaddress;
                    targetaddress=temps+startaddress;
                    memory[memoryindex++]=targetaddress%256;
                    memory[memoryindex++]=targetaddress/256;
                    break;
                }
            case 38 :{ //C - JZ 
                        memory[memoryindex++]=202;
                    i++;
                    strcpy(a,f2[i]);
                    int k=searchsymtabindex(a,symtabindex);
                    if(k==-1)
                    {
                        printf("Error in Jump Function. Label not found");
                    }
                    else
                    temps=symtab[k].relativeposition;
                    int targetaddress;
                    targetaddress=temps+startaddress;
                    memory[memoryindex++]=targetaddress%256;
                    memory[memoryindex++]=targetaddress/256;
                    break;
                }
            case 39 :{ //C - LDA 
                        memory[memoryindex++]=58;
                        i++;
                        strcpy(a,f2[i]);
                        temps=getdecvalue(a,4);
                        memory[memoryindex++]=temps%256;
                        memory[memoryindex++]=temps/256;

                    break;
                }
            case 40 :{ //C - LDAX 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 2 : {memory[memoryindex++]=10;break;}
                            case 4 : {memory[memoryindex++]= 26 ;break;}                      
                        }
                    break;
                }
            case 41 :{ //C - LHLD 
                         memory[memoryindex++]=42;
                        i++;
                        strcpy(a,f2[i]);
                        temps=getdecvalue(a,4);
                        memory[memoryindex++]=temps%256;
                        memory[memoryindex++]=temps/256;
                    break;
                }
            case 42 :{ //C - LXI 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 2 : {memory[memoryindex++]=1;break;}
                            case 4 : {memory[memoryindex++]= 17 ;break;}                      
                            case 6 : {memory[memoryindex++]= 33;break;}                         
                            case 9 : {memory[memoryindex++]=49;break;}
                        }
                        if(strcmp(a,",")==0)
                        {
                            i++;
                            strcpy(a,f2[i]);
                        }
                        else
                        {   
                            printf("Error in LXI.\n");
                            return 0;
                        }
                        temps=getdecvalue(a,4);
                        memory[memoryindex++]=temps%256;
                        memory[memoryindex++]=temps/256;
                    break;
                }
            case 43 :{ //C - MOV 
                        i++;
                        int choice;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        temps*=10;
                        choice=temps;
                        i++;
                        strcpy(a,f2[i]);
                        if(strcmp(a,",")==0)
                        {
                            i++;
                            strcpy(a,f2[i]);
                        }
                        else
                        {   
                            printf("Error in MOV.\n");
                            return 0;
                        }
                        temps=getregistersegment(a);
                        choice+=temps;
                        switch(choice)
                        {
                            case 11 : {memory[memoryindex++]=  127;break;}
                            case 12 : {memory[memoryindex++]= 120 ;break;}
                            case 13 : {memory[memoryindex++]= 121 ;break;}
                            case 14 : {memory[memoryindex++]=122  ;break;}
                            case 15 : {memory[memoryindex++]= 123 ;break;}
                            case 16 : {memory[memoryindex++]= 124 ;break;}
                            case 17 : {memory[memoryindex++]= 125 ;break;}
                            case 18 : {memory[memoryindex++]= 126 ;break;}
                            case 21 : {memory[memoryindex++]=71  ;break;}
                            case 22 : {memory[memoryindex++]= 64 ;break;}
                            case 23 : {memory[memoryindex++]=  65;break;}
                            case 24 : {memory[memoryindex++]=  66;break;}
                            case 25 : {memory[memoryindex++]=  67;break;}
                            case 26 : {memory[memoryindex++]=68  ;break;}
                            case 27 : {memory[memoryindex++]= 69 ;break;}
                            case 28 : {memory[memoryindex++]= 70 ;break;}
                            case 31 : {memory[memoryindex++]= 79 ;break;}
                            case 32 : {memory[memoryindex++]=  72;break;}
                            case 33 : {memory[memoryindex++]=  73;break;}
                            case 34 : {memory[memoryindex++]=74  ;break;}
                            case 35 : {memory[memoryindex++]= 75 ;break;}
                            case 36 : {memory[memoryindex++]= 76 ;break;}
                            case 37 : {memory[memoryindex++]=77  ;break;}
                            case 38 : {memory[memoryindex++]= 78 ;break;}
                            case 41 : {memory[memoryindex++]= 87 ;break;}
                            case 42 : {memory[memoryindex++]=80  ;break;}
                            case 43 : {memory[memoryindex++]=81  ;break;}
                            case 44 : {memory[memoryindex++]=82  ;break;}
                            case 45 : {memory[memoryindex++]= 83 ;break;}
                            case 46 : {memory[memoryindex++]= 84 ;break;}
                            case 47 : {memory[memoryindex++]= 85 ;break;}
                            case 48 : {memory[memoryindex++]= 86 ;break;}
                            case 51 : {memory[memoryindex++]=95  ;break;}
                            case 52 : {memory[memoryindex++]=88  ;break;}
                            case 53 : {memory[memoryindex++]=89  ;break;}
                            case 54 : {memory[memoryindex++]= 90 ;break;}
                            case 55 : {memory[memoryindex++]= 91 ;break;}
                            case 56 : {memory[memoryindex++]= 92 ;break;}
                            case 57 : {memory[memoryindex++]= 93 ;break;}
                            case 58 : {memory[memoryindex++]= 94 ;break;}
                            case 61 : {memory[memoryindex++]= 103 ;break;}
                            case 62 : {memory[memoryindex++]= 96 ;break;}
                            case 63 : {memory[memoryindex++]=  97;break;}
                            case 64 : {memory[memoryindex++]=  98;break;}
                            case 65 : {memory[memoryindex++]= 99 ;break;}
                            case 66 : {memory[memoryindex++]=  100;break;}
                            case 67 : {memory[memoryindex++]= 101 ;break;}
                            case 68 : {memory[memoryindex++]= 102 ;break;}
                            case 71 : {memory[memoryindex++]=111  ;break;}
                            case 72 : {memory[memoryindex++]= 104 ;break;}
                            case 73 : {memory[memoryindex++]= 105 ;break;}
                            case 74 : {memory[memoryindex++]= 106 ;break;}
                            case 75 : {memory[memoryindex++]=107  ;break;}
                            case 76 : {memory[memoryindex++]= 108 ;break;}
                            case 77 : {memory[memoryindex++]= 109 ;break;}
                            case 78 : {memory[memoryindex++]= 110 ;break;}
                            case 81 : {memory[memoryindex++]=119  ;break;}
                            case 82 : {memory[memoryindex++]=112  ;break;}
                            case 83 : {memory[memoryindex++]=113  ;break;}
                            case 84 : {memory[memoryindex++]=114  ;break;}
                            case 85 : {memory[memoryindex++]=115  ;break;}
                            case 86 : {memory[memoryindex++]=116  ;break;}
                            case 87 : {memory[memoryindex++]=117  ;break;}
                            

                        }

                    break;
                }
            case 44 :{ //C - MVI 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 1 : {memory[memoryindex++]= 62  ;break;}
                            case 2 : {memory[memoryindex++]= 6 ;break;}
                            case 3 : {memory[memoryindex++]= 14 ;break;}
                            case 4 : {memory[memoryindex++]= 22 ;break;}
                            case 5 : {memory[memoryindex++]= 30 ;break;}
                            case 6 : {memory[memoryindex++]= 38 ;break;}
                            case 7 : {memory[memoryindex++]= 46 ;break;}
                            case 8 : {memory[memoryindex++]= 54 ;break;}
       
                        }
                        i++;
                        strcpy(a,f2[i]);
                        if(strcmp(a,",")==0)
                        {
                            i++;
                            strcpy(a,f2[i]);
                        }
                        else
                        {   
                            printf("Error in MVI.\n");
                            return 0;
                        }
                        temps=getdecvalue(a,2);
                        memory[memoryindex++]=temps;
                    break;
                }
            case 45 :{ //C - NOP 
                    memory[memoryindex++]=0;
                    break;
                }
            case 46 :{ //C - ORA 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 1 : {memory[memoryindex++]= 183  ;break;}
                            case 2 : {memory[memoryindex++]= 176 ;break;}
                            case 3 : {memory[memoryindex++]= 177 ;break;}
                            case 4 : {memory[memoryindex++]= 178 ;break;}
                            case 5 : {memory[memoryindex++]= 179 ;break;}
                            case 6 : {memory[memoryindex++]= 180 ;break;}
                            case 7 : {memory[memoryindex++]= 181 ;break;}
                            case 8 : {memory[memoryindex++]= 182 ;break;}
                            
                        }
                    break;
                }
            case 47 :{ //C - ORI
                        memory[memoryindex++]=246;
                        i++;
                        strcpy(a,f2[i]);
                        temps=getdecvalue(a,2);
                        memory[memoryindex++]=temps; 

                    break;
                }
            case 48 :{ //ND- OUT 

                    break;
                }
            case 49 :{ //ND- PCHL 

                    break;
                }
            case 50 :{ //ND- POP 

                    break;
                }
            case 51 :{ //ND- PUSH 

                    break;
                }
            case 52 :{ //C - RAL 
                    memory[memoryindex++]=23;
                    break;
                }
            case 53 :{ //C - RAR 
                         memory[memoryindex++]=31;
                    break;
                }
            case 54 :{ //ND- RC 

                    break;
                }
            case 55 :{ //ND- RET 

                    break;
                }
            case 56 :{ //ND- RIM 

                    break;
                }
            case 57 :{ //C - RLC 
                        memory[memoryindex++]=7;
                    break;
                }
            case 58 :{ //ND- RM 

                    break;
                }
            case 59 :{ //ND- RNC 

                    break;
                }
            case 60 :{ //ND- RNZ 

                    break;
                }
            case 61 :{ //ND- RP 

                    break;
                }
            case 62 :{ //ND- RPE 

                    break;
                }
            case 63 :{ //ND- RPO 

                    break;
                }
            case 64 :{ //C - RRC 
                        memory[memoryindex++]=15;
                    break;
                }
            case 65 :{ //C - RST 
                         i++;
                        strcpy(a,f2[i]);
                        char x;
                        x=a[0];
                        switch(x)
                        {
                            case '0' : {memory[memoryindex++]= 199 ;break;}
                            case '1' : {memory[memoryindex++]= 207 ;break;}
                            case '2' : {memory[memoryindex++]= 215 ;break;}
                            case '3' : {memory[memoryindex++]= 223 ;break;}
                            case '4' : {memory[memoryindex++]= 231 ;break;}
                            case '5' : {memory[memoryindex++]= 239 ;break;}
                            case '6' : {memory[memoryindex++]= 247 ;break;}
                            case '7' : {memory[memoryindex++]= 255 ;break;}
                            
                            
                        }
                    break;
                }
            case 66 :{ //ND- RZ 

                    break;
                }
            case 67 :{ //C - SBB 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 1 : {memory[memoryindex++]= 159 ;break;}
                            case 2 : {memory[memoryindex++]= 152 ;break;}
                            case 3 : {memory[memoryindex++]= 153 ;break;}
                            case 4 : {memory[memoryindex++]= 154 ;break;}
                            case 5 : {memory[memoryindex++]= 155 ;break;}
                            case 6 : {memory[memoryindex++]= 156 ;break;}
                            case 7 : {memory[memoryindex++]= 157 ;break;}
                            case 8 : {memory[memoryindex++]= 158 ;break;}
                            
                        }
                    break;
                }
            case 68 :{ //C - SBI 
                        memory[memoryindex++]=222;
                        i++;
                        strcpy(a,f2[i]);
                        temps=getdecvalue(a,2);
                        memory[memoryindex++]=temps; 
                    break;
                }
            case 69 :{ //C - SHLD 
                        memory[memoryindex++]=34;
                        i++;
                        strcpy(a,f2[i]);
                        temps=getdecvalue(a,4);
                        memory[memoryindex++]=temps%256;
                        memory[memoryindex++]=temps/256;
                    break;
                }
            case 70 :{ //ND- SIM 

                    break;
                }
            case 71 :{ //ND- SPHL 

                    break;
                }
            case 72 :{ //C - STA 
                        memory[memoryindex++]=50;
                        i++;
                        strcpy(a,f2[i]);
                        temps=getdecvalue(a,4);
                        memory[memoryindex++]=temps%256;
                        memory[memoryindex++]=temps/256;
                    break;
                }
            case 73 :{ //C - STAX 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 2 : {memory[memoryindex++]=2;break;}
                            case 4 : {memory[memoryindex++]= 18;break;}                      
                        }
                    break;
                }
            case 74 :{ //C - STC 
                    memory[memoryindex++]=55;
                    break;
                }
            case 75 :{ //C - SUB 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 1 : {memory[memoryindex++]= 151 ;break;}
                            case 2 : {memory[memoryindex++]= 144 ;break;}
                            case 3 : {memory[memoryindex++]= 145 ;break;}
                            case 4 : {memory[memoryindex++]= 146 ;break;}
                            case 5 : {memory[memoryindex++]= 147 ;break;}
                            case 6 : {memory[memoryindex++]= 148 ;break;}
                            case 7 : {memory[memoryindex++]= 149 ;break;}
                            case 8 : {memory[memoryindex++]= 150 ;break;}
                            
                        }
                    break;
                }
            case 76 :{ //C - SUI 
                        memory[memoryindex++]=214;
                        i++;
                        strcpy(a,f2[i]);
                        temps=getdecvalue(a,2);
                        memory[memoryindex++]=temps; 
                    break;
                }
            case 77 :{ //C - XCHG 
                    memory[memoryindex++]=235;
                    break;
                }
            case 78 :{ //C - XRA 
                        i++;
                        strcpy(a,f2[i]);
                        temps=getregistersegment(a);
                        switch(temps)
                        {
                            case 1 : {memory[memoryindex++]= 175 ;break;}
                            case 2 : {memory[memoryindex++]= 168 ;break;}
                            case 3 : {memory[memoryindex++]= 169 ;break;}
                            case 4 : {memory[memoryindex++]= 170 ;break;}
                            case 5 : {memory[memoryindex++]= 171 ;break;}
                            case 6 : {memory[memoryindex++]= 172 ;break;}
                            case 7 : {memory[memoryindex++]= 173 ;break;}
                            case 8 : {memory[memoryindex++]= 174 ;break;}
                            
                        }
                    break;
                }
            case 79 :{ //C - XRI 
                        memory[memoryindex++]=238;
                        i++;
                        strcpy(a,f2[i]);
                        temps=getdecvalue(a,2);
                        memory[memoryindex++]=temps; 
                    break;
                }
            case 80 :{ //ND- XTHL 

                    break;
                }


                    break;
        }
    }while(1);
    printf("Status Check: Difference = %d\t (0 denotes no error)\n", memoryindex-locctr);
    /*for (i=0;i<memoryindex;i++)
    {
        printf("%X\t",memory[i]);
    }//*/
//Memory Segment created    
    FILE *fpcodemem;
    fpcodemem=fopen("memseg.txt","w");
    for (i=0;i<1024;)
    {
        fprintf(fpcodemem,"%X\t",i+startaddress);
        for(int j=0;j<16;j++,i++)
        {
            
            if(memory[i]>15)
                fprintf(fpcodemem,"%X  ",memory[i]);
            else fprintf(fpcodemem,"0%X  ",memory[i]);
        }
        fprintf(fpcodemem,"\n");  
    }
    fclose(fpcodemem);
//Saving object code
    FILE *objcode;
    objcode=fopen("objectcode.txt","w");
    for (i=0;i<memoryindex;i++)
    {
        if(memory[i]>15)
             fprintf(objcode,"%X\t%X\n",i+startaddress,memory[i]);
        else fprintf(objcode,"%X\t0%X\n",i+startaddress,memory[i]);
    }
    fclose(objcode);




//SIMULATOR
//Intialisation

    initialise_simulator();
    int simindex=0;
    int simtemp=0;
    int simchoice=0;
    int refadd=getdecvalue("2400",4);
    int debuggermode=0;
//get data for data segment
    printf("Data Entry Section\nEnter 1 to use fresh data\nEnter 2 to use previous data\n->");
    scanf("%d",&simtemp);
    if(simtemp==2)
    {
        FILE *dataseg;
        dataseg=fopen("datasegment.txt","r");
        for( i=0;i<512;)
        {
            int discard;
            fscanf(dataseg,"%X",&discard);
            for(int j=0;j<16;j++,i++)
            {
                fscanf(dataseg,"%X",&fixedmemory[i]);
            }
        }
        fclose(dataseg);
    }
    while(1)
    {
        int optiondata;
        printf("Do you want to add any data? 1- Yes 0-No\n->");
        scanf("%d",&optiondata);
        if(optiondata==0)
        {
            printf("\n\nLOG: Data entry complete.\n\n");
            break;
        }
        printf("Enter Location in Hex: between 2400 and 25FF\n->");
        scanf("%X",&optiondata);
        printf("Enter value in Hex: 00 to FF\n->");
        scanf("%X",&simtemp);
        fixedmemory[optiondata-refadd]=simtemp;
    }
//debugger
    printf("Run in debugging mode? - 1- YES 0- No\n->");
    scanf("%d",&debuggermode);
    //printf("Status Check: Debugger Mode on/off: %d")
    if(debuggermode==1)
    {
        printf("Log: Starting Debugging Mode\n");
        displayheader();
        printf("INIT SET  ");
        displayregsnflag();
    }
    i=0;
//run sim    
    do
    {
        
        i++;
        simchoice=memory[simindex++];
        int displayop=simchoice;
        //
       // printf("%d\n",simchoice);
        switch(simchoice)
        {
            case 0  :{//nop
                        break;
                }
            case 1 :{//LXI B
                        simtemp=memory[simindex++];
                        C=simtemp;
                        simtemp=memory[simindex++];
                        B=simtemp;
                        break;
                    }
            case 2 :{//STAX B
                        int address=(B*256 + C)-refadd;
                        fixedmemory[address]=A;
                        break;
                    }
            case 3 :{//INX B
                        simtemp=B*256+C;
                        simtemp++;
                        B=simtemp/256;
                        C=simtemp%256;

                        break;
                    }
            case 4 :{//INR B
                        B+=1;
                        B%=256;

                        break;
                    }
            case 5 :{//DCR B
                        B=B-1;
                        if(B<0)
                        {
                            B+=256;
                            S=1;
                        }
                        if(B==0) Z=1;
                        else Z=0; 

                        break;
                    }
            case 6 :{//MVI B,Data
                        B=memory[simindex++];
                        break;
                    }
            case 7 :{//RLC
                        simtemp=A/128;
                        A=A%128;
                        A=A*2;
                        A=A+simtemp;
                        CY=simtemp;

                        break;
                    }
            case 8 :{//DO NOT EXIST
                        printf("terminating program. STRAY VALUE ENCOUNTERED.DNE. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 9 :{//DAD B
                        L=L+C;
                        if(L>=256)
                        {
                            L-=256;
                            CY=1;
                        }else CY=0;
                        H=H+B+CY;
                        if(H>=256)
                        {
                            H-=256;
                            CY=1;
                        }else CY=0;

                        break;
                    }
            case 10 :{//LDAX B
                        int address=(B*256 + C)-refadd;
                        A=fixedmemory[address];

                        break;
                    }
            case 11 :{//DCX B
                        simtemp=B*256+C;
                        simtemp--;
                        B=simtemp/256;
                        C=simtemp%256;
                        if(simtemp==0) Z=1;
                        else Z=0; 
                        break;
                    }
            case 12 :{//INR C
                        C+=1;
                        C%=256;

                        break;
                    }
            case 13 :{//DCR C
                        C=C-1;
                        if(C<0)
                        {
                            C+=256;
                            S=1;
                        }
                        if(C==0) Z=1;
                        else Z=0; 
                        break;
                    }
            case 14 :{//MVI C,Data
                        C=memory[simindex++];

                        break;
                    }
            case 15 :{//RRC
                        simtemp=A%2;
                        A=A/2;
                        A=(128*simtemp);
                        CY=simtemp;

                        break;
                    }
            case 16 :{//DO NOT EXIST
                        printf("terminating program. STRAY VALUE ENCOUNTERED.DNE. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 17 :{//LXI D
                        simtemp=memory[simindex++];
                        E=simtemp;
                        simtemp=memory[simindex++];
                        D=simtemp;
                        break;

                        break;
                    }
            case 18 :{//STAX D
                        int address=(D*256 + E)-refadd;
                        fixedmemory[address]=A;
                        break;
                    }
            case 19 :{//INX D
                        simtemp=D*256+E;
                        simtemp++;
                        D=simtemp/256;
                        E=simtemp%256;
                        if(simtemp==0) Z=1;
                        else Z=0; 
                        break;
                    }
            case 20 :{//INR D
                        D+=1;
                        D%=256;

                        break;
                    }
            case 21 :{//DCR D
                        D=D-1;
                        if(D<0)
                        {
                            D+=256;
                            S=1;
                        }
                        if(D==0) Z=1;
                        else Z=0; 

                        break;
                    }
            case 22 :{//MVI D,Data
                        D=memory[simindex++];

                        break;
                    }
            case 23 :{//RAL
                        simtemp=A/128;
                        A=A%128;
                        A=A*2;
                        A=A+simtemp;

                        break;
                    }
            case 24 :{//DO NOT EXIST
                        printf("terminating program. STRAY VALUE ENCOUNTERED.DNE. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 25 :{//DAD D
                        L=L+E;
                        if(L>=256)
                        {
                            L-=256;
                            CY=1;
                        }else CY=0;
                        H=H+D+CY;
                        if(H>=256)
                        {
                            H-=256;
                            CY=1;
                        }else CY=0;

                        break;
                    }
            case 26 :{//LDAX D
                        int address=(D*256 + E)-refadd;
                        A=fixedmemory[address];

                        break;
                    }
            case 27 :{//DCX D
                        simtemp=D*256+E;
                        simtemp--;
                        D=simtemp/256;
                        E=simtemp%256;
                        if(simtemp==0) Z=1;
                        else Z=0; 
                        break;
                    }
            case 28 :{//INR E
                        E+=1;
                        E%=256;

                        break;
                    }
            case 29 :{//DCR E
                        E=E-1;
                        if(E<0)
                        {
                            E+=256;
                            S=1;
                        }
                        if(Z==0) Z=1;
                        else Z=0; 

                        break;
                    }
            case 30 :{//MVI E,Data
                        E=memory[simindex++];

                        break;
                    }
            case 31 :{//RAR
                        simtemp=A%2;
                        A=A/2;
                        A=(128*simtemp);


                        break;
                    }
            case 32 :{//RIM
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 33 :{//LXI H
                        simtemp=memory[simindex++];
                        L=simtemp;
                        simtemp=memory[simindex++];
                        H=simtemp;
                        break;

                        break;
                    }
            case 34 :{//SHLD Add
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=refadd;
                        L=fixedmemory[address];
                        H=fixedmemory[address+1];
                        break;
                    }
            case 35 :{//INX H
                        simtemp=H*256+L;
                        simtemp++;
                        H=simtemp/256;
                        L=simtemp%256;
                        setzero(simtemp);
                        break;
                    }
            case 36 :{//INR H
                        H+=1;
                        H%=256;
                        setzero(H);
                        break;
                    }
            case 37 :{//DCR H
                        H=H-1;
                        if(H<0)
                        {
                            H+=256;
                            S=1;
                        }
                        setzero(H);

                        break;
                    }
            case 38 :{//MVI H,Data
                        H=memory[simindex++];

                        break;
                    }
            case 39 :{//DAA
                        simtemp=A%10;
                        M=simtemp;
                        simtemp=A/10;
                        M+=(simtemp*16);
                        A=M;
                        break;
                    }
            case 40 :{//DO NOT EXIST
                        printf("terminating program. STRAY VALUE ENCOUNTERED.DNE. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 41 :{//DAD H
                        L=L+L;
                        if(L>=256)
                        {
                            L-=256;
                            CY=1;
                        }else CY=0;
                        H=H+H+CY;
                        if(H>=256)
                        {
                            H-=256;
                            CY=1;
                        }else CY=0;

                        break;
                    }
            case 42 :{//LHLD Add
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=refadd;
                        fixedmemory[address]=L;
                        fixedmemory[address+1]=H;

                        break;
                    }
            case 43 :{//DCX H
                        simtemp=H*256+L;
                        simtemp--;
                        H=simtemp/256;
                        L=simtemp%256;
                        setzero(simtemp);
                        break;
                    }
            case 44 :{//INR L
                        L+=1;
                        L%=256;

                        break;
                    }
            case 45 :{//DCR L
                        L=L-1;
                        if(L<0)
                        {
                            L+=256;
                            S=1;
                        }
                        setzero(L);
                        break;
                    }
            case 46 :{//MVI L,Data
                        L=memory[simindex++];

                        break;
                    }
            case 47 :{//CMA
                        A=255-A;

                        break;
                    }
            case 48 :{//SIM
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 49 :{//LXI SP
                        simtemp=memory[simindex++];
                        SP=simtemp;
                        simtemp=memory[simindex++];
                        SP+=(simtemp*256);
                        break;

                        break;
                    }
            case 50 :{//STA Address
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=refadd;
                        fixedmemory[address]=A;
                        break;
                    }
            case 51 :{//INX SP
                        SP++;

                        break;
                    }
            case 52 :{//INR M
                        int address=(H*256 + L)-refadd;
                        fixedmemory[address]+=1;
                        fixedmemory[address]%=1;

                        break;
                    }
            case 53 :{//DCR M
                        int address=(H*256 + L)-refadd;
                        fixedmemory[address]-=1;
                        
                        if(fixedmemory[address]<0)
                        {
                            fixedmemory[address]+=256;
                            S=1;
                        }
                        setzero(fixedmemory[address]);
                        break;
                    }
            case 54 :{//MVI M,Data
                        int address=(H*256 + L)-refadd;
                        fixedmemory[address]=memory[simindex++];
                        break;
                    }
            case 55 :{//STC
                        CY=1;

                        break;
                    }
            case 56 :{//DO NOT EXIST
                        printf("terminating program. STRAY VALUE ENCOUNTERED.DNE. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 57 :{//DAD SP
                        L=L+(SP%256);
                        if(L>=256)
                        {
                            L-=256;
                            CY=1;
                        }else CY=0;
                        H=H+(SP/256)+CY;
                        if(H>=256)
                        {
                            H-=256;
                            CY=1;
                        }else CY=0;

                        break;
                    }
            case 58 :{//LDA Address
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=refadd;
                        A=fixedmemory[address];
                        break;
                    }
            case 59 :{//DCX SP
                        SP--;

                        break;
                    }
            case 60 :{//INR A
                        A+=1;
                        A%=256;

                        break;
                    }
            case 61 :{//DCR A
                        A=A-1;
                        if(A<0)
                        {
                            A+=256;
                            S=1;
                        }
                        setzero(A);
                        break;
                    }
            case 62 :{//MVI A,Data
                        A=memory[simindex++];

                        break;
                    }
            case 63 :{//CMC
                        CY=1-CY;

                        break;
                    }
            //MOV SECTION START                    
            case 64 :{//MOV B,B

                        break;
                    }
            case 65 :{//MOV B,C
                        B=C;

                        break;
                    }
            case 66 :{//MOV B,D
                        B=D;
                        break;
                    }
            case 67 :{//MOV B,E
                        B=E;
                        break;
                    }
            case 68 :{//MOV B,H
                        B=H;
                        break;
                    }
            case 69 :{//MOV B,L
                        B=L;
                        break;
                    }
            case 70 :{//MOV B,M
                        int address=(H*256 + L)-refadd;
                        B=fixedmemory[address];
                        break;
                    }
            case 71 :{//MOV B,A
                        B=A;
                        break;
                    }
            case 72 :{//MOV C,B
                        C=B;
                        break;
                    }
            case 73 :{//MOV C,C
                        
                        break;
                    }
            case 74 :{//MOV C,D
                        C=D;
                        break;
                    }
            case 75 :{//MOV C,E
                        C=E;
                        break;
                    }
            case 76 :{//MOV C,H
                        C=H;
                        break;
                    }
            case 77 :{//MOV C,L
                        C=L;
                        break;
                    }
            case 78 :{//MOV C.M
                        int address=(H*256 + L)-refadd;
                        C=fixedmemory[address];
                        break;
                    }
            case 79 :{//MOV C,A
                        C=A;
                        break;
                    }
            case 80 :{
                        D=B;
                        break;
                    }
            case 81 :{
                        D=C;
                        break;
                    }
            case 82 :{
                        
                        break;
                    }
            case 83 :{
                        D=E;
                        break;
                    }
            case 84 :{
                        D=H;
                        break;
                    }
            case 85 :{
                        D=L;
                        break;
                    }
            case 86 :{
                        int address=(H*256 + L)-refadd;
                        D=fixedmemory[address];
                        break;
                    }
            case 87 :{
                        D=A;
                        break;
                    }
            case 88 :{
                        E=B;
                        break;
                    }
            case 89 :{
                        E=C;
                        break;
                    }
            case 90 :{
                        E=D;
                        break;
                    }
            case 91 :{
                        
                        break;
                    }
            case 92 :{
                        E=H;
                        break;
                    }
            case 93 :{
                        E=L;
                        break;
                    }
            case 94 :{
                        int address=(H*256 + L)-refadd;
                        E=fixedmemory[address];
                        break;
                    }
            case 95 :{
                        E=A;
                        break;
                    }
            case 96 :{
                        H=B;
                        break;
                    }
            case 97 :{
                        H=C;
                        break;
                    }
            case 98 :{
                        H=D;
                        break;
                    }
            case 99 :{
                        H=E;
                        break;
                    }
            case 100 :{

                        break;
                    }
            case 101 :{
                        H=L;
                        break;
                    }
            case 102 :{
                        int address=(H*256 + L)-refadd;
                        H=fixedmemory[address];
                        break;
                    }
            case 103 :{
                        H=A;
                        break;
                    }
            case 104 :{
                        L=B;
                        break;
                    }
            case 105 :{
                        L=C;
                        break;
                    }
            case 106 :{
                        L=D;
                        break;
                    }
            case 107 :{
                        L=E;
                        break;
                    }
            case 108 :{
                        L=H;
                        break;
                    }
            case 109 :{
                        
                        break;
                    }
            case 110 :{
                        int address=(H*256 + L)-refadd;
                        L=fixedmemory[address];
                        break;
                    }
            case 111 :{
                        L=A;
                        break;
                    }
            case 112 :{
                        int address=(H*256 + L)-refadd;
                        fixedmemory[address]=B;
                        break;
                    }
            case 113 :{
                        int address=(H*256 + L)-refadd;
                        fixedmemory[address]=C;

                        break;
                    }
            case 114 :{
                        int address=(H*256 + L)-refadd;
                        fixedmemory[address]=D;

                        break;
                    }
            case 115 :{
                        int address=(H*256 + L)-refadd;
                        fixedmemory[address]=E;

                        break;
                    }
            case 116 :{
                        int address=(H*256 + L)-refadd;
                        fixedmemory[address]=H;

                        break;
                    }
            case 117 :{

                        int address=(H*256 + L)-refadd;
                        fixedmemory[address]=L;

                        break;
                    }
            case 118 :{//HLT
                        printf("Code successfully terminated by HLT Interrupt\n");
                        simindex=-1;
                        break;
                    }
            case 119 :{
                        int address=(H*256 + L)-refadd;
                        fixedmemory[address]=A;

                        break;
                    }
            case 120 :{//MOV A,B
                        A=B;
                        break;
                    }
            case 121 :{//MOV A,C
                        A=C;
                        break;
                    }
            case 122 :{//MOV A,D
                        A=D;
                        break;
                    }
            case 123 :{//MOV A,E
                        A=E;

                        break;
                    }
            case 124 :{//MOV A,H
                        A=H;
                        break;
                    }
            case 125 :{//MOV A,L
                        A=L;
                        break;
                    }
            case 126 :{//MOV A,M
                        int address=(H*256 + L)-refadd;
                        A=fixedmemory[address];
                        break;
                    }
            case 127 :{//MOV A,A

                        break;
                    }
            //MOV SECTION ENDS
            case 128 :{//ADD B
                        A=A+B;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);
                        break;
                    }
            case 129 :{//ADD C
                        A=A+C;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 130 :{//ADD D
                        A=A+D;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 131 :{//ADD E
                        A=A+E;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 132 :{//ADD H
                        A=A+H;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        
                        else CY=0;
                        setzero(A);
                        break;
                    }
            case 133 :{//ADD L
                        A=A+L;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 134 :{//ADD M-
                        int address=(H*256 + L)-refadd;
                        A=A+fixedmemory[address];
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 135 :{//ADD A
                        A=A+A;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 136 :{//ADC B
                        A=A+B+CY;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 137 :{//ADC C
                        A=A+C+CY;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 138 :{//ADC D
                        A=A+D+CY;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 139 :{//ADC E
                        A=A+E+CY;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 140 :{//ADC H
                        A=A+H+CY;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 141 :{//ADC L
                        A=A+L+CY;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 142 :{//ADC M-
                        int address=(H*256 + L)-refadd;
                        A=A+fixedmemory[address]+CY;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 143 :{//ADC A
                        A=A+A+CY;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 144 :{//SUB B
                        A=A-B ;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);
                        break;
                    }
            case 145 :{//SUB C
                        A=A-C ;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 146 :{//SUB D
                        A=A-D ;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 147 :{//SUB E 
                        A=A-E ;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 148 :{//SUB H
                        A=A-H ;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 149 :{//SUB L
                        A=A-L ;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 150 :{//SUB M
                        int address=(H*256 + L)-refadd;
                        M=fixedmemory[address];
                        A=A-M ;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 151 :{//SUB A
                        A=A-A ;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;

                        setzero(A);

                        break;
                    }
            case 152 :{//SBB B
                        A=A-B -CY;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);
                        

                        break;
                    }
            case 153 :{//SBB C
                        A=A-C-CY;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);
                        


                        break;
                    }
            case 154 :{//SBB D
                        A=A-D-CY;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 155 :{//SBB E
                        A=A-E-CY;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 156 :{//SBB H
                        A=A-H-CY;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);
                        

                        break;
                    }
            case 157 :{//SBB L
                        A=A-L-CY;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);
                        

                        break;
                    }
            case 158 :{//SBB M
                        int address=(H*256 + L)-refadd;
                        M=fixedmemory[address];
                        A=A-M-CY;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);
                        

                        break;
                    }
            case 159 :{//SBB A
                        A=A-A-CY;
                        if(A<0)
                        {
                            CY=1;
                            A+=256;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 160 :{//ANA B
                        A=A&B;
                        setzero(A);
                        

                        break;
                    }
            case 161 :{//ANA C
                        A=A&C;
                        setzero(A);


                        break;
                    }
            case 162 :{//ANA D
                        A=A&D;
                        setzero(A);


                        break;
                    }
            case 163 :{//ANA E
                        A=A&E;
                        setzero(A);


                        break;
                    }
            case 164 :{//ANA H
                        A=A&H;
                        setzero(A);


                        break;
                    }
            case 165 :{//ANA L
                        A=A&L;
                        setzero(A);


                        break;
                    }
            case 166 :{//ANA M
                        int address=(H*256 + L)-refadd;
                        M=fixedmemory[address];
                        A=A&M;
                        setzero(A);


                        break;
                    }
            case 167 :{//ANA A
                        setzero(A);
                        break;
                    }
            case 168 :{//XRA B
                        A=A^B ;
                        setzero(A);

                        break;
                    }
            case 169 :{//XRA C
                        A=A^C ;
                        setzero(A);

                        break;
                    }
            case 170 :{//XRA D
                        A=A^D ;
                        setzero(A);

                        break;
                    }
            case 171 :{//XRA E
                        A=A^E ;
                        setzero(A);

                        break;
                    }
            case 172 :{//XRA H 
                        A=A^H ;
                        setzero(A);

                        break;
                    }
            case 173 :{//XRA L
                        A=A^L ;
                        setzero(A);

                        break;
                    }
            case 174 :{//XRA M 
                        int address=(H*256 + L)-refadd;
                        M=fixedmemory[address];
                        A=A^M ;
                        setzero(A);

                        break;
                    }
            case 175 :{//XRA A
                        A=A^A;
                        setzero(A);

                        break;
                    }
            case 176 :{//ORA B
                        A=A|B  ;
                        setzero(A);
                        break;
                    }
            case 177 :{//ORA C
                        A=A|C  ;
                        setzero(A);

                        break;
                    }
            case 178 :{//ORA D
                        A=A|D  ;
                        setzero(A);

                        break;
                    }
            case 179 :{//ORA E 
                        A=A|E  ;
                        setzero(A);

                        break;
                    }
            case 180 :{//ORA H
                        A=A|H  ;
                        setzero(A);

                        break;
                    }
            case 181 :{//ORA L
                        A=A|L  ;
                        setzero(A);

                        break;
                    }
            case 182 :{//ORA M
                        int address=(H*256 + L)-refadd;
                        M=fixedmemory[address];
                        A=A|M  ;
                        setzero(A);

                        break;
                    }
            case 183 :{//ORA A
                        A=A|A  ;
                        setzero(A);

                        break;
                    }
            case 184 :{//CMP B
                        if(A== B)
                        {
                            CY=0 ;
                            Z=1 ;
                        }
                        else if(A<B )
                        {
                            CY=1 ;
                            Z= 0;
                        }
                        else if(A>B )
                        {
                            CY=0 ;
                            Z=0 ;
                        }

                        break;
                    }
            case 185 :{//CMP C
                        if(A== C)
                        {
                            CY=0 ;
                            Z=1 ;
                        }
                        else if(A<C )
                        {
                            CY=1 ;
                            Z= 0;
                        }
                        else if(A>C )
                        {
                            CY=0 ;
                            Z=0 ;
                        }

                        break;
                    }
            case 186 :{//CMP D 
                        if(A==D )
                        {
                            CY=0 ;
                            Z=1 ;
                        }
                        else if(A<D )
                        {
                            CY=1 ;
                            Z= 0;
                        }
                        else if(A> D)
                        {
                            CY=0 ;
                            Z=0 ;
                        }

                        break;
                    }
            case 187 :{//CMP E
                        if(A==E )
                        {
                            CY=0 ;
                            Z=1 ;
                        }
                        else if(A<E )
                        {
                            CY=1 ;
                            Z= 0;
                        }
                        else if(A>E )
                        {
                            CY=0 ;
                            Z=0 ;
                        }

                        break;
                    }
            case 188 :{//CMP H
                        if(A==H )
                        {
                            CY=0 ;
                            Z=1 ;
                        }
                        else if(A<H )
                        {
                            CY=1 ;
                            Z= 0;
                        }
                        else if(A> H)
                        {
                            CY=0 ;
                            Z=0 ;
                        }

                        break;
                    }
            case 189 :{//CMP L
                        if(A==L)
                        {
                            CY=0 ;
                            Z=1 ;
                        }
                        else if(A<L)
                        {
                            CY=1 ;
                            Z= 0;
                        }
                        else if(A>L)
                        {
                            CY=0 ;
                            Z=0 ;
                        }

                        break;
                    }
            case 190 :{//CMP M
                        int address=(H*256 + L)-refadd;
                        M=fixedmemory[address];
                        if(A==M )
                        {
                            CY=0 ;
                            Z=1 ;
                        }
                        else if(A<M )
                        {
                            CY=1 ;
                            Z= 0;
                        }
                        else if(A> M)
                        {
                            CY=0 ;
                            Z=0 ;
                        }

                        break;
                    }
            case 191 :{//CMP A
                        CY=0;
                        Z=1;
                        break;
                    }
            case 192 :{//RNZ
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 193 :{//POP B
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 194 :{//JNZ Label
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=startaddress;
                        if(Z==0)
                            simindex=address;

                        break;
                    }
            case 195 :{//JMP Label
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=startaddress;
                        
                        simindex=address;

                        break;
                    }
            case 196 :{//CNZ
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 197 :{//PUSH B
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 198 :{//ADI DATA
                        simtemp=memory[simindex++];
                        A=A+simtemp;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 199 :{//RST 0
                        printf("Code successfully terminated by RST 0 Interrupt\n");
                        simindex=-1;

                        break;
                    }
            case 200 :{//RZ
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 201 :{//RET
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 202 :{//JZ Label
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=startaddress;
                        if(Z==1)
                            simindex=address;

                        break;
                    }
            case 203 :{//DO NOT EXIST
                        printf("terminating program. STRAY VALUE ENCOUNTERED.DNE. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 204 :{//CZ
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 205 :{//CALL
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 206 :{//ACI DATA
                        simtemp=memory[simindex++];
                        A=A+simtemp+CY;
                        if(A>=256)
                        {
                            A=A-256;
                            CY=1;
                        }
                        else CY=0;

                        break;
                    }
            case 207 :{//RST 1
                        printf("Code successfully terminated by RST 1 Interrupt\n");
                        simindex=-1;

                        break;
                    }
            case 208 :{//RNC
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 209 :{//POP D
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 210 :{//JNC Label
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=startaddress;
                        if(CY==0)
                            simindex=address; 

                        break;
                    }
            case 211 :{//OUT
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 212 :{//CNC
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 213 :{//PUSH D
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 214 :{//SUI DATA
                        simtemp=memory[simindex++];
                        A=A-simtemp;
                        if(A<0)
                        {
                            A=A+256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 215 :{//RST 2
                        printf("Code successfully terminated by RST 2 Interrupt\n");
                        simindex=-1;

                        break;
                    }
            case 216 :{//RC
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 217 :{//DO NOT EXIST
                        printf("terminating program. STRAY VALUE ENCOUNTERED.DNE. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 218 :{//JC Label
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=startaddress;
                        if(CY==1)
                            simindex=address;
                        break;
                    }
            case 219 :{//IN
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;


                        break;
                    }
            case 220 :{//CC
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 221 :{//DO NOT EXIST
                        printf("terminating program. STRAY VALUE ENCOUNTERED.DNE. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 222 :{//SBI DATA
                        simtemp=memory[simindex++];
                        A=A-simtemp-CY;
                        if(A<0)
                        {
                            A=A+256;
                            CY=1;
                        }
                        else CY=0;
                        setzero(A);

                        break;
                    }
            case 223 :{//RST 3
                        printf("Code successfully terminated by RST 3 Interrupt\n");
                        simindex=-1;

                        break;
                    }
            case 224 :{//RPO
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 225 :{//POP H
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 226 :{//JPO Label
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=startaddress;
                        if(P==0)
                            simindex=address;

                        break;
                    }
            case 227 :{//XTHL
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 228 :{//CPO
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 229 :{//PUSH H
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 230 :{//ANI DATA
                        simtemp=memory[simindex++];
                        A=A&simtemp;

                        break;
                    }
            case 231 :{//RST 4
                        printf("Code successfully terminated by RST 4 Interrupt\n");
                        simindex=-1;

                        break;
                    }
            case 232 :{//RPE
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 233 :{//PCHL
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 234 :{//JPE Label
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=startaddress;
                        if(P==1)
                            simindex=address; 

                        break;
                    }
            case 235 :{//XCHG
                        simtemp=D ;
                        D=H;
                        H=simtemp;
                        simtemp=E;
                        E=L;
                        L=simtemp;


                        break;
                    }
            case 236 :{//CPE
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 237 :{//DO NOT EXIST
                        printf("terminating program. STRAY VALUE ENCOUNTERED.DNE. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 238 :{//XRI DATA
                        simtemp=memory[simindex++];
                        A=A^simtemp;
                        setzero(A);

                        break;
                    }
            case 239 :{//RST 5
                        printf("Code successfully terminated by RST 5 Interrupt\n");
                        simindex=-1;

                        break;
                    }
            case 240 :{//RP
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 241 :{//POP PSW
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 242 :{//JP Label
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=startaddress;
                        if(S==0)
                            simindex=address;

                        break;
                    }
            case 243 :{//DI
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;


                        break;
                    }
            case 244 :{//CP
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 245 :{//PUSH PSW
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 246 :{//ORI DATA
                        simtemp=memory[simindex++];
                        A=A|simtemp;
                        setzero(A);

                        break;
                    }
            case 247 :{//RST 6
                        printf("Code successfully terminated by RST 6 Interrupt\n");
                        simindex=-1;

                        break;
                    }
            case 248 :{//RM
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 249 :{//SPHL
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 250 :{//JM Label
                        int address=0;
                        simtemp=memory[simindex++];
                        address=simtemp;
                        simtemp=memory[simindex++];
                        address+=(simtemp*256);
                        address-=startaddress;
                        if(S==1)
                            simindex=address;

                        break;
                    }
            case 251 :{//EI
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;


                        break;
                    }
            case 252 :{//CM
                        printf("terminating program. function not defined. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 253 :{//DO NOT EXIST
                        printf("terminating program. STRAY VALUE ENCOUNTERED.DNE. %d\n",simchoice);
                        simindex=-1;

                        break;
                    }
            case 254 :{//CPI Data
                        simtemp=memory[simindex++];
                        if(A==simtemp)
                        {
                            CY=0 ;
                            Z=1 ;
                        }
                        else if(A<simtemp)
                        {
                            CY=1 ;
                            Z= 0;
                        }
                        else if(A>simtemp)
                        {
                            CY=0 ;
                            Z=0 ;
                        }
                        break;
                    }
            case 255 :{//RST 7
                        printf("Code successfully terminated by RST 7 Interrupt\n");
                        simindex=-1;

                        break;
                    }
      
        }
        setparitysignA();
        if(debuggermode==1)
        {
            printf("%2d    %2X  ",i,displayop);
            displayregsnflag();
            //fflush(stdin);
            //char cha;
            //scanf("%c",&cha);
            if(i==1) getchar();
            getchar();
        }
    }while((simindex<memoryindex)&&simindex!=-1);
//ending sim-display results
    printf("Log: Displaying Final Status\n");
    displayheader();
    printf("End Out:  ");
    displayregsnflag();
    //printf("%X %X %X %X\n",A,B,C,D);
//Updating Data Segment

    FILE *dataseg2;
    dataseg2=fopen("datasegment.txt","w");
    for(int i=0;i<512;)
    {
        fprintf(dataseg2,"%X\t",i+refadd);
        for(int j=0;j<16;j++,i++)
        {
            if(fixedmemory[i]<16) fprintf(dataseg2,"0%X  ",fixedmemory[i]);
            else fprintf(dataseg2,"%X  ",fixedmemory[i]);
            
        }
        fprintf(dataseg2,"\n");
    }
    fclose(dataseg2);
    //*/
    printf("Log: Updating Memory File\n");
    printf("Log: Exiting Simulator\n");
    printf("Log: Terminating Session\n");
    return 0;
}
    






                       
