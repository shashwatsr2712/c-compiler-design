/* SECTION A - VERBATIM CODE */
%option noinput nounput noyywrap
%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"
%}

/* SECTION B - DEFINITIONS */
DIGIT    [0-9]
ID       [a-zA-Z_][a-z0-9]*
COMMENT_UTIL    */

/* SECTION C - RULES */
%%

[ \t]+      /* skip delimiters */

\n          yylineno+=1;

"#"[^\n]*   /* skip preprocessor directives */

"("         return LPAREN_TOK;

")"         return RPAREN_TOK;

"{"         return LCURLY_TOK;

"}"         return RCURLY_TOK;

"["         return LSQ_TOK;

"]"         return RSQ_TOK;

";"         return SEMICOLON_TOK;

","         return COMMA_TOK;

"+"         return PLUS_TOK;

"-"         return MINUS_TOK;

"*"         return ASTERISK_TOK;

"//"[^\n]*        /* single line comments */

"/*"[^{COMMENT_UTIL}]*"*/"     /* multi line comments */

"/"         return QUOTIENT_TOK;

"%"         return MODULUS_TOK;

"<="        return LT_EQ_TOK;

"<"         return LT_TOK;

">="        return GT_EQ_TOK;

">"         return GT_TOK;

"=="        return EQ_TOK;

"="         return ASSIGN_TOK;

"&&"        return AND_TOK;

"||"        return OR_TOK;

"!="        return NOT_EQ_TOK;

"!"         return NOT_TOK;

{DIGIT}+{ID}+   {
                    fprintf(yyout,"Error encountered while parsing %s on line %d.\n",yytext,yylineno);
                    return 0;
                }

{DIGIT}+    {
                yylval.number=atoi(yytext);
                return INT_CONST_TOK;
            }

{DIGIT}+"."{DIGIT}*{ID}+    {
                                fprintf(yyout,"Error encountered while parsing %s on line %d.\n",yytext,yylineno);
                                return 0;
                            }

{DIGIT}+"."{DIGIT}*     {
                            yylval.decimal=atof(yytext);
                            return FLOAT_CONST_TOK;
                        }

"int"       return INT_TOK;

"float"     return FLOAT_TOK;

"for"       return FOR_TOK;

"while"     return WHILE_TOK;

"if"        return IF_TOK;

"else"      return ELSE_TOK;

"return"    return RETURN_TOK;

"main"      return MAIN_TOK;

{ID}+       {
                yylval.string=strdup(yytext);
                return IDENTIFIER_TOK;
            }

%%