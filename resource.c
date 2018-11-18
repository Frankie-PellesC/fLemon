#include "main.h"
#include <windows.h>
#include "resource.h"

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		ResHandleCreate
 \date		Created  on Sun Sep  2 16:30:58 2018
 \date		Modified on Sun Sep  2 16:30:58 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
RES_HANDLE ResHandleCreate(void)
{
	RES_HANDLE hTmplt = calloc(1, sizeof(RES_DATACTRL));
	if (!hTmplt)
	{
		printf("Error: out of memory!\n");
		exit(1);
	}
	return hTmplt;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		OpenTemplate
 \date		Created  on Sun Sep  2 16:31:02 2018
 \date		Modified on Sun Sep  2 16:31:02 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
RES_HANDLE OpenTemplate(void)
{
	HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(RES_TEMPLATE), MAKEINTRESOURCE(RT_RCDATA));
	if (NULL == hRsrc)
	{
		printf("Can't access template (%#x).\n", GetLastError());
		exit(1);
	}

	HGLOBAL hGlbl = LoadResource(NULL, hRsrc);
	if (NULL == hGlbl)
	{
		printf("Can't load template (%#x).\n", GetLastError());
		exit(1);
	}

	LPVOID pMem = LockResource(hGlbl);
	if (NULL == pMem)
	{
		printf("Can't access template memory (%#x).\n", GetLastError());
		exit(1);
	}

	RES_HANDLE hTmplt = ResHandleCreate();

	hTmplt->base      = pMem;
	hTmplt->p         = pMem;
	hTmplt->end       = (char *)pMem + SizeofResource(NULL, hRsrc);

	return hTmplt;
}

/*
 * PellesC bug.
 * compile the function without optimizations
 */
#ifdef __POCC__
#pragma optimize(none)
#endif
/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		res_fgets
 \date		Created  on Sun Sep  2 17:37:13 2018
 \date		Modified on Sun Sep  2 17:37:13 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
char * res_fgets(char * restrict dst, int max, RES_HANDLE restrict hTmplt)
{
	if (hTmplt->fp)
		return fgets(dst, max, hTmplt->fp);

	if (hTmplt->p >= hTmplt->end)
		return NULL;

	int i = 0;
	while((i < (max-1)) && (hTmplt->p < hTmplt->end))
	{
		dst[i] = *hTmplt->p;
		hTmplt->p++;
		if (dst[i++] == '\n')
			break;
	}
	dst[i] = '\0';

	return dst;
}
#ifdef __POCC__
#pragma optimize()
#endif
