/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    INT_CONST_TOK = 258,
    FLOAT_CONST_TOK = 259,
    INT_TOK = 260,
    FLOAT_TOK = 261,
    MAIN_TOK = 262,
    LPAREN_TOK = 263,
    RPAREN_TOK = 264,
    LCURLY_TOK = 265,
    RCURLY_TOK = 266,
    LSQ_TOK = 267,
    RSQ_TOK = 268,
    PLUS_TOK = 269,
    MINUS_TOK = 270,
    ASTERISK_TOK = 271,
    QUOTIENT_TOK = 272,
    MODULUS_TOK = 273,
    AND_TOK = 274,
    OR_TOK = 275,
    NOT_TOK = 276,
    LT_EQ_TOK = 277,
    LT_TOK = 278,
    GT_EQ_TOK = 279,
    GT_TOK = 280,
    EQ_TOK = 281,
    NOT_EQ_TOK = 282,
    ASSIGN_TOK = 283,
    SEMICOLON_TOK = 284,
    COMMA_TOK = 285,
    IF_TOK = 286,
    ELSE_TOK = 287,
    FOR_TOK = 288,
    WHILE_TOK = 289,
    DO_TOK = 290,
    RETURN_TOK = 291,
    IDENTIFIER_TOK = 292,
    LOWER_THAN_ELSE = 293
  };
#endif
/* Tokens.  */
#define INT_CONST_TOK 258
#define FLOAT_CONST_TOK 259
#define INT_TOK 260
#define FLOAT_TOK 261
#define MAIN_TOK 262
#define LPAREN_TOK 263
#define RPAREN_TOK 264
#define LCURLY_TOK 265
#define RCURLY_TOK 266
#define LSQ_TOK 267
#define RSQ_TOK 268
#define PLUS_TOK 269
#define MINUS_TOK 270
#define ASTERISK_TOK 271
#define QUOTIENT_TOK 272
#define MODULUS_TOK 273
#define AND_TOK 274
#define OR_TOK 275
#define NOT_TOK 276
#define LT_EQ_TOK 277
#define LT_TOK 278
#define GT_EQ_TOK 279
#define GT_TOK 280
#define EQ_TOK 281
#define NOT_EQ_TOK 282
#define ASSIGN_TOK 283
#define SEMICOLON_TOK 284
#define COMMA_TOK 285
#define IF_TOK 286
#define ELSE_TOK 287
#define FOR_TOK 288
#define WHILE_TOK 289
#define DO_TOK 290
#define RETURN_TOK 291
#define IDENTIFIER_TOK 292
#define LOWER_THAN_ELSE 293

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 266 "lex.y" /* yacc.c:1909  */

        int number;     // for integer constants 
        float decimal;  // for floating point numbers
        char* string;   // for name of identifiers
        struct DtypeName{      // for expressions
                int dtype;      // for datatype of expressions
                char* place;    // for "place" value of expression
        } DtypeName;

#line 140 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
