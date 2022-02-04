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

#ifndef YY_YY_SCOUTPATTERNPARSER_HH_INCLUDED
# define YY_YY_SCOUTPATTERNPARSER_HH_INCLUDED
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
    STRING = 258,
    TEXT = 259,
    CALLBACKS = 260,
    CLASS = 261,
    CLEANUP = 262,
    CONDITION = 263,
    DATA = 264,
    DESCR = 265,
    DOCNAME = 266,
    DIAGNOSIS = 267,
    HIDDEN = 268,
    INCLUDE = 269,
    INFO = 270,
    INIT = 271,
    MODE = 272,
    NAME = 273,
    NODOCS = 274,
    PARENT = 275,
    PATTERN = 276,
    PROLOG = 277,
    STATICINIT = 278,
    TYPE = 279,
    UNIT = 280
  };
#endif
/* Tokens.  */
#define STRING 258
#define TEXT 259
#define CALLBACKS 260
#define CLASS 261
#define CLEANUP 262
#define CONDITION 263
#define DATA 264
#define DESCR 265
#define DOCNAME 266
#define DIAGNOSIS 267
#define HIDDEN 268
#define INCLUDE 269
#define INFO 270
#define INIT 271
#define MODE 272
#define NAME 273
#define NODOCS 274
#define PARENT 275
#define PATTERN 276
#define PROLOG 277
#define STATICINIT 278
#define TYPE 279
#define UNIT 280

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SCOUTPATTERNPARSER_HH_INCLUDED  */
