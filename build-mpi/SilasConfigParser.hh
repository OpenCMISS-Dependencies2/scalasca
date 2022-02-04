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

#ifndef YY_YY_SILASCONFIGPARSER_HH_INCLUDED
# define YY_YY_SILASCONFIGPARSER_HH_INCLUDED
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
    SC_AGGREGATE_IDLE = 258,
    SC_BALANCE = 259,
    SC_IDLE_OVERHEAD = 260,
    SC_CUT = 261,
    SC_NOOP_THRESHOLD = 262,
    SC_OPTION = 263,
    SC_STRING = 264,
    SC_NUMBER = 265,
    SC_UNSIGNED = 266,
    SC_INTEGER = 267,
    SC_REAL = 268,
    SC_MESSAGE = 269,
    SC_MODEL = 270,
    SC_REGION = 271,
    SC_SCALE = 272,
    SC_SKIP_THRESHOLD = 273,
    SC_SIZE = 274,
    SC_TAG = 275,
    SC_RELATION = 276
  };
#endif
/* Tokens.  */
#define SC_AGGREGATE_IDLE 258
#define SC_BALANCE 259
#define SC_IDLE_OVERHEAD 260
#define SC_CUT 261
#define SC_NOOP_THRESHOLD 262
#define SC_OPTION 263
#define SC_STRING 264
#define SC_NUMBER 265
#define SC_UNSIGNED 266
#define SC_INTEGER 267
#define SC_REAL 268
#define SC_MESSAGE 269
#define SC_MODEL 270
#define SC_REGION 271
#define SC_SCALE 272
#define SC_SKIP_THRESHOLD 273
#define SC_SIZE 274
#define SC_TAG 275
#define SC_RELATION 276

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SILASCONFIGPARSER_HH_INCLUDED  */
