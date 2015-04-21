/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     T_Print = 258,
     T_ReadInteger = 259,
     T_Assign = 260,
     T_Assert = 261,
     T_LessEqual = 262,
     T_GreaterEqual = 263,
     T_Equal = 264,
     T_NotEqual = 265,
     T_QM = 266,
     T_Colon = 267,
     T_And = 268,
     T_Or = 269,
     T_If = 270,
     T_Else = 271,
     T_Mem = 272,
     T_ReadSecretInt = 273,
     T_Identifier = 274,
     T_StringConstant = 275,
     T_IntConstant = 276,
     T_UnaryMinus = 277,
     T_Lower_Than_Else = 278
   };
#endif
/* Tokens.  */
#define T_Print 258
#define T_ReadInteger 259
#define T_Assign 260
#define T_Assert 261
#define T_LessEqual 262
#define T_GreaterEqual 263
#define T_Equal 264
#define T_NotEqual 265
#define T_QM 266
#define T_Colon 267
#define T_And 268
#define T_Or 269
#define T_If 270
#define T_Else 271
#define T_Mem 272
#define T_ReadSecretInt 273
#define T_Identifier 274
#define T_StringConstant 275
#define T_IntConstant 276
#define T_UnaryMinus 277
#define T_Lower_Than_Else 278




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 33 "simple.y"
{
  char *stringConstant;
  unsigned integerConstant;
  char *identifier;
  ast_t * ast;
  ast_list_t *ast_list;
}
/* Line 1529 of yacc.c.  */
#line 103 "simple.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;
