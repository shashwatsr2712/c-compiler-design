#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "token_id.h"


// Global vars
FILE* yyin; // Pointer to input source file in C
char* yytok; // To store token
int error; // Error flag 
int yylineno; // To store line number


// Utility to check if char is digit
int isDigit(char ch){
    if(ch>=48 && ch<=57){
        return 1;
    }
    return 0;
}


// Utility to check if char is digit
int isLetter(char ch){
    if((ch>=65 && ch<=90) || (ch>=97 && ch<=122)){
        return 1;
    }
    return 0;
}


// Utility to check if char is delimiter
int isDelimiter(char ch){
    if(ch==' ' || ch=='\n' || ch=='\t'){
        if(ch=='\n'){
            yylineno+=1;
        }
        return 1;
    }
    return 0;
}


// Function to parse and assign next token
// ID (as mentioned in token_id.h) is returned; 0 returned for EOF or comments
int yylex(){
    int i=0;
    char ch;
    int state=0;
    while(1){
        // printf("%s %d\n",yytok,state);
        // Controlling states of Finite Automata
        switch(state){
            case 0:
            ch=fgetc(yyin);
            if(ch==EOF){
                return 0;
            }
            if(isDelimiter(ch)){
                state=2;
                break;
            } else if(ch=='|'){
                state=3;
            } else if(ch=='&'){
                state=6;
            } else if(ch=='<'){
                state=9;
            } else if(ch=='>'){
                state=12;
            } else if(ch=='='){
                state=15;
            } else if(ch=='!'){
                state=18;
            } else if(ch=='+'){
                state=21;
            } else if(ch=='-'){
                state=24;
            } else if(ch=='*'){
                state=27;
            } else if(ch=='%'){
                state=28;
            } else if(ch=='/'){
                state=29;
            } else if(ch=='('){
                state=36;
            } else if(ch==')'){
                state=37;
            } else if(ch=='{'){
                state=38;
            } else if(ch=='}'){
                state=39;
            } else if(isLetter(ch)){
                if(ch=='r'){
                    state=40;
                } else if(ch=='d'){
                    state=46;
                } else if(ch=='c'){
                    state=48;
                } else if(ch=='e'){
                    state=52;
                } else if(ch=='i'){
                    state=56;
                } else if(ch=='f'){
                    state=60;
                } else if(ch=='w'){
                    state=63;
                } else{
                    state=76;
                }
            } else if(isDigit(ch)){
                state=78;
            } else if(ch=='#'){
                state=84;
                break;
            } else if(ch==','){
                state=86;
            } else if(ch==';'){
                state=87;
            } else{
                // Scenarios not considered in this code
            }
            yytok[i]=ch;
            i+=1;    
            break;

            case 1:
            return 0;

            case 2:
            ch=fgetc(yyin);
            if(isDelimiter(ch)){
                state=2;
            } else if(ch==EOF){
                state=1;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=0;
            }
            break;

            case 3:
            ch=fgetc(yyin);
            if(ch=='|'){
                yytok[i]='|';
                i+=1;
                state=4;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=5;
            }
            break;

            case 4:
            return OR_TOK;

            case 5:
            return B_OR_TOK;

            case 6:
            ch=fgetc(yyin);
            if(ch=='&'){
                yytok[i]='&';
                i+=1;
                state=7;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=8;
            }
            break;

            case 7:
            return AND_TOK;

            case 8:
            return B_AND_TOK;

            case 9:
            ch=fgetc(yyin);
            if(ch=='='){
                yytok[i]='=';
                i+=1;
                state=10;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=11;
            }
            break;

            case 10:
            return LT_EQ_TOK;

            case 11:
            return LT_TOK;

            case 12:
            ch=fgetc(yyin);
            if(ch=='='){
                yytok[i]='=';
                i+=1;
                state=13;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=14;
            }
            break;

            case 13:
            return GT_EQ_TOK;

            case 14:
            return GT_TOK;

            case 15:
            ch=fgetc(yyin);
            if(ch=='='){
                yytok[i]='=';
                i+=1;
                state=16;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=17;
            }
            break;

            case 16:
            return EQ_TOK;

            case 17:
            return ASSIGN_TOK;

            case 18:
            ch=fgetc(yyin);
            if(ch=='='){
                yytok[i]='=';
                i+=1;
                state=19;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=20;
            }
            break;

            case 19:
            return NOT_EQ_TOK;

            case 20:
            return NOT_TOK;

            case 21:
            ch=fgetc(yyin);
            if(ch=='+'){
                yytok[i]='+';
                i+=1;
                state=22;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=23;
            }
            break;

            case 22:
            return INC_TOK;

            case 23:
            return PLUS_TOK;

            case 24:
            ch=fgetc(yyin);
            if(ch=='-'){
                yytok[i]='-';
                i+=1;
                state=25;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=26;
            }
            break;

            case 25:
            return DEC_TOK;

            case 26:
            return MINUS_TOK;

            case 27:
            return ASTERISK_TOK;

            case 28:
            return MOD_TOK;

            case 29:
            ch=fgetc(yyin);
            if(ch=='/'){
                state=34;
            } else if(ch=='*'){
                state=31;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=30;
            }
            break;

            case 30:
            return QUOTIENT_TOK;

            case 31:
            ch=fgetc(yyin);
            if(ch=='*'){
                state=32;
            } else{
                state=31;
            }
            break;

            case 32:
            ch=fgetc(yyin);
            if(ch=='/'){
                state=33;
            } else if(ch=='*'){
                state=32;
            } else{
                state=31;
            }
            break;

            case 33:
            return 0;

            case 34:
            ch=fgetc(yyin);
            if(ch=='\n'){
                yylineno+=1;
                state=35;
            } else{
                state=34;
            }
            break;

            case 35:
            return 0;

            case 36:
            return LPAREN_TOK;

            case 37:
            return RPAREN_TOK;

            case 38:
            return LCURLY_TOK;

            case 39:
            return RCURLY_TOK;

            case 40:
            ch=fgetc(yyin);
            if(ch=='e'){
                yytok[i]=ch;
                i+=1;
                state=41;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 41:
            ch=fgetc(yyin);
            if(ch=='t'){
                yytok[i]=ch;
                i+=1;
                state=42;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 42:
            ch=fgetc(yyin);
            if(ch=='u'){
                yytok[i]=ch;
                i+=1;
                state=43;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 43:
            ch=fgetc(yyin);
            if(ch=='r'){
                yytok[i]=ch;
                i+=1;
                state=44;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 44:
            ch=fgetc(yyin);
            if(ch=='n'){
                yytok[i]=ch;
                i+=1;
                state=45;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 45:
            ch=fgetc(yyin);
            if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=68;
            }
            break;

            case 46:
            ch=fgetc(yyin);
            if(ch=='o'){
                yytok[i]=ch;
                i+=1;
                state=47;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 47:
            ch=fgetc(yyin);
            if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=69;
            }
            break;

            case 48:
            ch=fgetc(yyin);
            if(ch=='h'){
                yytok[i]=ch;
                i+=1;
                state=49;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 49:
            ch=fgetc(yyin);
            if(ch=='a'){
                yytok[i]=ch;
                i+=1;
                state=50;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 50:
            ch=fgetc(yyin);
            if(ch=='r'){
                yytok[i]=ch;
                i+=1;
                state=51;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 51:
            ch=fgetc(yyin);
            if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=70;
            }
            break;

            case 52:
            ch=fgetc(yyin);
            if(ch=='l'){
                yytok[i]=ch;
                i+=1;
                state=53;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 53:
            ch=fgetc(yyin);
            if(ch=='s'){
                yytok[i]=ch;
                i+=1;
                state=54;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 54:
            ch=fgetc(yyin);
            if(ch=='e'){
                yytok[i]=ch;
                i+=1;
                state=55;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 55:
            ch=fgetc(yyin);
            if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=71;
            }
            break;

            case 56:
            ch=fgetc(yyin);
            if(ch=='f'){
                yytok[i]=ch;
                i+=1;
                state=57;
            } else if(ch=='n'){
                yytok[i]=ch;
                i+=1;
                state=58;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 57:
            ch=fgetc(yyin);
            if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=72;
            }
            break;

            case 58:
            ch=fgetc(yyin);
            if(ch=='t'){
                yytok[i]=ch;
                i+=1;
                state=59;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 59:
            ch=fgetc(yyin);
            if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=73;
            }
            break;

            case 60:
            ch=fgetc(yyin);
            if(ch=='o'){
                yytok[i]=ch;
                i+=1;
                state=61;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 61:
            ch=fgetc(yyin);
            if(ch=='r'){
                yytok[i]=ch;
                i+=1;
                state=62;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 62:
            ch=fgetc(yyin);
            if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=74;
            }
            break;

            case 63:
            ch=fgetc(yyin);
            if(ch=='h'){
                yytok[i]=ch;
                i+=1;
                state=64;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 64:
            ch=fgetc(yyin);
            if(ch=='i'){
                yytok[i]=ch;
                i+=1;
                state=65;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 65:
            ch=fgetc(yyin);
            if(ch=='l'){
                yytok[i]=ch;
                i+=1;
                state=66;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 66:
            ch=fgetc(yyin);
            if(ch=='e'){
                yytok[i]=ch;
                i+=1;
                state=67;
            } else if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 67:
            ch=fgetc(yyin);
            if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=75;
            }
            break;

            case 68:
            return RETURN_TOK;

            case 69:
            return DO_TOK;

            case 70:
            return CHAR_TOK;

            case 71:
            return ELSE_TOK;

            case 72:
            return IF_TOK;

            case 73:
            return INT_TOK;

            case 74:
            return FOR_TOK;

            case 75:
            return WHILE_TOK;

            case 76:
            ch=fgetc(yyin);
            if(isLetter(ch) || isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=76;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=77;
            }
            break;

            case 77:
            return IDENTIFIER;

            case 78:
            ch=fgetc(yyin);
            if(isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=78;
            } else if(isLetter(ch)){
                state=80;
            } else if(ch=='.'){
                yytok[i]=ch;
                i+=1;
                state=81;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=79;
            }
            break;

            case 79:
            return INT_CONST_TOK;

            case 80:
            error=1;
            return 0;

            case 81:
            ch=fgetc(yyin);
            if(isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=82;
            }
            break;

            case 82:
            ch=fgetc(yyin);
            if(isDigit(ch)){
                yytok[i]=ch;
                i+=1;
                state=82;
            } else{
                fseek(yyin,-1,SEEK_CUR);
                state=83;
            }
            break;

            case 83:
            return FLOAT_CONST_TOK;

            case 84:
            ch=fgetc(yyin);
            if(ch!='\n'){
                state=84;
            } else{
                yylineno+=1;
                state=85;
            }
            break;

            case 85:
            return 0;

            case 86:
            return COMMA_TOK;

            case 87:
            return SEMICOLON_TOK;
        }
    }
}


// Driver code
int main(int argc, char* argv[]){
    int token;
    error=0;
    if(argc==2){
        yylineno=1;
        yyin=fopen(argv[1],"r");
        FILE* lexed_out=fopen("tokens_fa.txt","w");
        fprintf(lexed_out,"TOKEN\tTOKEN_KEY\tTOKEN_LENGTH\tTOKEN_LINENO\n");
        fprintf(lexed_out,"===================================================\n\n");
        while(!feof(yyin)){
            yytok=malloc(50*sizeof(char));
            token=yylex();
            if(error==1){
                printf("Process Terminated due to error while parsing!\n");
                break;
            }
            if(token!=0){
                fprintf(lexed_out,"%-10s\t%d\t\t%d\t\t\t\t%d\n",yytok,token,(int)strlen(yytok),yylineno);
            }
            free(yytok);
        }
        fclose(yyin);
        fclose(lexed_out);
    }
    return 0;
}