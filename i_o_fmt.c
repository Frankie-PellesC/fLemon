/*+@@file@@----------------------------------------------------------------*//*!
 \file		i_o_fmt.c
 \par Description 
            replacement text functions
 \par  Status: 
            
 \par Project: 
            Lemon parser
 \date		Created  on Sat Sep  1 22:05:45 2018
 \date		Modified on Sat Sep  1 22:05:45 2018
 \author	
\*//*-@@file@@----------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "i_o_fmt.h"

/*!
 * Compilers are starting to complain about the use of sprintf() and strcpy(),
 * saying they are unsafe.
 * So we define our own versions of those routines too.
 *
 * There are three routines here:
 *		- lemon_sprintf()
 *		- lemon_vsprintf()
 *		- lemon_addtext()
 *
 * The first two are replacements for sprintf() and vsprintf().
 * The third is a helper routine for vsnprintf() that adds texts to the end of a
 * buffer, making sure the buffer is always zero-terminated.
 *
 * The string formatter is a minimal subset of stdlib sprintf() supporting only
 * a few simply conversions:
 *
 *   %d
 *   %s
 *   %.*s
 */

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		lemon_addtext
 \details	Add text to dynamic buffer
 \param		[in] zBuf The buffer to which text is added
 \param		[in] pnUsed Slots of the buffer used so far
 \param		[in] zIn Text to add
 \param		[in] nIn Bytes of text to add.  -1 to use strlen()
 \param		[in] iWidth Field width.  Negative to left justify
 \date		Created  on Sat Sep  1 22:01:15 2018
 \date		Modified on Sat Sep  1 22:01:15 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void lemon_addtext(char *zBuf, int *pnUsed, const char *zIn, int nIn, int iWidth)
{
	if (nIn < 0)
		for (nIn = 0; zIn[nIn]; nIn++)
		{
		}

	while (iWidth > nIn)
	{
		zBuf[(*pnUsed)++] = ' ';
		iWidth--;
	}

	if (nIn == 0)
		return;

	memcpy(&zBuf[*pnUsed], zIn, nIn);

	*pnUsed += nIn;

	while ((-iWidth) > nIn)
	{
		zBuf[(*pnUsed)++] = ' ';
		iWidth++;
	}

	zBuf[*pnUsed] = 0;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		lemon_vsprintf
 \date		Created  on Sat Sep  1 22:01:41 2018
 \date		Modified on Sat Sep  1 22:01:41 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
int lemon_vsprintf(char *str, const char *zFormat, va_list ap)
{
	int         i, j, k, c;
	int         nUsed = 0;
	const char *z;
	char        zTemp[50];

	str[0] = 0;

	for (i = j = 0; (c = zFormat[i]) != 0; i++)
	{
		if (c == '%')
		{
			int iWidth = 0;

			lemon_addtext(str, &nUsed, &zFormat[j], i - j, 0);

			c = zFormat[++i];

			if (ISDIGIT(c) || (c == '-' && ISDIGIT(zFormat[i + 1])))
			{
				if (c == '-')
					i++;

				while (ISDIGIT(zFormat[i]))
					iWidth = iWidth * 10 + zFormat[i++] - '0';

				if (c == '-')
					iWidth = -iWidth;

				c = zFormat[i];
			}

			if (c == 'd')
			{
				int v = va_arg(ap, int);

				if (v < 0)
				{
					lemon_addtext(str, &nUsed, "-", 1, iWidth);
					v = -v;
				}
				else if (v == 0)
				{
					lemon_addtext(str, &nUsed, "0", 1, iWidth);
				}

				k = 0;

				while (v > 0)
				{
					k++;
					zTemp[sizeof(zTemp) - k] = (v % 10) + '0';
					v /= 10;
				}

				lemon_addtext(str, &nUsed, &zTemp[sizeof(zTemp) - k], k, iWidth);
			}
			else if (c == 's')
			{
				z = va_arg(ap, const char *);
				lemon_addtext(str, &nUsed, z, -1, iWidth);
			}
			else if (c == '.' && memcmp(&zFormat[i], ".*s", 3) == 0)
			{
				i += 2;
				k  = va_arg(ap, int);
				z  = va_arg(ap, const char *);
				lemon_addtext(str, &nUsed, z, k, iWidth);
			}
			else if (c == '%')
			{
				lemon_addtext(str, &nUsed, "%", 1, 0);
			}
			else
			{
				fprintf(stderr, "illegal format\n");
				exit(1);
			}

			j = i + 1;
		}
	}

	lemon_addtext(str, &nUsed, &zFormat[j], i - j, 0);

	return nUsed;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		lemon_sprintf
 \date		Created  on Sat Sep  1 22:03:53 2018
 \date		Modified on Sat Sep  1 22:03:53 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
int lemon_sprintf(char *str, const char *format, ...)
{
	va_list ap;
	int     rc;

	va_start(ap, format);

	rc = lemon_vsprintf(str, format, ap);

	va_end(ap);

	return rc;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		lemon_strcpy
 \date		Created  on Sat Sep  1 22:04:22 2018
 \date		Modified on Sat Sep  1 22:04:34 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void lemon_strcpy(char *dest, const char *src)
{
	while ((*(dest++) = *(src++)) != 0)
	{
	}
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		lemon_strcat
 \date		Created  on Sat Sep  1 22:04:57 2018
 \date		Modified on Sat Sep  1 22:04:57 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void lemon_strcat(char *dest, const char *src)
{
	while (*dest)
		dest++;

	lemon_strcpy(dest, src);
}
