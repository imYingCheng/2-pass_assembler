#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#define bufSize 20
#define size 100
char directiveBuf[size][bufSize];       //store the directive value in opcode.txt
char opcodeBuf[size][bufSize];          //store the opcode value in opcode.txt
int opEnd;                              
char labelBuf[size][bufSize];           //store the label value in location.txt
int locCounter[size];                   //store the location value int location.txt
int locEnd;
int lastLoc;                            //store the last location value
int startLoc;                           //store the first locaiton value
int countObProgram = 0;
int countLength = 0;
char eachStartLoc[6];
char stringObProgram[60]="\0";
bool startAddrModify = false;
void pass2();
void pass1();
int main(){
    pass1();
    pass2();
}
void pass1(){
    char label[bufSize];
    char directive[bufSize];
    char buffer[bufSize];
    FILE *sFile;
    sFile=fopen("source.txt","r");
    FILE *locFile;
    locFile = fopen("location.txt", "w");
    FILE *symbolFile;
    symbolFile = fopen("symbol_table.txt", "w");
    if(sFile==NULL)
        perror("Cannot find source.txt");
    else{
        int loc;
        char c;
        bool start = false;
        while(!feof(sFile)){
            c = fgetc(sFile);           //to ensure whether the label value is null
            if(c!='\t'){
                ungetc(c,sFile);        //put back the char just read
                fscanf(sFile, "%s", label);     //read label
            }
            else{
                memset(label, '\0', bufSize);   //the label is null
            }
            fscanf(sFile, "%s", directive);     //read directive
            c = fgetc(sFile);           //to ensure whether the value is null
            if(c!='\n'){
                ungetc(c,sFile);        //put back the char just read
                fscanf(sFile, "%s", buffer);
                c = fgetc(sFile);       //read '\n'
            }
            else{
                memset(buffer, '\0', bufSize);  //the value is null
            }
            if(start&&strcmp(directive,"END"))  //if the directive is END, fprintf to location.txt
                fprintf(locFile, "%X\t%s\t%s\t%s\n", loc, label, directive, buffer);
            if(start&&label[0]!='\0')   //if the label is not null, fprintf to symbol_table.txt
                fprintf(symbolFile, "%s\t%X\n", label, loc);
            if(!strcmp(directive,"START")){
                loc=strtol(buffer,NULL,16);     //change decimal to hexdecimal
                startLoc = loc;                 //store the first location value
                fprintf(locFile, "%X\t%s\t%s\t%s\n", loc, label, directive, buffer);
                start = true;
            }
            else if(!strcmp(directive,"BYTE")){
                int i = -1, record = 0;
                char type=buffer[0];
                while (buffer[++i] != '\'');
                while (buffer[++i] != '\'')
                    record++;                   //store the length of the BYTE
                if(type=='X')
                    record /= 2;
                loc += record;
            }
            else if(!strcmp(directive,"RESW")){
                int tmp = atoi(buffer);         //buffer stores the number of reserved words
                tmp *= 3;                       //one word equals to three bytes
                loc += tmp;
            }
            else if(!strcmp(directive,"RESB")){
                int tmp = atoi(buffer);         //buffer stores the number of reserved bytes
                loc += tmp;
            }
            else if(!strcmp(directive,"END")){
                lastLoc = loc;                  //store the last locaiton value
                fprintf(locFile, "\t%s\t%s\t%s", label, directive, buffer);
                break;
            }
            else{
                loc += 3;
            }
        }
    }
    fclose(sFile);
    fclose(locFile);
    fclose(symbolFile);
}
void pass2(){
    bool start = false;
    FILE *symbolFile;
    symbolFile = fopen("symbol_table.txt", "r");
    int i = 0;
    while(!feof(symbolFile)){
        fscanf(symbolFile, "%s\t%X\n", labelBuf[i], &locCounter[i++]);      //store the value in symbol_table.txt
    }
    locEnd = i;
    fclose(symbolFile);
    FILE *opFile;
    opFile=fopen("opcode.txt","r");
    i = 0;
    while(!feof(opFile))
        fscanf(opFile, "%s %s\n", directiveBuf[i], opcodeBuf[i++]);         //store the value int opcode.txt
    opEnd = i;
    fclose(opFile);
    FILE *locFile;
    locFile = fopen("location.txt", "r");
    FILE *sFile;
    sFile = fopen("source_program.txt", "w");
    FILE *obFile;
    obFile = fopen("object_program.txt", "w");
    char label[bufSize];
    char directive[bufSize];
    char buffer[bufSize];
    char loc[bufSize];
    char obCode[6];
    char c;
    while (true){
        memset(obCode, '\0', 6);                //clear the value stored in obCode
        c = fgetc(locFile);                     //check if the location value is null
        if(c!='\t'){
            ungetc(c,locFile);                  //put back the char just read
            fscanf(locFile, "%s", loc);         //read the location value
            c = fgetc(locFile);                 //read '\t'
        }
        else{
            memset(loc, '\0', bufSize);         //the locaiton value is null
        }
        c = fgetc(locFile);                     //check if the label value is null
        if(c!='\t'){
            ungetc(c,locFile);                  //put back the char just read
            fscanf(locFile, "%s", label);       //read the label
        }
        else{
            memset(label, '\0', bufSize);       //the label value is null
        }
        fscanf(locFile, "%s", directive);       //read the directive
        c = fgetc(locFile);                     //read the '\t'
        c = fgetc(locFile);                     //check if the value is null
        if(c!='\n'){
            ungetc(c,locFile);                  //put back the char just read
            fscanf(locFile, "%s", buffer);
            c = fgetc(locFile);                 //read '\n'
        }
        else{
            memset(buffer, '\0', bufSize);      //the value is null
        }
        if(!strcmp(directive, "BYTE")){
            int j = -1;
            if(buffer[0]=='C'){
                int i = 0;
                while (buffer[++j] != '\'');
                while (buffer[++j] != '\''){
                    char tmp[2];
                    itoa((int)buffer[j], tmp, 16); //change (int)buffer[j] to hexdecimal and store in tmp
                    obCode[i++] = tmp[0];           //store the opCode in obCode
                    obCode[i++] = tmp[1];
                }
            }
            else if(buffer[0]=='X'){
                int tmp = 0;
                while (buffer[++j] != '\'');        //directly store the value in ' ' as obCode
                while (buffer[++j] != '\''){
                    obCode[tmp++]=buffer[j];
                }
            }
        }
        else if(!strcmp(directive, "WORD")){
            int obCodeTmp = 6;
            int bufferSize = (int)(strlen(buffer));
            char tmp[bufferSize];
            itoa(atoi(buffer),tmp , 16);
            for (i = bufferSize-1; i >= 0; i--){
                obCode[--obCodeTmp] = tmp[i];
            }
            for (i = obCodeTmp-1; i >= 0;i--)
                obCode[i] = '0';
        }
        else if(!strcmp(directive, "RESB")||!strcmp(directive, "RESW")||!strcmp(directive, "START")||!strcmp(directive, "END")){
            obCode[0] = '\0';
        }
        else{
            for (i = 0; i < opEnd;i++){
                if(!strcmp(directive,directiveBuf[i]))//find the correspond opCode
                    break;
            }
            strcat(obCode, opcodeBuf[i]);
            if (buffer[strlen(buffer) - 2] == ',' && buffer[strlen(buffer) - 1] == 'X'){//modify the obCode
                int tmpSize = (int)(strlen(buffer) - 2);
                char tmp[tmpSize];
                for (i = 0; i < tmpSize; i++){
                    tmp[i] = buffer[i];
                }
                for (i = 0; i < locEnd; i++){
                    if(!strcmp(tmp,labelBuf[i]))
                        break;
                }
                char str[6];
                itoa(locCounter[i]+4096*8, str, 16);        //plus 1 in x field in object code
                strcat(obCode, str);
            }
            else if(buffer[0]=='\0'){           //for the RSUB directive, complete the rest obCode as '0'
                for (i = 2; i < 6;i++)
                    obCode[i] = '0';
            }
            else{
                for (i = 0; i < locEnd;i++){
                    if(!strcmp(buffer,labelBuf[i]))
                        break;
                }
                char str[6];
                itoa(locCounter[i] , str, 16);  //find the correspond location
                strcat(obCode, str);
            }
        }
        for (i = 0; i < strlen(obCode);i++){
            if((int)(obCode[i])>=97&&(int)(obCode[i])<=122)     //change lower case to upper case
                obCode[i] -= 32;
        }
        if((int)(strlen(buffer))>=8)
            fprintf(sFile, "%s\t%s\t%s\t%s\t%s\n", loc,label,directive,buffer,obCode);
        else
            fprintf(sFile, "%s\t%s\t%s\t%s\t\t%s\n", loc,label,directive,buffer,obCode);
                            //write in object_program.txt
        if(!start){           //write header record          
            start = true;
            fprintf(obFile, "%c%s\t%06X%06X\n", 'H', label, startLoc, (lastLoc - startLoc));
            memset(eachStartLoc, '0', 6);
            int tmp = (int)strlen(loc);
            for (i = tmp-1; i >=0 ;i--){
                eachStartLoc[2+i] = loc[i];
            }
        }
        else if(countLength+(strlen(obCode) / 2)>30||!strcmp(directive,"END")||!strcmp(directive,"RESW")||!strcmp(directive,"RESB")){
            //if the length of text record>69 or the directive is RESW or RESB, write text line.
            if(stringObProgram[0]!='\0'){                       //write text record
                fprintf(obFile, "%c%6s%02X%s\n", 'T', eachStartLoc, countLength, stringObProgram);  
            }
            memset(eachStartLoc, '0', 6);
            for (i = 3; i >=0 ;i--){
                eachStartLoc[2+i] = loc[i];
            }
            if(!strcmp(directive,"RESW"))                       //represent the starting address should be modified
                startAddrModify = true;
            else if(!strcmp(directive,"RESB"))
                startAddrModify = true;
            countLength = strlen(obCode) / 2;
            memset(stringObProgram, '\0', 60);
            strcat(stringObProgram, obCode);
        }
        else{
            if(startAddrModify){                                //modify the starting address
                startAddrModify = false;
                memset(eachStartLoc, '0', 6);
                for (i = 3; i >=0 ;i--){
                    eachStartLoc[2+i] = loc[i];
                }
            }
            countLength += strlen(obCode)/2;                    //count the length
            strcat(stringObProgram, obCode);
        }
        if(!strcmp(directive,"END")){
            fprintf(obFile, "%c%06X", 'E', startLoc);
            break;
        }
    }
    fclose(locFile);
    fclose(sFile);
    fclose(obFile);
}