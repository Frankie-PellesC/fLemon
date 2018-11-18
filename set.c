/*+@@file@@----------------------------------------------------------------*//*!
 \file		set.c
 \par Description 
            Set manipulation routines for the LEMON parser generator.
 \par  Status: 
            
 \par Project: 
            Lemon parser
 \date		Created  on Sat Sep  1 22:13:59 2018
 \date		Modified on Sat Sep  1 22:13:59 2018
 \author	
\*//*-@@file@@----------------------------------------------------------------*/
#include "main.h"

static int size = 0;

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		SetSize
 \details	Set the set size
 \date		Created  on Sat Sep  1 22:14:38 2018
 \date		Modified on Sat Sep  1 22:14:38 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void SetSize(int n)
{
	size = n + 1;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		SetNew
 \details	Allocate a new set
 \date		Created  on Sat Sep  1 22:14:49 2018
 \date		Modified on Sat Sep  1 22:14:49 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
char *SetNew(void)
{
	char *s;

	s = (char *)calloc(size, 1);

	if (s == 0)
	{
		extern void memory_error();
		memory_error();
	}

	return s;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		SetFree
 \details	Deallocate a set
 \date		Created  on Sat Sep  1 22:15:23 2018
 \date		Modified on Sat Sep  1 22:15:23 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void SetFree(char *s)
{
	free(s);
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		SetAdd
 \details	Add a new element to the set.
 \return	Return TRUE if the element was added and FALSE if it was 
            already there.
 \date		Created  on Sat Sep  1 22:16:06 2018
 \date		Modified on Sat Sep  1 22:16:06 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
int SetAdd(char *s, int e)
{
	int rv;

	assert(e >= 0 && e < size);

	rv   = s[e];
	s[e] = 1;

	return !rv;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		SetUnion
 \details	Add every element of s2 to s1.
 \return	Return TRUE if s1 changes.
 \date		Created  on Sat Sep  1 22:17:03 2018
 \date		Modified on Sat Sep  1 22:17:03 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
int SetUnion(char *s1, char *s2)
{
	int i, progress;

	progress = 0;

	for (i = 0; i < size; i++)
	{
		if (s2[i] == 0)
			continue;

		if (s1[i] == 0)
		{
			progress = 1;
			s1[i] = 1;
		}
	}

	return progress;
}
