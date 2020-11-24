/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 38 "lex.y" /* yacc.c:339  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
-> A symbol table is maintained using hashing (with chaining)
        to keep track of identifiers and their scopes.
-> Each entry of this table is a pointer to a list of "Symbols"
-> A "Symbol" is a structure which stores name, datatype, scope
-> Scope is zero initially (global). On entry in a block, it is made +1
        and -1 on exit
-> A simple hash function takes var name as input
        and returns an index into the hashtable
-> For the obtained index, that symbol is "chained" to the corresponding list
-> When coming out of a block, all symbols declared in that scope are deleted
        from the symbol table
*/

/* Size of HashTable */
#define TABLE_SIZE 100
int yylex();
int yyerror(char* s);
int yylineno;
FILE* yyin;
int success=1;

FILE* interm_file;      /* File to store the generated intermediate code */
int interm_label=0;     // Keeps track of the label number for lines of intermediate code 
int new_temp_index=1;   // Used to create new unique temporary variable name for intermediate code   

int scope=0;    /* Tracks scope during parsing */ 
int dtype;      /* Stores datatype during parsing */

/** SYMBOL TABLE DS BEGINS **/

// Structure for a symbol's entry in a symbol table
typedef struct Symbol{
        char name[10]; // name of the identifier
        int dtype; // 0 for int; 1 for float
        int scope; // scope of this symbol
        struct Symbol* next;
} Symbol;

// Symbol table structure
typedef struct Symtable{
        Symbol* entry[TABLE_SIZE];
} Symtable;

// Hash function implementing (sn-1+16(sn-2+ .. + 16(s1+16s0)))%TABLE_SIZE
int hash(char* s){
        int hval=0;
        while((*s)!='\0'){
                hval=(hval<<4)+(*s);
                s++;
        }
        return hval%TABLE_SIZE;
}

// Global variable for symbol table
Symtable table;

// Initialize all pointers in the symbol table to NULL
void init(){ 
        for(int i=0;i<TABLE_SIZE;i++){
                table.entry[i]=NULL;
        }
}

// Function to add a new entry to symbol table
void add(char* symb){
        int hval=hash(symb);
        Symbol* head=table.entry[hval];
        if(head==NULL){ // No collision
                head=(Symbol*)malloc(sizeof(Symbol));
                strcpy(head->name,symb);
                head->dtype=dtype;
                head->scope=scope;
                head->next=NULL;
                table.entry[hval]=head;
                return;
        }
        // For collision, add this new symbol to the end of list
        while(head->next!=NULL){
                head=head->next;
        }
        head->next=(Symbol*)malloc(sizeof(Symbol));
        strcpy(head->name,symb);
        head->dtype=dtype;
        head->scope=scope;
        head->next=NULL;
}

// Function to check if the variable is being redefined in same scope
// returns 1 for redeclaration, 2 for conflicting types, otherwise 0
int existsInSameScope(char* symb){
        int hval=hash(symb);
        Symbol* head=table.entry[hval];
        if(head==NULL){
                return 0;
        }
        while(head!=NULL){
                // If the symbol names match and
                //      the current scope==its scope
                // then it implies that this identifier is
                //      either being redeclared (if datatypes are same)
                //      or, conflicting types found (if datatypes differ)
                if(strcmp(head->name,symb)==0 && head->scope==scope){
                        if(dtype==head->dtype){ // redeclaration
                                return 1;
                        } else{ // conflicting types
                                return 2;
                        }
                }
                head=head->next;
        }
        return 0;
}

// Function to check if hashtable contains a particular symbol
int contains(char* symb){
        int hval=hash(symb);
        Symbol* head=table.entry[hval];
        if(head==NULL){
                return 0;
        }
        while(head!=NULL){
                // If the symbol names match
                // then it implies that this identifier is already present
                if(strcmp(head->name,symb)==0){
                        return 1;
                }
                head=head->next;
        }
        return 0;
}

// Function to get the data type of a symbol present in symbol table
int getDtype(char* symb){
        int hval=hash(symb);
        Symbol* head=table.entry[hval];
        while(head!=NULL){
                if(strcmp(head->name,symb)==0){
                        return head->dtype;
                }
                head=head->next;
        }
        // not needed since the symbol is known to exist in the table
        return 0;
}

// Function to delete symbols of a scope when coming out of that scope
void deleteSymbolsOfScope(int scope){
        for(int i=0;i<TABLE_SIZE;i++){
                Symbol* head=table.entry[i];
                Symbol* temp;
                if(head==NULL){
                        continue;
                }
                // Delete "matching scope" symbols at the start of the list 
                while(head!=NULL && head->scope==scope){
                        temp=head;
                        head=head->next;
                        free(temp);
                        table.entry[i]=head;
                }
                // Look for the required symbols using the previous pointer
                while(head!=NULL && head->next!=NULL){
                        if(head->next->scope==scope){ // Delete the next symbol
                                temp=head->next;
                                head->next=temp->next;
                                free(temp);
                        } else{ // Move the pointer forward
                                head=head->next;
                        }
                }
        }
}

// Helper function to print symbols in symbol table
void printSymbolTable(){
        for(int i=0;i<TABLE_SIZE;i++){
                if(table.entry[i]==NULL){
                        continue;
                }
                printf("\nPrinting at index %d\n",i);
                Symbol* head=table.entry[i];
                while(head!=NULL){
                        printf("%s %d %d\n",head->name,head->dtype,head->scope);
                        head=head->next;
                }
        }
}

// Free all the DMA space of hashtable
void freeAll(){
        for(int i=0;i<TABLE_SIZE;i++){
                Symbol* head=table.entry[i];
                Symbol* temp;
                while(head!=NULL){
                        temp=head;
                        head=head->next;
                        free(temp);
                }
        }
}

/** SYMBOL TABLE DS ENDS **/

/** FUNCTIONS FOR INTERMEDIATE CODE GENERATION **/

// Function to return name of a new temporary variable
char* newTemp(){
        static char t[3];
        sprintf(t,"t%d",new_temp_index);
        return t;
}

// Function to emit the quadruple to output file
void generate(char* str){
        fprintf(interm_file,"%s",str);
}


#line 291 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
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
#line 266 "lex.y" /* yacc.c:355  */

        int number;     // for integer constants 
        float decimal;  // for floating point numbers
        char* string;   // for name of identifiers
        struct DtypeName{      // for expressions
                int dtype;      // for datatype of expressions
                char* place;    // for "place" value of expression
        } DtypeName;

#line 417 "y.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 434 "y.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   149

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  39
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  38
/* YYNRULES -- Number of rules.  */
#define YYNRULES  81
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  148

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   293

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   335,   335,   337,   339,   341,   343,   347,   348,   350,
     351,   353,   355,   357,   359,   361,   363,   365,   367,   369,
     371,   384,   397,   399,   403,   405,   422,   426,   443,   460,
     477,   494,   511,   522,   528,   535,   545,   547,   549,   551,
     553,   555,   557,   559,   562,   564,   566,   569,   571,   573,
     575,   578,   580,   583,   584,   586,   592,   597,   599,   600,
     602,   604,   606,   613,   619,   621,   623,   624,   626,   628,
     630,   636,   638,   641,   642,   644,   646,   648,   649,   651,
     653,   655
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT_CONST_TOK", "FLOAT_CONST_TOK",
  "INT_TOK", "FLOAT_TOK", "MAIN_TOK", "LPAREN_TOK", "RPAREN_TOK",
  "LCURLY_TOK", "RCURLY_TOK", "LSQ_TOK", "RSQ_TOK", "PLUS_TOK",
  "MINUS_TOK", "ASTERISK_TOK", "QUOTIENT_TOK", "MODULUS_TOK", "AND_TOK",
  "OR_TOK", "NOT_TOK", "LT_EQ_TOK", "LT_TOK", "GT_EQ_TOK", "GT_TOK",
  "EQ_TOK", "NOT_EQ_TOK", "ASSIGN_TOK", "SEMICOLON_TOK", "COMMA_TOK",
  "IF_TOK", "ELSE_TOK", "FOR_TOK", "WHILE_TOK", "DO_TOK", "RETURN_TOK",
  "IDENTIFIER_TOK", "LOWER_THAN_ELSE", "$accept", "S", "MAIN_FN", "BLK",
  "BLK_START", "BLK_END", "BLKS", "STMTS", "STMT", "DECLARATION",
  "DATATYPE", "VARS", "ARRAY_OPTN", "ASSIGN", "EXPRESSION", "LOGIC_OP",
  "RELAT_OP", "COND", "ELSE_PART", "ITER", "INNER_BODY", "BODY_START",
  "INIT_PART", "CHANGE_PART", "COND_PART", "CONDITIONS", "CONDITION",
  "FN_CALL", "FN_SIGN", "OPTIONS", "PARAMS", "PARAM", "US_DEF_FNS",
  "US_DEF_FN", "FN_NAME", "OPTIONS_FORMAL", "PARAMS_FORMAL",
  "PARAM_FORMAL", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293
};
# endif

#define YYPACT_NINF -104

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-104)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      30,  -104,  -104,     9,    30,    12,  -104,   -26,  -104,    30,
      16,  -104,    18,  -104,    21,    30,    35,     3,    45,  -104,
      -2,  -104,  -104,  -104,  -104,    35,    30,    47,  -104,  -104,
      50,    56,    60,    35,     2,     4,  -104,    61,  -104,    40,
      34,    44,  -104,  -104,    48,  -104,     2,    37,     2,  -104,
      42,    54,  -104,  -104,     2,     2,  -104,    78,     0,    19,
      47,  -104,  -104,  -104,    67,  -104,  -104,  -104,    86,    77,
    -104,    23,    69,  -104,    70,    89,    97,  -104,   105,    -3,
       2,     2,     2,     2,     2,  -104,  -104,  -104,  -104,   106,
    -104,    76,   108,   110,  -104,  -104,    58,    87,  -104,  -104,
    -104,  -104,  -104,  -104,     2,    35,  -104,  -104,     2,     2,
       2,    35,     2,  -104,    -3,    -3,  -104,  -104,  -104,  -104,
       0,   116,   117,    34,   110,    99,  -104,   103,  -104,   109,
    -104,  -104,  -104,  -104,    35,  -104,    96,   107,  -104,    52,
     125,  -104,   121,   122,    35,  -104,  -104,  -104
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    18,    19,     0,    74,     0,     1,     0,     2,    74,
       0,    76,     0,    73,     0,    78,     0,     0,     0,    77,
      79,     5,     3,    10,    81,     0,     0,     8,    75,    80,
       0,     0,     0,    52,     0,     0,    10,     0,     9,     0,
       0,     0,    16,    15,     0,    64,    59,    54,    59,    51,
       0,     0,    33,    34,     0,     0,    35,     0,    67,     0,
       8,     6,     4,    12,    24,    17,    13,    14,     0,     0,
      58,    60,     0,    53,     0,     0,     0,    50,     0,    32,
       0,     0,     0,     0,     0,    11,    71,    72,    70,     0,
      66,    68,    35,    25,    63,     7,     0,    20,    38,    39,
      40,    41,    42,    43,     0,    52,    36,    37,     0,     0,
      59,    52,    59,    26,    27,    28,    29,    30,    31,    65,
       0,     0,     0,     0,    62,    44,    61,     0,    48,     0,
      69,    22,    23,    21,    52,    45,    57,     0,    46,     0,
       0,    49,     0,     0,    52,    55,    56,    47
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -104,  -104,  -104,    -9,  -104,  -104,    79,   102,    90,  -104,
      29,    17,  -104,    95,   -34,  -104,  -104,  -104,  -104,  -104,
    -103,  -104,  -104,  -104,   -47,    36,  -104,  -104,    84,  -104,
      25,  -104,   137,  -104,  -104,  -104,   123,  -104
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     3,     4,    49,    23,    62,    37,    27,    38,    39,
      40,    65,    97,    41,    68,   108,   104,    42,   135,    43,
      50,    51,    74,   140,    69,    70,    71,    44,    45,    89,
      90,    91,     8,     9,    12,    18,    19,    20
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      57,    75,   125,    86,    87,    52,    53,    22,   128,     6,
      54,    11,    58,    82,    83,    84,    28,    55,    36,    10,
      78,    79,    52,    53,    14,    93,    15,    54,    26,     5,
      16,   138,    59,     7,    55,     1,     2,    88,     7,    56,
      24,   147,   106,   107,    17,    21,   114,   115,   116,   117,
     118,    36,     1,     2,    25,    17,    92,    21,    46,     1,
       2,   121,   122,   127,    47,   129,   142,   143,    48,    63,
     124,    64,    61,    66,    72,    93,    76,    67,    30,    96,
      31,    32,    33,    34,    35,    30,   105,    31,    32,    33,
      34,    35,    80,    81,    82,    83,    84,   109,   111,   110,
      80,    81,    82,    83,    84,   112,   120,    85,    98,    99,
     100,   101,   102,   103,   113,   119,    58,   123,   137,    80,
      81,    82,    83,    84,    80,    81,    82,    83,    84,   131,
     132,   134,   136,   139,   144,   145,   141,   146,    60,    95,
     133,    77,    73,    94,   126,   130,    13,     0,     0,    29
};

static const yytype_int16 yycheck[] =
{
      34,    48,   105,     3,     4,     3,     4,    16,   111,     0,
       8,    37,     8,    16,    17,    18,    25,    15,    27,     7,
      54,    55,     3,     4,     8,    59,     8,     8,    30,     0,
       9,   134,    28,     4,    15,     5,     6,    37,     9,    37,
      37,   144,    19,    20,    15,    10,    80,    81,    82,    83,
      84,    60,     5,     6,     9,    26,    37,    10,     8,     5,
       6,     3,     4,   110,     8,   112,    14,    15,     8,    29,
     104,    37,    11,    29,    37,   109,    34,    29,    31,    12,
      33,    34,    35,    36,    37,    31,     9,    33,    34,    35,
      36,    37,    14,    15,    16,    17,    18,    28,     9,    29,
      14,    15,    16,    17,    18,     8,    30,    29,    22,    23,
      24,    25,    26,    27,     9,     9,     8,    30,     9,    14,
      15,    16,    17,    18,    14,    15,    16,    17,    18,    13,
      13,    32,    29,    37,     9,    14,    29,    15,    36,    60,
     123,    51,    47,    59,   108,   120,     9,    -1,    -1,    26
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,     6,    40,    41,    49,     0,    49,    71,    72,
       7,    37,    73,    71,     8,     8,     9,    49,    74,    75,
      76,    10,    42,    43,    37,     9,    30,    46,    42,    75,
      31,    33,    34,    35,    36,    37,    42,    45,    47,    48,
      49,    52,    56,    58,    66,    67,     8,     8,     8,    42,
      59,    60,     3,     4,     8,    15,    37,    53,     8,    28,
      46,    11,    44,    29,    37,    50,    29,    29,    53,    63,
      64,    65,    37,    52,    61,    63,    34,    47,    53,    53,
      14,    15,    16,    17,    18,    29,     3,     4,    37,    68,
      69,    70,    37,    53,    67,    45,    12,    51,    22,    23,
      24,    25,    26,    27,    55,     9,    19,    20,    54,    28,
      29,     9,     8,     9,    53,    53,    53,    53,    53,     9,
      30,     3,     4,    30,    53,    59,    64,    63,    59,    63,
      69,    13,    13,    50,    32,    57,    29,     9,    59,    37,
      62,    29,    14,    15,     9,    14,    15,    59
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    39,    40,    41,    42,    43,    44,    45,    45,    46,
      46,    47,    47,    47,    47,    47,    47,    48,    49,    49,
      50,    50,    51,    51,    51,    52,    53,    53,    53,    53,
      53,    53,    53,    53,    53,    53,    54,    54,    55,    55,
      55,    55,    55,    55,    56,    56,    57,    58,    58,    58,
      59,    59,    60,    61,    61,    62,    62,    62,    63,    63,
      64,    64,    65,    66,    66,    67,    68,    68,    69,    69,
      70,    70,    70,    71,    71,    72,    73,    74,    74,    75,
      75,    76
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     5,     4,     1,     1,     3,     0,     2,
       0,     3,     2,     2,     2,     1,     1,     2,     1,     1,
       2,     4,     3,     3,     0,     3,     3,     3,     3,     3,
       3,     3,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     5,     6,     2,     9,     5,     7,
       2,     1,     0,     1,     0,     3,     3,     0,     1,     0,
       1,     3,     3,     3,     1,     4,     1,     0,     1,     3,
       1,     1,     1,     2,     0,     6,     1,     1,     0,     1,
       3,     2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 337 "lex.y" /* yacc.c:1646  */
    {printf("\nSyntax of main function is correct");}
#line 1619 "y.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 341 "lex.y" /* yacc.c:1646  */
    {scope+=1;}
#line 1625 "y.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 343 "lex.y" /* yacc.c:1646  */
    { deleteSymbolsOfScope(scope); 
                                 scope-=1;
                                }
#line 1633 "y.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 353 "lex.y" /* yacc.c:1646  */
    {printf("\nSyntax of return statement is correct");}
#line 1639 "y.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 355 "lex.y" /* yacc.c:1646  */
    {printf("\nSyntax of declaration statement is correct");}
#line 1645 "y.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 357 "lex.y" /* yacc.c:1646  */
    {printf("\nSyntax of assignment statement is correct");}
#line 1651 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 359 "lex.y" /* yacc.c:1646  */
    {printf("\nSyntax of function call is correct");}
#line 1657 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 361 "lex.y" /* yacc.c:1646  */
    {printf("\nSyntax of iterative statement is correct");}
#line 1663 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 363 "lex.y" /* yacc.c:1646  */
    {printf("\nSyntax of conditional statement is correct");}
#line 1669 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 367 "lex.y" /* yacc.c:1646  */
    {dtype=0;}
#line 1675 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 369 "lex.y" /* yacc.c:1646  */
    {dtype=1;}
#line 1681 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 371 "lex.y" /* yacc.c:1646  */
    { int type = existsInSameScope((yyvsp[-1].string)); 
                                         if(type==1){
                                                char buff[50];
                                                sprintf(buff,": Multiple definitions of the identifier %s",(yyvsp[-1].string));
                                                yyerror(buff);
                                         } else if(type==2){
                                                char buff[50];
                                                sprintf(buff,": Conflicting types for the identifier %s",(yyvsp[-1].string));
                                                yyerror(buff);
                                         } else{
                                                add((yyvsp[-1].string));
                                        } }
#line 1698 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 384 "lex.y" /* yacc.c:1646  */
    { int type = existsInSameScope((yyvsp[-3].string)); 
                                                         if(type==1){
                                                                char buff[50];
                                                                sprintf(buff,": Multiple definitions of the identifier %s",(yyvsp[-3].string));
                                                                yyerror(buff);
                                                         } else if(type==2){
                                                                char buff[50];
                                                                sprintf(buff,": Conflicting types for the identifier %s",(yyvsp[-3].string));
                                                                yyerror(buff);
                                                         } else{
                                                                add((yyvsp[-3].string));
                                                        } }
#line 1715 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 399 "lex.y" /* yacc.c:1646  */
    { char buff[40];
                                                 sprintf(buff,": Incorrect Array Declaration");
                                                 yyerror(buff);
                                                }
#line 1724 "y.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 405 "lex.y" /* yacc.c:1646  */
    { if(!contains((yyvsp[-2].string))){
                                                        char buff[40];
                                                        sprintf(buff,": Undefined identifier %s",(yyvsp[-2].string));
                                                        yyerror(buff);
                                                 } else{
                                                        if(getDtype((yyvsp[-2].string))!=(yyvsp[0].DtypeName).dtype){
                                                                char buff[50];
                                                                sprintf(buff,": Illegal assignment (Type Error) detected");
                                                                yyerror(buff);
                                                        } else{ // no issues; let us generate the intermediate code
                                                                char buff[40];
                                                                sprintf(buff,"%d: %s = %s\n",interm_label,(yyvsp[-2].string),(yyvsp[0].DtypeName).place);
                                                                interm_label+=1;
                                                                generate(buff);
                                                        }
                                                } }
#line 1745 "y.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 422 "lex.y" /* yacc.c:1646  */
    { (yyval.DtypeName).dtype=(yyvsp[-1].DtypeName).dtype;
                                                 (yyval.DtypeName).place=strdup((yyvsp[-1].DtypeName).place);
                                                }
#line 1753 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 426 "lex.y" /* yacc.c:1646  */
    { if((yyvsp[-2].DtypeName).dtype!=(yyvsp[0].DtypeName).dtype){
                                                        char buff[40];
                                                        sprintf(buff,": Type error in expression");
                                                        yyerror(buff);
                                                 } else{
                                                        (yyval.DtypeName).dtype=(yyvsp[-2].DtypeName).dtype;
                                                        char* temp=newTemp();
                                                        new_temp_index+=1;
                                                        (yyval.DtypeName).place=strdup(temp);

                                                        char buff[40];
                                                        sprintf(buff,"%d: %s = %s + %s\n",interm_label,(yyval.DtypeName).place,(yyvsp[-2].DtypeName).place,(yyvsp[0].DtypeName).place);
                                                        interm_label+=1;
                                                        generate(buff);
                                                 }
                                                }
#line 1774 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 443 "lex.y" /* yacc.c:1646  */
    { (yyval.DtypeName).dtype=(yyvsp[-2].DtypeName).dtype; 
                                                 if((yyvsp[-2].DtypeName).dtype!=(yyvsp[0].DtypeName).dtype){
                                                        char buff[40];
                                                        sprintf(buff,": Type error in expression");
                                                        yyerror(buff);
                                                 } else{
                                                        char* temp=newTemp();
                                                        new_temp_index+=1;
                                                        (yyval.DtypeName).place=strdup(temp);

                                                        char buff[40];
                                                        sprintf(buff,"%d: %s = %s - %s\n",interm_label,(yyval.DtypeName).place,(yyvsp[-2].DtypeName).place,(yyvsp[0].DtypeName).place);
                                                        interm_label+=1;
                                                        generate(buff);
                                                 }
                                                }
#line 1795 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 460 "lex.y" /* yacc.c:1646  */
    { (yyval.DtypeName).dtype=(yyvsp[-2].DtypeName).dtype; 
                                                 if((yyvsp[-2].DtypeName).dtype!=(yyvsp[0].DtypeName).dtype){
                                                        char buff[40];
                                                        sprintf(buff,": Type error in expression");
                                                        yyerror(buff);
                                                 } else{
                                                        char* temp=newTemp();
                                                        new_temp_index+=1;
                                                        (yyval.DtypeName).place=strdup(temp);

                                                        char buff[40];
                                                        sprintf(buff,"%d: %s = %s * %s\n",interm_label,(yyval.DtypeName).place,(yyvsp[-2].DtypeName).place,(yyvsp[0].DtypeName).place);
                                                        interm_label+=1;
                                                        generate(buff);
                                                 }
                                                }
#line 1816 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 477 "lex.y" /* yacc.c:1646  */
    { (yyval.DtypeName).dtype=(yyvsp[-2].DtypeName).dtype; 
                                                 if((yyvsp[-2].DtypeName).dtype!=(yyvsp[0].DtypeName).dtype){
                                                        char buff[40];
                                                        sprintf(buff,": Type error in expression");
                                                        yyerror(buff);
                                                 } else{
                                                        char* temp=newTemp();
                                                        new_temp_index+=1;
                                                        (yyval.DtypeName).place=strdup(temp);

                                                        char buff[40];
                                                        sprintf(buff,"%d: %s = %s / %s\n",interm_label,(yyval.DtypeName).place,(yyvsp[-2].DtypeName).place,(yyvsp[0].DtypeName).place);
                                                        interm_label+=1;
                                                        generate(buff);
                                                 }
                                                }
#line 1837 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 494 "lex.y" /* yacc.c:1646  */
    { (yyval.DtypeName).dtype=(yyvsp[-2].DtypeName).dtype;
                                                 if((yyvsp[-2].DtypeName).dtype!=(yyvsp[0].DtypeName).dtype){
                                                        char buff[40];
                                                        sprintf(buff,": Type error in expression");
                                                        yyerror(buff);
                                                 } else{
                                                        char* temp=newTemp();
                                                        new_temp_index+=1;
                                                        (yyval.DtypeName).place=strdup(temp);

                                                        char buff[40];
                                                        sprintf(buff,"%d: %s = %s %% %s\n",interm_label,(yyval.DtypeName).place,(yyvsp[-2].DtypeName).place,(yyvsp[0].DtypeName).place);
                                                        interm_label+=1;
                                                        generate(buff);
                                                 }
                                                }
#line 1858 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 511 "lex.y" /* yacc.c:1646  */
    { (yyval.DtypeName).dtype=(yyvsp[0].DtypeName).dtype;
                                         char* temp=newTemp();
                                         new_temp_index+=1;
                                         (yyval.DtypeName).place=strdup(temp);

                                         char buff[40];
                                         sprintf(buff,"%d: %s = - %s\n",interm_label,(yyval.DtypeName).place,(yyvsp[0].DtypeName).place);
                                         interm_label+=1;
                                         generate(buff);
                                        }
#line 1873 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 522 "lex.y" /* yacc.c:1646  */
    { (yyval.DtypeName).dtype=0; 
                                 char temp_place[10];
                                 sprintf(temp_place,"%d",(yyvsp[0].number));
                                 (yyval.DtypeName).place=strdup(temp_place);
                                }
#line 1883 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 528 "lex.y" /* yacc.c:1646  */
    { (yyval.DtypeName).dtype=1;
                                 char buff[40];
                                 sprintf(buff,"%d: %.2f = %s\n",interm_label,(yyvsp[0].decimal),(yyval.DtypeName).place);
                                 interm_label+=1;
                                 generate(buff);
                                }
#line 1894 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 535 "lex.y" /* yacc.c:1646  */
    { if(!contains((yyvsp[0].string))){
                                        char buff[40];
                                        sprintf(buff,": Undefined identifier %s",(yyvsp[0].string));
                                        yyerror(buff);
                                 } else{
                                        (yyval.DtypeName).dtype=getDtype((yyvsp[0].string));
                                        (yyval.DtypeName).place=strdup((yyvsp[0].string));
                                }}
#line 1907 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 575 "lex.y" /* yacc.c:1646  */
    { deleteSymbolsOfScope(scope);
                                 scope-=1;}
#line 1914 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 580 "lex.y" /* yacc.c:1646  */
    {scope+=1;}
#line 1920 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 586 "lex.y" /* yacc.c:1646  */
    { if(!contains((yyvsp[-2].string))){
                                                                char buff[40];
                                                                sprintf(buff,": Undefined identifier %s",(yyvsp[-2].string));
                                                                yyerror(buff);
                                                         } }
#line 1930 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 592 "lex.y" /* yacc.c:1646  */
    { if(!contains((yyvsp[-2].string))){
                                                                char buff[40];
                                                                sprintf(buff,": Undefined identifier %s",(yyvsp[-2].string));
                                                                yyerror(buff);
                                                         } }
#line 1940 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 606 "lex.y" /* yacc.c:1646  */
    { if((yyvsp[-2].DtypeName).dtype!=(yyvsp[0].DtypeName).dtype){
                                                        char buff[40];
                                                        sprintf(buff,": Type mismatch in operands");
                                                        yyerror(buff);  
                                                }}
#line 1950 "y.tab.c" /* yacc.c:1646  */
    break;

  case 63:
#line 613 "lex.y" /* yacc.c:1646  */
    { if(!contains((yyvsp[-2].string))){
                                                        char buff[40];
                                                        sprintf(buff,": Undefined identifier %s",(yyvsp[-2].string));
                                                        yyerror(buff);
                                                 } }
#line 1960 "y.tab.c" /* yacc.c:1646  */
    break;

  case 70:
#line 630 "lex.y" /* yacc.c:1646  */
    { if(!contains((yyvsp[0].string))){
                                char buff[40];
                                sprintf(buff,": Undefined identifier %s",(yyvsp[0].string));
                                yyerror(buff);
                         } }
#line 1970 "y.tab.c" /* yacc.c:1646  */
    break;

  case 75:
#line 644 "lex.y" /* yacc.c:1646  */
    {printf("\nSyntax of user defined function is correct");}
#line 1976 "y.tab.c" /* yacc.c:1646  */
    break;

  case 81:
#line 655 "lex.y" /* yacc.c:1646  */
    { int type = existsInSameScope((yyvsp[0].string)); 
                                         if(type==1){
                                                char buff[50];
                                                sprintf(buff,": Multiple definitions of the identifier %s",(yyvsp[0].string));
                                                yyerror(buff);
                                         } else if(type==2){
                                                char buff[50];
                                                sprintf(buff,": Conflicting types for the identifier %s",(yyvsp[0].string));
                                                yyerror(buff);
                                         } else{
                                                add((yyvsp[0].string));
                                        } }
#line 1993 "y.tab.c" /* yacc.c:1646  */
    break;


#line 1997 "y.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 667 "lex.y" /* yacc.c:1906  */


/* SECTION D - C FUNCTIONS */
int main(){
    yyin=fopen("source_file_lex1.c","r");
    interm_file=fopen("intermediate_code.txt","w");
    init();
    if(yyparse()==0){
        printf("\nParsed successfully.\n");
    }
    fclose(yyin);
    fclose(interm_file);
    freeAll();
    return 0;
}

// Called to display error message along with the corresponding line number
int yyerror(char* msg){
    extern int yylineno;
    printf("\nParsing failed at line %d %s\n",yylineno,msg);
    success=0;
    return 0;
}
