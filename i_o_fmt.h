#ifndef I_O_FMT_H__
#define I_O_FMT_H__
#pragma once

void lemon_addtext( char *zBuf,  int *pnUsed,  const char *zIn,  int nIn,  int iWidth );
int  lemon_vsprintf(char *str, const char *zFormat, va_list ap);
int  lemon_sprintf(char *str, const char *format, ...);
void lemon_strcpy(char *dest, const char *src);
void lemon_strcat(char *dest, const char *src);

#endif	// I_O_FMT_H__
