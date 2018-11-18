#ifndef MAIN_H__
#define MAIN_H__
#pragma once

#ifndef __POCC__
#define _CRT_SECURE_NO_WARNINGS 1
#define restrict
#endif

/*
 * These are the sources (including headers) to the LEMON LALR(1) parser generator. 
 * The sources have been combined into a single file to make it easy to include
 * LEMON in the source tree and Makefile of another program.
 *
 * The author of this program disclaims copyright.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

#ifndef __WIN32__
#   if defined(_WIN32) || defined(WIN32)
#       define __WIN32__
#   endif
#endif

extern int     nDefine;	/* Number of -D options on the command line */
extern char **azDefine;	/* Name of the -D macros */
extern char  *outputDir;
extern int    showPrecedenceConflict;
extern char  *user_templatename;

#define PRIVATE static

#ifdef TEST
#define MAXRHS 5       /* Set low to exercise exception code */
#else
#define MAXRHS 1000
#endif

char *msort(char*,char**,int(*)(const char*,const char*));
void  ErrorMsg(const char *, int,const char *, ...);

#define RES_TEMPLATE	8001

/*
 * Compilers are getting increasingly pedantic about type conversions
 * as C evolves ever closer to Ada....
 * To work around the latest problems we have to define the following
 * variant of strlen().
 */
#define lemonStrlen(X)   ((int)strlen(X))

#define ISUPPER(X) isupper((unsigned char)(X))
#define ISLOWER(X) islower((unsigned char)(X))
#define ISSPACE(X) isspace((unsigned char)(X))
#define ISDIGIT(X) isdigit((unsigned char)(X))
#define ISALNUM(X) isalnum((unsigned char)(X))
#define ISALPHA(X) isalpha((unsigned char)(X))

#define MemoryCheck(X)	if((X)==0)						\
						{								\
							extern void memory_error();	\
  							memory_error();				\
						}

#endif	//MAIN_H__
