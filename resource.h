#ifndef RESOURCE_H__
#define RESOURCE_H__
#pragma once

#ifndef __POCC__
#define restrict
#endif

typedef struct
{
	FILE *fp;
	char *base;
	char *p;
	char *end;
} RES_DATACTRL, *RES_HANDLE;

RES_HANDLE ResHandleCreate(void);
RES_HANDLE OpenTemplate(void);
char * res_fgets(char * restrict dst, int max, RES_HANDLE restrict hTmplt); 


#endif	// RESOURCE_H__
