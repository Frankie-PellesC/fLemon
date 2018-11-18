/*+@@file@@----------------------------------------------------------------*//*!
 \file		error.c
 \par Description 
            Code for printing error message.
 \par  Status: 
            
 \par Project: 
            Lemon parser
 \date		Created  on Sat Sep  1 18:47:46 2018
 \date		Modified on Sat Sep  1 18:47:46 2018
 \author	
\*//*-@@file@@----------------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>

void ErrorMsg(const char *filename, int lineno, const char *format, ...)
{
	va_list ap;
	fprintf(stderr, "%s:%d: ", filename, lineno);
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	fprintf(stderr, "\n");
}
