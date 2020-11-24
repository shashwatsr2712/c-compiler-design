#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "token_id.h"


// Global vars
FILE* yyin; // Pointer to input source file in C
char* yytok; // To store token
int error; // Error flag 


// Utility to check if char is digit
int isDigit(char ch){
    if(ch>=48 && ch<=57){
        return 1;
    }
    return 0;
}


// Utility to check if character is a C specific symbol
int isSpecialCharacter(char x){
    if(x=='('){
        return LPAREN_TOK;    
    }
    if(x==')'){
        return RPAREN_TOK;
    }
    if(x=='{'){
        return LCURLY_TOK;
    }
    if(x=='}'){
        return RCURLY_TOK;
    }
    if(x=='+'){
        return PLUS_TOK;
    }
    if(x=='-'){
        return MINUS_TOK;
    }
    if(x=='*'){
        return ASTERISK_TOK;
    }
    if(x=='/'){
        return QUOTIENT_TOK;
    }
    if(x=='%'){
        return MOD_TOK;
    }
    if(x==';'){
        return SEMICOLON_TOK;
    }
    if(x=='='){
        return ASSIGN_TOK;
    }
    if(x=='<'){
        return LT_TOK;
    }
    if(x=='>'){
        return GT_TOK;
    }
    if(x=='&'){
        return B_AND_TOK;
    }
    if(x=='|'){
        return B_OR_TOK;
    }
    if(x==','){
        return COMMA_TOK;
    }
    if(x=='!'){
        return NOT_TOK;
    }
    return 0;
}


// Utility to check if char is delimiter
int isDelimiter(char ch){
    if(ch==' ' || ch=='\n' || ch=='\t'){
        return 1;
    }
    return 0;
}


// Function to parse and assign next token
// ID (as mentioned in token_id.h) is returned; 0 returned for EOF or comments
int yylex(){
    char ch=fgetc(yyin);
    if(ch==EOF){ // Reached EOF
        return 0;
    }

    // Keep skipping delimiters
    while(isDelimiter(ch)){
        // printf("Delimiter found\n");
        ch=fgetc(yyin);
    }
    
    int i=0;
    int x=0;
    
    if(x=isSpecialCharacter(ch)){
        // printf("Special char found %c\n",ch);
        yytok[i]=(char)x;
        i+=1;
        if(x=='&'){
            if(ch=fgetc(yyin)=='&'){ // Special case
                yytok[i]='&';
                i+=1;
                return AND_TOK;
            }
            fseek(yyin,-1,SEEK_CUR);
        }
        if(x=='|'){
            if((ch=fgetc(yyin))=='|'){ // Special case
                yytok[i]='|';
                i+=1;
                return OR_TOK;
            }
            fseek(yyin,-1,SEEK_CUR);
        }
        if(x=='='){
            if((ch=fgetc(yyin))=='='){ // Special case
                yytok[i]='=';
                i+=1;
                return EQ_TOK;
            }
            fseek(yyin,-1,SEEK_CUR);
        }
        if(x=='/'){
            if((ch=fgetc(yyin))=='/'){ // Handle comments beginning with //
                while(ch!='\n' && ch!=EOF){
                    ch=fgetc(yyin);
                } 
                return 0;
            }
            fseek(yyin,-1,SEEK_CUR);
            if((ch=fgetc(yyin))=='*'){ // Handle comment surrounded by /* and */
                ch=fgetc(yyin);
                do{
                    while(ch!='*'){
                        ch=fgetc(yyin);
                    }
                } while((ch=fgetc(yyin))!='/');
                return 0;
            }
            fseek(yyin,-1,SEEK_CUR);
        }
        return x; 
    }

    // A character enclosed within single quotes
    if(ch=='\''){
        ch=fgetc(yyin);
        yytok[i]=ch;
        i+=1;
        ch=fgetc(yyin);
        if(ch!='\''){
            error=1;
            return 0;
        }
        return 269;
    }

    // preprocessor directives (not required; why?)
    if(ch=='#'){
        while(ch!='\n' && ch!=EOF){
            ch=fgetc(yyin);
        }
        return 0;
    }

    // A string enclosed within double quotes
    if(ch=='\"'){
        while((ch=fgetc(yyin))!='\"'){
            yytok[i]=ch;
            i+=1;
        }
        return 269;
    }

    // Form string which could be an identifier or keyword
    while(!isDelimiter(ch) && !isSpecialCharacter(ch)){
        yytok[i]=ch;
        i+=1;
        // printf("%d %c\n",i-1,ch);
        ch=fgetc(yyin);
    }
    fseek(yyin,-1,SEEK_CUR); // Reverse file pointer by one character
    int len=(int)strlen(yytok);
    if(isDigit(yytok[0])){
        int i1=0;
        while(i1<len && isDigit(yytok[i1])){
            i1+=1;
        }
        if(i1<len && isDigit(yytok[i1])==0){
            // Flag error
            error=1;
            return 0;
        }
    }
    // printf("%s %d\n",yytok,len);
    if(strcmp(yytok,"if")==0){
        return IF_TOK;
    }
    if(strcmp(yytok,"else")==0){
        return ELSE_TOK;
    }
    if(strcmp(yytok,"for")==0){
        return FOR_TOK;
    }
    if(strcmp(yytok,"while")==0){
        return WHILE_TOK;
    }
    if(strcmp(yytok,"do")==0){
        return DO_TOK;
    }
    if(strcmp(yytok,"if")==0){
        return IF_TOK;
    }
    if(strcmp(yytok,"int")==0){
        return INT_TOK;
    }
    if(strcmp(yytok,"char")==0){
        return CHAR_TOK;
    }
    if(strcmp(yytok,"float")==0){
        return FLOAT_TOK;
    }
    if(strcmp(yytok,"return")==0){
        return RETURN_TOK;
    }

    // Token does not match any keyword above; hence it is an identifier/variable or a constant
    int i1=0;
    for(i1=0;i1<len;i1++){
        if(!isDigit(yytok[i1])){
            return 256; // An identifier
        }
    }
    return 269; // A constant
}


// Driver code
int main(int argc, char* argv[]){
    int token;
    error=0;
    if(argc==2){
        yyin=fopen(argv[1],"r");
        FILE* lexed_out=fopen("tokens.txt","w");
        fprintf(lexed_out,"TOKEN\tTOKEN_KEY\tTOKEN_LENGTH\n");
        fprintf(lexed_out,"===================================================\n\n");
        while(!feof(yyin)){
            yytok=malloc(50*sizeof(char));
            token=yylex();
            if(error==1){
                printf("Process Terminated due to error while parsing!\n");
                break;
            }
            if(token!=0){
                fprintf(lexed_out,"%-10s\t%d\t\t%d\n",yytok,token,(int)strlen(yytok));
            }
            free(yytok);
        }
        fclose(yyin);
        fclose(lexed_out);
    }
    return 0;
}