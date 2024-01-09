/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_MARK_CODE_BOSQUECORE_SRC_BSQON_PARSER_LB_BSQON_TAB_H_INCLUDED
# define YY_YY_HOME_MARK_CODE_BOSQUECORE_SRC_BSQON_PARSER_LB_BSQON_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    SYM_BAR = 258,                 /* SYM_BAR  */
    SYM_AMP = 260,                 /* SYM_AMP  */
    SYM_COLON = 262,               /* ":"  */
    SYM_COMMA = 263,               /* ","  */
    KW_NONE = 264,                 /* "none"  */
    KW_NOTHING = 265,              /* "nothing"  */
    KW_TRUE = 266,                 /* "true"  */
    KW_FALSE = 267,                /* "false"  */
    KW_SOMETHING = 268,            /* "something"  */
    KW_OK = 269,                   /* "ok"  */
    KW_ERR = 270,                  /* "err"  */
    SYM_DOUBLE_COLON = 271,        /* "::"  */
    SYM_ENTRY = 272,               /* SYM_ENTRY  */
    SYM_BANG = 273,                /* SYM_BANG  */
    SYM_EQUALS = 274,              /* SYM_EQUALS  */
    SYM_DOT = 275,                 /* SYM_DOT  */
    SYM_AT = 276,                  /* SYM_AT  */
    SYM_UNDERSCORE = 277,          /* SYM_UNDERSCORE  */
    KW_SOME = 278,                 /* KW_SOME  */
    KW_SRC = 279,                  /* KW_SRC  */
    KW_LET = 280,                  /* KW_LET  */
    KW_IN = 281,                   /* KW_IN  */
    TOKEN_NAT = 282,               /* TOKEN_NAT  */
    TOKEN_INT = 283,               /* TOKEN_INT  */
    TOKEN_BIG_NAT = 284,           /* TOKEN_BIG_NAT  */
    TOKEN_BIG_INT = 285,           /* TOKEN_BIG_INT  */
    TOKEN_RATIONAL = 286,          /* TOKEN_RATIONAL  */
    TOKEN_FLOAT = 287,             /* TOKEN_FLOAT  */
    TOKEN_DECIMAL = 288,           /* TOKEN_DECIMAL  */
    TOKEN_DECIMAL_DEGREE = 289,    /* TOKEN_DECIMAL_DEGREE  */
    TOKEN_COMPLEX = 290,           /* TOKEN_COMPLEX  */
    TOKEN_LAT_LONG = 291,          /* TOKEN_LAT_LONG  */
    TOKEN_NUMBERINO = 292,         /* "numberino"  */
    TOKEN_BYTE_BUFFER = 293,       /* TOKEN_BYTE_BUFFER  */
    TOKEN_UUID_V4 = 294,           /* TOKEN_UUID_V4  */
    TOKEN_UUID_V7 = 295,           /* TOKEN_UUID_V7  */
    TOKEN_SHA_HASH = 296,          /* TOKEN_SHA_HASH  */
    TOKEN_STRING = 297,            /* TOKEN_STRING  */
    TOKEN_ASCII_STRING = 298,      /* TOKEN_ASCII_STRING  */
    TOKEN_REGEX = 299,             /* TOKEN_REGEX  */
    TOKEN_PATH_ITEM = 300,         /* TOKEN_PATH_ITEM  */
    TOKEN_DATE_TIME = 301,         /* TOKEN_DATE_TIME  */
    TOKEN_UTC_DATE_TIME = 302,     /* TOKEN_UTC_DATE_TIME  */
    TOKEN_PLAIN_DATE = 303,        /* TOKEN_PLAIN_DATE  */
    TOKEN_PLAIN_TIME = 304,        /* TOKEN_PLAIN_TIME  */
    TOKEN_LOGICAL_TIME = 305,      /* TOKEN_LOGICAL_TIME  */
    TOKEN_TICK_TIME = 306,         /* TOKEN_TICK_TIME  */
    TOKEN_TIMESTAMP = 307,         /* TOKEN_TIMESTAMP  */
    TOKEN_IDENTIFIER = 308,        /* "identifier"  */
    TOKEN_TYPE_COMPONENT = 309,    /* "type name"  */
    TOKEN_UNSPEC_IDENTIFIER = 310  /* "unspec identifier"  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 30 "/home/mark/Code/BosqueCore/src/bsqon/parser/lb/bsqon.y"

   struct BSQON_TYPE_AST_List* bsqon_t_list;
   struct BSQON_TYPE_AST_NamedListEntry* bsqon_t_nametypel_entry;
   struct BSQON_TYPE_AST_NamedList* bsqon_t_namedlist;
   struct BSQON_TYPE_AST_Node* bsqon_t;

   struct BSQON_AST_NamedListEntry* bsqon_nameval_entry;
   struct BSQON_AST_List* bsqon_list;
   struct BSQON_AST_NamedList* bsqon_namedlist;

   struct BSQON_AST_Node* bsqon;
   struct ByteString* bstr;
   char* str;

#line 132 "/home/mark/Code/BosqueCore/src/bsqon/parser/lb/bsqon.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_HOME_MARK_CODE_BOSQUECORE_SRC_BSQON_PARSER_LB_BSQON_TAB_H_INCLUDED  */
