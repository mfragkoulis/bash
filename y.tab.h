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
     IF = 258,
     THEN = 259,
     ELSE = 260,
     ELIF = 261,
     FI = 262,
     CASE = 263,
     ESAC = 264,
     FOR = 265,
     SELECT = 266,
     WHILE = 267,
     UNTIL = 268,
     DO = 269,
     DONE = 270,
     FUNCTION = 271,
     COPROC = 272,
     COND_START = 273,
     COND_END = 274,
     COND_ERROR = 275,
     IN = 276,
     BANG = 277,
     TIME = 278,
     TIMEOPT = 279,
     TIMEIGN = 280,
     SGSH_START = 281,
     SGSH_END = 282,
     WORD = 283,
     ASSIGNMENT_WORD = 284,
     REDIR_WORD = 285,
     NUMBER = 286,
     ARITH_CMD = 287,
     ARITH_FOR_EXPRS = 288,
     COND_CMD = 289,
     AND_AND = 290,
     OR_OR = 291,
     GREATER_GREATER = 292,
     LESS_LESS = 293,
     LESS_AND = 294,
     LESS_LESS_LESS = 295,
     GREATER_AND = 296,
     SEMI_SEMI = 297,
     SEMI_AND = 298,
     SEMI_SEMI_AND = 299,
     LESS_LESS_MINUS = 300,
     AND_GREATER = 301,
     AND_GREATER_GREATER = 302,
     LESS_GREATER = 303,
     GREATER_BAR = 304,
     BAR_AND = 305,
     yacc_EOF = 306
   };
#endif
/* Tokens.  */
#define IF 258
#define THEN 259
#define ELSE 260
#define ELIF 261
#define FI 262
#define CASE 263
#define ESAC 264
#define FOR 265
#define SELECT 266
#define WHILE 267
#define UNTIL 268
#define DO 269
#define DONE 270
#define FUNCTION 271
#define COPROC 272
#define COND_START 273
#define COND_END 274
#define COND_ERROR 275
#define IN 276
#define BANG 277
#define TIME 278
#define TIMEOPT 279
#define TIMEIGN 280
#define SGSH_START 281
#define SGSH_END 282
#define WORD 283
#define ASSIGNMENT_WORD 284
#define REDIR_WORD 285
#define NUMBER 286
#define ARITH_CMD 287
#define ARITH_FOR_EXPRS 288
#define COND_CMD 289
#define AND_AND 290
#define OR_OR 291
#define GREATER_GREATER 292
#define LESS_LESS 293
#define LESS_AND 294
#define LESS_LESS_LESS 295
#define GREATER_AND 296
#define SEMI_SEMI 297
#define SEMI_AND 298
#define SEMI_SEMI_AND 299
#define LESS_LESS_MINUS 300
#define AND_GREATER 301
#define AND_GREATER_GREATER 302
#define LESS_GREATER 303
#define GREATER_BAR 304
#define BAR_AND 305
#define yacc_EOF 306




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 329 "./parse.y"
{
  WORD_DESC *word;		/* the word that we read. */
  int number;			/* the number that we read. */
  WORD_LIST *word_list;
  COMMAND *command;
  REDIRECT *redirect;
  ELEMENT element;
  PATTERN_LIST *pattern;
}
/* Line 1529 of yacc.c.  */
#line 161 "y.tab.h"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

