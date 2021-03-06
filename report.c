/*+@@file@@----------------------------------------------------------------*//*!
 \file		report.c
 \par Description 
            Procedures for generating reports and tables in the LEMON 
            parser generator.
 \par  Status: 
            
 \par Project: 
            Lemon parser
 \date		Created  on Sat Sep  1 20:03:53 2018
 \date		Modified on Sat Sep  1 20:03:53 2018
 \author	
\*//*-@@file@@----------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include <io.h>
#include "struct.h"
#include "action.h"
#include "i_o_fmt.h"
#include "set.h"
#include "table.h"
#include "resource.h"

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		file_makename
 \details	Generate a filename with the given suffix.\n
            Space to hold the name comes from malloc() and must be freed by 
            the calling function.
 \date		Created  on Sat Sep  1 20:04:23 2018
 \date		Modified on Sat Sep  1 20:04:23 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE char *file_makename(struct lemon *lemp, const char *suffix)
{
  char *name;
  char *cp;
  char *filename = lemp->filename;
  int   sz;

	if (outputDir)
	{
		cp = strrchr(filename, '/');
		if (cp)
			filename = cp + 1;
	}
	sz = lemonStrlen(filename);
	sz += lemonStrlen(suffix);
	if (outputDir)
		sz += lemonStrlen(outputDir) + 1;
	sz += 5;
	name = (char *)malloc(sz);
	if (name == 0)
	{
		fprintf(stderr, "Can't allocate space for a filename.\n");
		exit(1);
	}
	name[0] = 0;
	if (outputDir)
	{
		lemon_strcpy(name, outputDir);
		lemon_strcat(name, "/");
	}
	lemon_strcat(name, filename);
	cp = strrchr(name, '.');
	if (cp)
		*cp = 0;
	lemon_strcat(name, suffix);
	return name;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		file_open
 \details	Open a file with a name based on the name of the input file, 
            but with a different (specified) suffix, and return a pointer 
            to the stream
 \date		Created  on Sat Sep  1 20:05:32 2018
 \date		Modified on Sat Sep  1 20:05:32 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE FILE *file_open(struct lemon *lemp, const char *suffix, const char *mode)
{
	FILE *fp;

	if (lemp->outname)
		free(lemp->outname);

	lemp->outname = file_makename(lemp, suffix);

	fp = fopen(lemp->outname, mode);
	if (fp == 0 && *mode == 'w')
	{
		fprintf(stderr, "Can't open file \"%s\".\n", lemp->outname);
		lemp->errorcnt++;
		return 0;
	}

	return fp;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		rule_print
 \details	Print the text of a rule
 \date		Created  on Sat Sep  1 20:05:56 2018
 \date		Modified on Sat Sep  1 20:05:56 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void rule_print(FILE *out, struct rule *rp)
{
	int i, j;

	fprintf(out, "%s", rp->lhs->name);
	//if( rp->lhsalias ) fprintf(out,"(%s)",rp->lhsalias);

	fprintf(out, " ::=");

	for (i = 0; i < rp->nrhs; i++)
	{
		struct symbol *sp = rp->rhs[i];
		if (sp->type == MULTITERMINAL)
		{
			fprintf(out, " %s", sp->subsym[0]->name);
			for (j = 1; j < sp->nsubsym; j++)
			{
				fprintf(out, "|%s", sp->subsym[j]->name);
			}
		}
		else
		{
			fprintf(out, " %s", sp->name);
		}
		//if( rp->rhsalias[i] ) fprintf(out,"(%s)",rp->rhsalias[i]);
	}
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Reprint
 \details	Duplicate the input file without comments and without actions 
            on rules
 \date		Created  on Sat Sep  1 20:07:12 2018
 \date		Modified on Sat Sep  1 20:07:12 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void Reprint(struct lemon *lemp)
{
	struct rule *rp;
	struct symbol *sp;
	int i, j, maxlen, len, ncolumns, skip;

	printf("// Reprint of input file \"%s\".\n// Symbols:\n", lemp->filename);
	maxlen = 10;

	for (i = 0; i < lemp->nsymbol; i++)
	{
		sp = lemp->symbols[i];
		len = lemonStrlen(sp->name);
		if (len > maxlen)
			maxlen = len;
	}

	ncolumns = 76 / (maxlen + 5);
	if (ncolumns < 1)
		ncolumns = 1;

	skip = (lemp->nsymbol + ncolumns - 1) / ncolumns;

	for (i = 0; i < skip; i++)
	{
		printf("//");
		for (j = i; j < lemp->nsymbol; j += skip)
		{
			sp = lemp->symbols[j];
			assert(sp->index == j);
			printf(" %3d %-*.*s", j, maxlen, maxlen, sp->name);
		}
		printf("\n");
	}

	for (rp = lemp->rule; rp; rp = rp->next)
	{
		rule_print(stdout, rp);
		printf(".");
		if (rp->precsym)
			printf(" [%s]", rp->precsym->name);
		//if( rp->code ) printf("\n    %s",rp->code);
		printf("\n");
	}
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		RulePrint
 \details	Print a single rule.
 \date		Created  on Sat Sep  1 20:08:03 2018
 \date		Modified on Sat Sep  1 20:08:03 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void RulePrint(FILE *fp, struct rule *rp, int iCursor)
{
	struct symbol *sp;
	int i, j;

	fprintf(fp, "%s ::=", rp->lhs->name);

	for (i = 0; i <= rp->nrhs; i++)
	{
		if (i == iCursor)
			fprintf(fp, " *");
		if (i == rp->nrhs)
			break;
		sp = rp->rhs[i];
		if (sp->type == MULTITERMINAL)
		{
			fprintf(fp, " %s", sp->subsym[0]->name);
			for (j = 1; j < sp->nsubsym; j++)
			{
				fprintf(fp, "|%s", sp->subsym[j]->name);
			}
		}
		else
		{
			fprintf(fp, " %s", sp->name);
		}
	}
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		ConfigPrint
 \details	Print the rule for a configuration.
 \date		Created  on Sat Sep  1 20:08:28 2018
 \date		Modified on Sat Sep  1 20:08:28 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void ConfigPrint(FILE *fp, struct config *cfp)
{
	RulePrint(fp, cfp->rp, cfp->dot);
}

//#define TEST
#if 0
/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		SetPrint
 \details	Print a set
 \date		Created  on Sat Sep  1 20:11:04 2018
 \date		Modified on Sat Sep  1 20:11:04 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE void SetPrint(FILE *out, char *set, struct lemon *lemp)
{
	int i;
	char *spacer;
	spacer = "";
	fprintf(out, "%12s[", "");
	for (i = 0; i < lemp->nterminal; i++)
	{
		if (SetFind(set, i))
		{
			fprintf(out, "%s%s", spacer, lemp->symbols[i]->name);
			spacer = " ";
		}
	}
	fprintf(out, "]\n");
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		PlinkPrint
 \details	Print a plink chain
 \date		Created  on Sat Sep  1 20:12:07 2018
 \date		Modified on Sat Sep  1 20:12:07 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE void PlinkPrint(FILE *out, struct plink *plp, char *tag)
{
	while (plp)
	{
		fprintf(out, "%12s%s (state %2d) ", "", tag, plp->cfp->stp->statenum);
		ConfigPrint(out, plp->cfp);
		fprintf(out, "\n");
		plp = plp->next;
	}
}
#endif

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		PrintAction
 \details	Print an action to the given file descriptor.

 \param		[in] ap The action to print

 \param		[in] fp Stream where to print action
 \return	Return FALSE if nothing was actually printed.
 \date		Created  on Sat Sep  1 20:14:50 2018
 \date		Modified on Sat Sep  1 20:14:50 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
int PrintAction(struct action *ap, FILE * fp, int indent)
{
	int result = 1;
	switch (ap->type)
	{
		case SHIFT:
		{
			struct state *stp = ap->x.stp;
			fprintf(fp, "%*s shift        %-7d", indent, ap->sp->name, stp->statenum);
			break;
		}
		case REDUCE:
		{
			struct rule *rp = ap->x.rp;
			fprintf(fp, "%*s reduce       %-7d", indent, ap->sp->name, rp->iRule);
			RulePrint(fp, rp, -1);
			break;
		}
		case SHIFTREDUCE:
		{
			struct rule *rp = ap->x.rp;
			fprintf(fp, "%*s shift-reduce %-7d", indent, ap->sp->name, rp->iRule);
			RulePrint(fp, rp, -1);
			break;
		}
		case ACCEPT:
			fprintf(fp, "%*s accept", indent, ap->sp->name);
			break;
		case ERROR:
			fprintf(fp, "%*s error", indent, ap->sp->name);
			break;
		case SRCONFLICT:
		case RRCONFLICT:
			fprintf(fp, "%*s reduce       %-7d ** Parsing conflict **",
										indent, ap->sp->name, ap->x.rp->iRule);
			break;
		case SSCONFLICT:
			fprintf(fp, "%*s shift        %-7d ** Parsing conflict **",
									indent, ap->sp->name, ap->x.stp->statenum);
			break;
		case SH_RESOLVED:
			if (showPrecedenceConflict)
			{
				fprintf(fp, "%*s shift        %-7d -- dropped by precedence",
									indent, ap->sp->name, ap->x.stp->statenum);
			}
			else
			{
				result = 0;
			}
			break;
		case RD_RESOLVED:
			if (showPrecedenceConflict)
			{
				fprintf(fp, "%*s reduce %-7d -- dropped by precedence",
										indent, ap->sp->name, ap->x.rp->iRule);
			}
			else
			{
				result = 0;
			}
			break;
		case NOT_USED:
			result = 0;
			break;
	}
	if (result && ap->spOpt)
	{
		fprintf(fp, "  /* because %s==%s */", ap->sp->name, ap->spOpt->name);
	}
	return result;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		ReportOutput
 \details	Generate the "*.out" log file
 \date		Created  on Sat Sep  1 20:15:41 2018
 \date		Modified on Sat Sep  1 20:15:41 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void ReportOutput(struct lemon *lemp)
{
	int            i, n;
	struct state  *stp;
	struct config *cfp;
	struct action *ap;
	struct rule   *rp;
	FILE          *fp;

	fp = file_open(lemp, ".out", "wb");
	if (fp == 0)
		return;

	for (i = 0; i < lemp->nxstate; i++)
	{
		stp = lemp->sorted[i];
		fprintf(fp, "State %d:\n", stp->statenum);

		if (lemp->basisflag)
			cfp = stp->bp;
		else
			cfp = stp->cfp;

		while (cfp)
		{
			char buf[20];
			if (cfp->dot == cfp->rp->nrhs)
			{
				lemon_sprintf(buf, "(%d)", cfp->rp->iRule);
				fprintf(fp, "    %5s ", buf);
			}
			else
			{
				fprintf(fp, "          ");
			}

			ConfigPrint(fp, cfp);
			fprintf(fp, "\n");
#if 0
			SetPrint(fp, cfp->fws, lemp);
			PlinkPrint(fp, cfp->fplp, "To  ");
			PlinkPrint(fp, cfp->bplp, "From");
#endif
			if (lemp->basisflag)
				cfp = cfp->bp;
			else
				cfp = cfp->next;
		}

		fprintf(fp, "\n");

		for (ap = stp->ap; ap; ap = ap->next)
		{
			if (PrintAction(ap, fp, 30))
				fprintf(fp, "\n");
		}

		fprintf(fp, "\n");
	}

	fprintf(fp, "----------------------------------------------------\n");
	fprintf(fp, "Symbols:\n");
	fprintf(fp, "The first-set of non-terminals is shown after the name.\n\n");

	for (i = 0; i < lemp->nsymbol; i++)
	{
		int j;
		struct symbol *sp;

		sp = lemp->symbols[i];
		fprintf(fp, "  %3d: %s", i, sp->name);

		if (sp->type == NONTERMINAL)
		{
			fprintf(fp, ":");
			if (sp->lambda)
			{
				fprintf(fp, " <lambda>");
			}
			for (j = 0; j < lemp->nterminal; j++)
			{
				if (sp->firstset && SetFind(sp->firstset, j))
				{
					fprintf(fp, " %s", lemp->symbols[j]->name);
				}
			}
		}

		if (sp->prec >= 0)
			fprintf(fp, " (precedence=%d)", sp->prec);

		fprintf(fp, "\n");
	}

	fprintf(fp, "----------------------------------------------------\n");
	fprintf(fp, "Syntax-only Symbols:\n");
	fprintf(fp, "The following symbols never carry semantic content.\n\n");

	for (i = n = 0; i < lemp->nsymbol; i++)
	{
		int w;

		struct symbol *sp = lemp->symbols[i];

		if (sp->bContent)
			continue;

		w = (int)strlen(sp->name);

		if (n > 0 && n + w > 75)
		{
			fprintf(fp, "\n");
			n = 0;
		}

		if (n > 0)
		{
			fprintf(fp, " ");
			n++;
		}

		fprintf(fp, "%s", sp->name);
		n += w;
	}

	if (n > 0)
		fprintf(fp, "\n");
	fprintf(fp, "----------------------------------------------------\n");
	fprintf(fp, "Rules:\n");

	for (rp = lemp->rule; rp; rp = rp->next)
	{
		fprintf(fp, "%4d: ", rp->iRule);
		rule_print(fp, rp);
		fprintf(fp, ".");

		if (rp->precsym)
		{
			fprintf(fp, " [%s precedence=%d]", rp->precsym->name, rp->precsym->prec);
		}

		fprintf(fp, "\n");
	}

	fclose(fp);

	return;
}

#if 0
/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		pathsearch
 \details	Search for the file "name" which is in the same directory as 
            the exacutable
 \date		Created  on Sat Sep  1 20:18:08 2018
 \date		Modified on Sat Sep  1 20:18:08 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE char *pathsearch(char *argv0, char *name, int modemask)
{
	const char *pathlist;
	char *pathbufptr;
	char *pathbuf;
	char *path, *cp;
	char c;

	#ifdef __WIN32__
		cp = strrchr(argv0, '\\');
	#else
		cp = strrchr(argv0, '/');
	#endif

	if (cp)
	{
		c    = *cp;
		*cp  = 0;
		path = (char *)malloc(lemonStrlen(argv0) + lemonStrlen(name) + 2);

		if (path)
			lemon_sprintf(path, "%s/%s", argv0, name);

		*cp  = c;
	}
	else
	{
		pathlist = getenv("PATH");

		if (pathlist == 0)
			pathlist = ".:/bin:/usr/bin";

		pathbuf = (char *)malloc(lemonStrlen(pathlist) + 1);
		path    = (char *)malloc(lemonStrlen(pathlist) + lemonStrlen(name) + 2);

		if ((pathbuf != 0) && (path != 0))
		{
			pathbufptr = pathbuf;
			lemon_strcpy(pathbuf, pathlist);

			while (*pathbuf)
			{
				cp = strchr(pathbuf, ':');

				if (cp == 0)
					cp = &pathbuf[lemonStrlen(pathbuf)];

				c   = *cp;
				*cp = 0;
				lemon_sprintf(path, "%s/%s", pathbuf, name);
				*cp = c;

				if (c == 0)
					pathbuf[0] = 0;
				else
					pathbuf = &cp[1];

				if (_access(path, modemask) == 0)
					break;
			}

			free(pathbufptr);
		}
	}

	return path;
}
#endif

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		compute_action
 \details	Given an action, compute the integer value for that action 
            which is to be put in the action table of the generated 
            machine.\n
 \return	Return negative if no action should be generated.
 \date		Created  on Sat Sep  1 20:20:35 2018
 \date		Modified on Sat Sep  1 20:20:35 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE int compute_action(struct lemon *lemp, struct action *ap)
{
	int act;
	switch (ap->type)
	{
		case SHIFT:
			act = ap->x.stp->statenum;
			break;
		case SHIFTREDUCE:
		{
			/*
			 * Since a SHIFT is inherient after a prior REDUCE, convert any
			 * SHIFTREDUCE action with a nonterminal on the LHS into a simple
			 * REDUCE action:
			 */
			if (ap->sp->index >= lemp->nterminal)
			{
				act = lemp->minReduce + ap->x.rp->iRule;
			}
			else
			{
				act = lemp->minShiftReduce + ap->x.rp->iRule;
			}
			break;
		}
		case REDUCE:
			act = lemp->minReduce + ap->x.rp->iRule;
			break;
		case ERROR:
			act = lemp->errAction;
			break;
		case ACCEPT:
			act = lemp->accAction;
			break;
		default:
			act = -1;
			break;
	}
	return act;
}

#define LINESIZE 1000
/*
 * The next cluster of routines are for reading the template file
 * and writing the results to the generated parser
 */

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		tplt_xfer
 \details	The function transfers data from "in" to "out" until a line is 
            seen which begins with "%%".\n
            The line number is tracked.\n
            if name!=0, then any word that begin with "Parse" is changed to 
            begin with *name instead.
 \date		Created  on Sat Sep  1 20:22:50 2018
 \date		Modified on Sat Sep  1 20:22:50 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE void tplt_xfer(char *name, RES_HANDLE hTmplt, FILE *out, int *lineno)
{
	int i, iStart;
	char line[LINESIZE];

	while (res_fgets(line, LINESIZE, hTmplt) &&
				( (strlen(line) < 2) || line[0] != '%' || line[1] != '%' ) )
	{
		(*lineno)++;
		iStart = 0;
		if (name)
		{
			for (i = 0; line[i]; i++)
			{
				if (line[i] == 'P' &&
						strncmp(&line[i], "Parse", 5) == 0 && (i == 0 || !ISALPHA(line[i - 1])))
				{
					if (i > iStart)
						fprintf(out, "%.*s", i - iStart, &line[iStart]);
					fprintf(out, "%s", name);
					i += 4;
					iStart = i + 1;
				}
			}
		}
		fprintf(out, "%s", &line[iStart]);
	}
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		tplt_open
 \details	This function finds the template file and opens it, 
            returning a pointer to the opened file.
 \date		Created  on Sat Sep  1 20:23:20 2018
 \date		Modified on Sat Sep  1 20:23:20 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
//PRIVATE FILE *tplt_open(struct lemon *lemp)
PRIVATE RES_HANDLE tplt_open(struct lemon *lemp)
{
	FILE       *in;
	RES_HANDLE  hTmplt         = NULL;

	/*
	 * first, see if user specified a template filename on the command line.
	 */
	if (user_templatename != 0)
	{
		if (_access(user_templatename, 004) == -1)
		{
			fprintf(stderr, "Can't find the parser driver template file \"%s\".\n", user_templatename);
			lemp->errorcnt++;
			return 0;
		}

		in = fopen(user_templatename, "rb");

		if (in == 0)
		{
			fprintf(stderr, "Can't open the template file \"%s\".\n", user_templatename);
			lemp->errorcnt++;
			return 0;
		}

		hTmplt     = ResHandleCreate();
		hTmplt->fp = in;

		return hTmplt;
	}

	/*
	 * frankie
	 * Try resource skeleton before giveup.
	 */
	hTmplt = OpenTemplate();
	if (hTmplt)
		return hTmplt;

	return 0;

	//static char templatename[] = "lempar.lt";
	//char        buf[1000];
	//FILE       *in;
	//char       *tpltname;
	//char       *cp;

	//cp = strrchr(lemp->filename, '.');

	//if (cp)
	//{
		//lemon_sprintf(buf, "%.*s.lt", (int)(cp - lemp->filename), lemp->filename);
	//}
	//else
	//{
		//lemon_sprintf(buf, "%s.lt", lemp->filename);
	//}

	//if (_access(buf, 004) == 0)
	//{
		//tpltname = buf;
	//}
	//else if (_access(templatename, 004) == 0)
	//{
		//tpltname = templatename;
	//}
	//else
	//{
		//tpltname = pathsearch(lemp->argv0, templatename, 0);
	//}

	//if (tpltname == 0)
	//{
		//fprintf(stderr, "Can't find the parser driver template file \"%s\".\n", templatename);
		//lemp->errorcnt++;
		//return 0;
	//}

	//in = fopen(tpltname, "rb");

	//if (in == 0)
	//{
		//fprintf(stderr, "Can't open the template file \"%s\".\n", templatename);
		//lemp->errorcnt++;
		//return 0;
	//}

	//hTmplt     = ResHandleCreate();
	//hTmplt->fp = in;

	//return hTmplt;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		tplt_linedir
 \details	Print a #line directive line to the output file.
 \date		Created  on Sat Sep  1 20:24:44 2018
 \date		Modified on Sat Sep  1 20:24:44 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE void tplt_linedir(FILE *out, int lineno, char *filename)
{
	fprintf(out, "#line %d \"", lineno);

	while (*filename)
	{
		if (*filename == '\\')
			putc('\\', out);

		putc(*filename, out);

		filename++;
	}

	fprintf(out, "\"\n");
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		tplt_print
 \details	Print a string to the file and keep the linenumber up to date
 \date		Created  on Sat Sep  1 20:25:08 2018
 \date		Modified on Sat Sep  1 20:25:08 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE void tplt_print(FILE *out, struct lemon *lemp, char *str, int *lineno)
{
	if (str == 0)
		return;

	while (*str)
	{
		putc(*str, out);
		if (*str == '\n')
			(*lineno)++;

		str++;
	}

	if (str[-1] != '\n')
	{
		putc('\n', out);
		(*lineno)++;
	}

	if (!lemp->nolinenosflag)
	{
		(*lineno)++;
		tplt_linedir(out, *lineno, lemp->outname);
	}

	return;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		emit_destructor_code
 \details	The following routine emits code for the destructor for the 
            symbol sp
 \date		Created  on Sat Sep  1 20:25:48 2018
 \date		Modified on Sat Sep  1 20:25:48 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void emit_destructor_code(FILE *out, struct symbol *sp, struct lemon *lemp, int *lineno)
{
	char *cp = 0;

	if (sp->type == TERMINAL)
	{
		cp = lemp->tokendest;

		if (cp == 0)
			return;

		fprintf(out, "{\n");

		(*lineno)++;
	}
	else if (sp->destructor)
	{
		cp = sp->destructor;

		fprintf(out, "{\n");

		(*lineno)++;

		if (!lemp->nolinenosflag)
		{
			(*lineno)++;
			tplt_linedir(out, sp->destLineno, lemp->filename);
		}
	}
	else if (lemp->vardest)
	{
		cp = lemp->vardest;

		if (cp == 0)
			return;

		fprintf(out, "{\n");
		(*lineno)++;
	}
	else
	{
		assert(0);	/* Cannot happen */
	}

	for (; *cp; cp++)
	{
		if (*cp == '$' && cp[1] == '$')
		{
			fprintf(out, "(yypminor->yy%d)", sp->dtnum);
			cp++;
			continue;
		}

		if (*cp == '\n')
			(*lineno)++;

		fputc(*cp, out);
	}

	fprintf(out, "\n");
	(*lineno)++;

	if (!lemp->nolinenosflag)
	{
		(*lineno)++;
		tplt_linedir(out, *lineno, lemp->outname);
	}

	fprintf(out, "}\n");
	(*lineno)++;

	return;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		has_destructor
 \details	Return TRUE (non-zero) if the given symbol has a destructor.
 \date		Created  on Sat Sep  1 20:27:03 2018
 \date		Modified on Sat Sep  1 20:27:03 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
int has_destructor(struct symbol *sp, struct lemon *lemp)
{
	int ret;

	if (sp->type == TERMINAL)
	{
		ret = lemp->tokendest != 0;
	}
	else
	{
		ret = lemp->vardest != 0 || sp->destructor != 0;
	}

	return ret;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		append_str
 \details	Append text to a dynamically allocated string.\n
            If zText is 0 then reset the string to be empty again.\n
            Always return the complete text of the string (which is 
            overwritten with each call).\n
            n bytes of zText are stored.\n
            If n==0 then all of zText up to the first \000 terminator is 
            stored.\n
            zText can contain up to two instances of %d.\n
            The values of p1 and p2 are written into the first and second 
            %d.\n
            If n==-1, then the previous character is overwritten.
 \date		Created  on Sat Sep  1 20:28:47 2018
 \date		Modified on Sat Sep  1 20:28:47 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE char *append_str(const char *zText, int n, int p1, int p2)
{
	static char  empty[1] = { 0 };
	static char *z        = 0;
	static int   alloced  = 0;
	static int   used     = 0;
	int          c;
	char         zInt[40];

	if (zText == 0)
	{
		if (used == 0 && z != 0)
			z[0] = 0;

		used = 0;

		return z;
	}

	if (n <= 0)
	{
		if (n < 0)
		{
			used += n;
			assert(used >= 0);
		}

		n = lemonStrlen(zText);
	}

	if ((int)(n + sizeof(zInt) * 2 + used) >= alloced)
	{
		alloced = n + sizeof(zInt) * 2 + used + 200;
		z       = (char *)realloc(z, alloced);
	}

	if (z == 0)
		return empty;

	while (n-- > 0)
	{
		c = *(zText++);

		if (c == '%' && n > 0 && zText[0] == 'd')
		{
			lemon_sprintf(zInt, "%d", p1);
			p1 = p2;
			lemon_strcpy(&z[used], zInt);
			used += lemonStrlen(&z[used]);
			zText++;
			n--;
		}
		else
		{
			z[used++] = (char)c;
		}
	}

	z[used] = 0;

	return z;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		translate_code
 \details	Write and transform the rp->code string so that symbols are 
            expanded.\n
            Populate the rp->codePrefix and rp->codeSuffix strings, as 
            appropriate.\n
            Return 1 if the expanded code requires that "yylhsminor" local 
            variable to be defined.
 \date		Created  on Sat Sep  1 20:30:57 2018
 \date		Modified on Sat Sep  1 20:30:57 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE int translate_code(struct lemon *lemp, struct rule *rp)
{
	char       *cp, *xp;
	int         i;
	int         rc          = 0;	/* True if yylhsminor is used */
	int         dontUseRhs0 = 0;	/* If true, use of left-most RHS label is illegal */
	const char *zSkip       = 0;	/* The zOvwrt comment within rp->code, or NULL */
	char        lhsused     = 0;	/* True if the LHS element has been used */
	char        lhsdirect;			/* True if LHS writes directly into stack */
	char        used[MAXRHS];		/* True for each RHS element which is used */
	char        zLhs[50];			/* Convert the LHS symbol into this string */
	char        zOvwrt[900];		/* Comment that to allow LHS to overwrite RHS */

	for (i = 0; i < rp->nrhs; i++)
		used[i] = 0;
	lhsused = 0;

	if (rp->code == 0)
	{
		static char newlinestr[2] = { '\n', '\0' };
		rp->code = newlinestr;
		rp->line = rp->ruleline;
		rp->noCode = 1;
	}
	else
	{
		rp->noCode = 0;
	}

	if (rp->nrhs == 0)
	{
		/*
		 * If there are no RHS symbols, then writing directly to the LHS is ok
		 */
		lhsdirect = 1;
	}
	else if (rp->rhsalias[0] == 0)
	{
		/*
		 * The left-most RHS symbol has no value.
		 * LHS direct is ok.
		 * But we have to call the distructor on the RHS symbol first.
		 */
		lhsdirect = 1;

		if (has_destructor(rp->rhs[0], lemp))
		{
			append_str(0, 0, 0, 0);
			append_str("  yy_destructor(yypParser,%d,&yymsp[%d].minor);\n",
											0, rp->rhs[0]->index, 1 - rp->nrhs);
			rp->codePrefix = Strsafe(append_str(0, 0, 0, 0));
			rp->noCode = 0;
		}
	}
	else if (rp->lhsalias == 0)
	{
		/*
		 * There is no LHS value symbol.
		 */
		lhsdirect = 1;
	}
	else if (strcmp(rp->lhsalias, rp->rhsalias[0]) == 0)
	{
		/*
		 * The LHS symbol and the left-most RHS symbol are the same, so
		 * direct writing is allowed
		 */
		lhsdirect = 1;
		lhsused   = 1;
		used[0]   = 1;

		if (rp->lhs->dtnum != rp->rhs[0]->dtnum)
		{
			ErrorMsg(lemp->filename, rp->ruleline,
						"%s(%s) and %s(%s) share the same label but have "
						"different datatypes.",
						rp->lhs->name, rp->lhsalias, rp->rhs[0]->name, rp->rhsalias[0]);
			lemp->errorcnt++;
		}
	}
	else
	{
		lemon_sprintf(zOvwrt, "/*%s-overwrites-%s*/", rp->lhsalias, rp->rhsalias[0]);
		zSkip = strstr(rp->code, zOvwrt);

		if (zSkip != 0)
		{
			/*
			 * The code contains a special comment that indicates that it is safe
			 * for the LHS label to overwrite left-most RHS label.
			 */
			lhsdirect = 1;
		}
		else
		{
			lhsdirect = 0;
		}
	}

	if (lhsdirect)
	{
		sprintf(zLhs, "yymsp[%d].minor.yy%d", 1 - rp->nrhs, rp->lhs->dtnum);
	}
	else
	{
		rc = 1;
		sprintf(zLhs, "yylhsminor.yy%d", rp->lhs->dtnum);
	}

	append_str(0, 0, 0, 0);

	/*
	 * This const cast is wrong but harmless, if we're careful.
	 */
	for (cp = (char *)rp->code; *cp; cp++)
	{
		if (cp == zSkip)
		{
			append_str(zOvwrt, 0, 0, 0);
			cp += lemonStrlen(zOvwrt) - 1;
			dontUseRhs0 = 1;
			continue;
		}

		if (ISALPHA(*cp) && (cp == rp->code ||
							(!ISALNUM(cp[-1]) && cp[-1] != '_')))
		{
			char saved;

			for (xp = &cp[1]; ISALNUM(*xp) || *xp == '_'; xp++)
				;

			saved = *xp;
			*xp   = 0;

			if (rp->lhsalias && strcmp(cp, rp->lhsalias) == 0)
			{
				append_str(zLhs, 0, 0, 0);
				cp      = xp;
				lhsused = 1;
			}
			else
			{
				for (i = 0; i < rp->nrhs; i++)
				{
					if (rp->rhsalias[i] && strcmp(cp, rp->rhsalias[i]) == 0)
					{
						if (i == 0 && dontUseRhs0)
						{
							ErrorMsg(lemp->filename, rp->ruleline, "Label %s used after '%s'.", rp->rhsalias[0], zOvwrt);
							lemp->errorcnt++;
						}
						else if (cp != rp->code && cp[-1] == '@')
						{
							/*
							 * If the argument is of the form @X then substituted
							 * the token number of X, not the value of X
							 */
							append_str("yymsp[%d].major", -1, i - rp->nrhs + 1, 0);
						}
						else
						{
							struct symbol *sp = rp->rhs[i];
							int dtnum;
							if (sp->type == MULTITERMINAL)
							{
								dtnum = sp->subsym[0]->dtnum;
							}
							else
							{
								dtnum = sp->dtnum;
							}
							append_str("yymsp[%d].minor.yy%d", 0, i - rp->nrhs + 1, dtnum);
						}

						cp      = xp;
						used[i] = 1;
						break;
					}
				}
			}

			*xp = saved;
		}

		append_str(cp, 1, 0, 0);
	}	/* End loop */

	/*
	 * Main code generation completed
	 */
	cp = append_str(0, 0, 0, 0);

	if (cp && cp[0])
		rp->code = Strsafe(cp);

	append_str(0, 0, 0, 0);

	/*
	 * Check to make sure the LHS has been used
	 */
	if (rp->lhsalias && !lhsused)
	{
		ErrorMsg(lemp->filename, rp->ruleline,
					"Label \"%s\" for \"%s(%s)\" is never used.",
							rp->lhsalias, rp->lhs->name, rp->lhsalias);
		lemp->errorcnt++;
	}

	/*
	 * Generate destructor code for RHS minor values which are not referenced.
	 * Generate error messages for unused labels and duplicate labels.
	 */
	for (i = 0; i < rp->nrhs; i++)
	{
		if (rp->rhsalias[i])
		{
			if (i > 0)
			{
				int j;

				if (rp->lhsalias && strcmp(rp->lhsalias, rp->rhsalias[i]) == 0)
				{
					ErrorMsg(lemp->filename, rp->ruleline,
								"%s(%s) has the same label as the LHS but is not the left-most "
												"symbol on the RHS.", rp->rhs[i]->name, rp->rhsalias);
					lemp->errorcnt++;
				}

				for (j = 0; j < i; j++)
				{
					if (rp->rhsalias[j] && strcmp(rp->rhsalias[j], rp->rhsalias[i]) == 0)
					{
						ErrorMsg(lemp->filename, rp->ruleline,
								"Label %s used for multiple symbols on the RHS of a rule.", rp->rhsalias[i]);
						lemp->errorcnt++;
						break;
					}
				}
			}

			if (!used[i])
			{
				ErrorMsg(lemp->filename, rp->ruleline,
							"Label %s for \"%s(%s)\" is never used.",
							rp->rhsalias[i], rp->rhs[i]->name, rp->rhsalias[i]);
				lemp->errorcnt++;
			}
		}
		else if (i > 0 && has_destructor(rp->rhs[i], lemp))
		{
			append_str("  yy_destructor(yypParser,%d,&yymsp[%d].minor);\n",
										0, rp->rhs[i]->index, i - rp->nrhs + 1);
		}
	}

	/*
	 * If unable to write LHS values directly into the stack, write the
	 * saved LHS value now.
	 */
	if (lhsdirect == 0)
	{
		append_str("  yymsp[%d].minor.yy%d = ", 0, 1 - rp->nrhs, rp->lhs->dtnum);
		append_str(zLhs, 0, 0, 0);
		append_str(";\n", 0, 0, 0);
	}

	/*
	 * Suffix code generation complete
	 */
	cp = append_str(0, 0, 0, 0);

	if (cp && cp[0])
	{
		rp->codeSuffix = Strsafe(cp);
		rp->noCode = 0;
	}

	return rc;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		emit_code
 \details	Generate code which executes when the rule "rp" is reduced.\n
            Write the code to "out".\n
            Make sure lineno stays up-to-date.
 \date		Created  on Sat Sep  1 20:39:45 2018
 \date		Modified on Sat Sep  1 20:39:45 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE void emit_code(FILE *out, struct rule *rp, struct lemon *lemp, int *lineno)
{
	const char *cp;

	/*
	 * Setup code prior to the #line directive
	 */
	if (rp->codePrefix && rp->codePrefix[0])
	{
		fprintf(out, "{%s", rp->codePrefix);

		for (cp = rp->codePrefix; *cp; cp++)
		{
			if (*cp == '\n')
				(*lineno)++;
		}
	}

	/*
	 * Generate code to do the reduce action
	 */
	if (rp->code)
	{
		if (!lemp->nolinenosflag)
		{
			(*lineno)++;
			tplt_linedir(out, rp->line, lemp->filename);
		}

		fprintf(out, "{%s", rp->code);

		for (cp = rp->code; *cp; cp++)
		{
			if (*cp == '\n')
				(*lineno)++;
		}

		fprintf(out, "}\n");

		(*lineno)++;

		if (!lemp->nolinenosflag)
		{
			(*lineno)++;
			tplt_linedir(out, *lineno, lemp->outname);
		}
	}

	/*
	 * Generate breakdown code that occurs after the #line directive
	 */
	if (rp->codeSuffix && rp->codeSuffix[0])
	{
		fprintf(out, "%s", rp->codeSuffix);

		for (cp = rp->codeSuffix; *cp; cp++)
		{
			if (*cp == '\n')
				(*lineno)++;
		}
	}

	if (rp->codePrefix)
	{
		fprintf(out, "}\n");
		(*lineno)++;
	}

	return;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		print_stack_union
 \details	Print the definition of the union used for the parser's data 
            stack.\n
            This union contains fields for every possible data type for 
            tokens and nonterminals.\n
            In the process of computing and printing this union, also set 
            the ".dtnum" field of every terminal and nonterminal symbol.

 \param		[in] out The output stream

 \param		[in] lemp he main info structure for this parser

 \param		[in] plineno Pointer to the line number

 \param		[in] mhflag True if generating makeheaders output
 \date		Created  on Sat Sep  1 20:41:47 2018
 \date		Modified on Sat Sep  1 20:44:08 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void print_stack_union(FILE *out, struct lemon *lemp, int *plineno, int mhflag)
{
	int         lineno = *plineno;	/* The line number of the output */
	char      **types;				/* A hash table of datatypes */
	int         arraysize;			/* Size of the "types" array */
	int         maxdtlength;		/* Maximum length of any ".datatype" field. */
	char       *stddt;				/* Standardized name for a datatype */
	int         i, j;				/* Loop counters */
	unsigned    hash;				/* For hashing the name of a type */
	const char *name;				/* Name of the parser */

	/*
	 * Allocate and initialize types[] and allocate stddt[]
	 */
	arraysize = lemp->nsymbol * 2;

	types = (char **)calloc(arraysize, sizeof(char *));

	if (types == 0)
	{
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}

	for (i = 0; i < arraysize; i++)
		types[i] = 0;

	maxdtlength = 0;

	if (lemp->vartype)
	{
		maxdtlength = lemonStrlen(lemp->vartype);
	}

	for (i = 0; i < lemp->nsymbol; i++)
	{
		int len;

		struct symbol *sp = lemp->symbols[i];

		if (sp->datatype == 0)
			continue;

		len = lemonStrlen(sp->datatype);

		if (len > maxdtlength)
			maxdtlength = len;
	}

	stddt = (char *)malloc(maxdtlength * 2 + 1);

	if (stddt == 0)
	{
		fprintf(stderr, "Out of memory.\n");
		exit(1);
	}

	/*!
	 * Build a hash table of datatypes.\n
	 * The ".dtnum" field of each symbol is filled in with the hash index plus 1.\n
	 * A ".dtnum" value of 0 is used for terminal symbols.\n
	 * If there is no %default_type defined then 0 is also used as the .dtnum value
	 * for nonterminals which do not specify a datatype using the %type directive.
	 */
	for (i = 0; i < lemp->nsymbol; i++)
	{
		struct symbol *sp = lemp->symbols[i];
		char *cp;

		if (sp == lemp->errsym)
		{
			sp->dtnum = arraysize + 1;
			continue;
		}

		if (sp->type != NONTERMINAL || (sp->datatype == 0 && lemp->vartype == 0))
		{
			sp->dtnum = 0;
			continue;
		}

		cp = sp->datatype;

		if (cp == 0)
			cp = lemp->vartype;

		j = 0;

		while (ISSPACE(*cp))
			cp++;

		while (*cp)
			stddt[j++] = *cp++;

		while (j > 0 && ISSPACE(stddt[j - 1]))
			j--;

		stddt[j] = 0;

		if (lemp->tokentype && strcmp(stddt, lemp->tokentype) == 0)
		{
			sp->dtnum = 0;
			continue;
		}

		hash = 0;

		for (j = 0; stddt[j]; j++)
		{
			hash = hash * 53 + stddt[j];
		}

		hash = (hash & 0x7fffffff) % arraysize;

		while (types[hash])
		{
			if (strcmp(types[hash], stddt) == 0)
			{
				sp->dtnum = hash + 1;
				break;
			}

			hash++;

			if (hash >= (unsigned)arraysize)
				hash = 0;
		}

		if (types[hash] == 0)
		{
			sp->dtnum = hash + 1;

			types[hash] = (char *)malloc(lemonStrlen(stddt) + 1);

			if (types[hash] == 0)
			{
				fprintf(stderr, "Out of memory.\n");
				exit(1);
			}

			lemon_strcpy(types[hash], stddt);
		}
	}

	/*
	 * Print out the definition of YYTOKENTYPE and YYMINORTYPE
	 */
	name   = lemp->name ? lemp->name : "Parse";
	lineno = *plineno;

	if (mhflag)
	{
		fprintf(out, "#if INTERFACE\n");
		lineno++;
	}

	fprintf(out, "#define %sTOKENTYPE %s\n",
				name, lemp->tokentype ? lemp->tokentype : "void*");

	lineno++;

	if (mhflag)
	{
		fprintf(out, "#endif\n");
		lineno++;
	}

	fprintf(out, "typedef union {\n");
	lineno++;

	fprintf(out, "  int yyinit;\n");
	lineno++;

	fprintf(out, "  %sTOKENTYPE yy0;\n", name);
	lineno++;

	for (i = 0; i < arraysize; i++)
	{
		if (types[i] == 0)
			continue;

		fprintf(out, "  %s yy%d;\n", types[i], i + 1);
		lineno++;

		free(types[i]);
	}

	if (lemp->errsym && lemp->errsym->useCnt)
	{
		fprintf(out, "  int yy%d;\n", lemp->errsym->dtnum);
		lineno++;
	}

	free(stddt);
	free(types);

	fprintf(out, "} YYMINORTYPE;\n");
	lineno++;

	*plineno = lineno;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		minimum_size_type
 \details	Return the name of a C datatype able to represent values 
            between lwr and upr, inclusive.\n
            If pnByte!=NULL then also write the sizeof for that type (1, 2, 
            or 4) into *pnByte.
 \date		Created  on Sat Sep  1 20:50:17 2018
 \date		Modified on Sat Sep  1 20:50:17 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
static const char *minimum_size_type(int lwr, int upr, int *pnByte)
{
	const char *zType = "int";
	int         nByte = 4;

	if (lwr >= 0)
	{
		if (upr <= 255)
		{
			zType = "unsigned char";
			nByte = 1;
		}
		else if (upr < 65535)
		{
			zType = "unsigned short int";
			nByte = 2;
		}
		else
		{
			zType = "unsigned int";
			nByte = 4;
		}
	}
	else if (lwr >= -127 && upr <= 127)
	{
		zType = "signed char";
		nByte = 1;
	}
	else if (lwr >= -32767 && upr < 32767)
	{
		zType = "short";
		nByte = 2;
	}

	if (pnByte)
		*pnByte = nByte;

	return zType;
}

/*!
 * Each state contains a set of token transaction and a set of
 * nonterminal transactions.\n
 * Each of these sets makes an instance of the following structure.\n
 * An array of these structures is used to order the creation of
 * entries in the yy_action[] table.
 */
struct axset
{
	struct state *stp;		/*!< A pointer to a state */
	int           isTkn;	/*!< True to use tokens.  False for non-terminals */
	int           nAction;	/*!< Number of actions */
	int           iOrder;	/*!< Original order of action sets */
};

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		axset_compare
 \details	Compare to axset structures for sorting purposes
 \date		Created  on Sat Sep  1 20:53:51 2018
 \date		Modified on Sat Sep  1 20:53:51 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
static int axset_compare(const void *a, const void *b)
{
	struct axset *p1 = (struct axset *)a;
	struct axset *p2 = (struct axset *)b;
	int c;

	c = p2->nAction - p1->nAction;

	if (c == 0)
	{
		c = p1->iOrder - p2->iOrder;
	}

	assert(c != 0 || p1 == p2);

	return c;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		writeRuleText
 \details	Write text on "out" that describes the rule "rp".
 \date		Created  on Sat Sep  1 20:54:20 2018
 \date		Modified on Sat Sep  1 20:54:20 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
static void writeRuleText(FILE *out, struct rule *rp)
{
	int j;

	fprintf(out, "%s ::=", rp->lhs->name);

	for (j = 0; j < rp->nrhs; j++)
	{
		struct symbol *sp = rp->rhs[j];

		if (sp->type != MULTITERMINAL)
		{
			fprintf(out, " %s", sp->name);
		}
		else
		{
			int k;

			fprintf(out, " %s", sp->subsym[0]->name);

			for (k = 1; k < sp->nsubsym; k++)
			{
				fprintf(out, "|%s", sp->subsym[k]->name);
			}
		}
	}
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		ReportTable
 \details	Generate C source code for the parser

 \param		[in] lemp The parser parameters block

 \param		[in] mhflag Output in makeheaders format if true
 \date		Created  on Sat Sep  1 20:56:53 2018
 \date		Modified on Sat Sep  1 20:56:53 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void ReportTable(struct lemon *lemp, int mhflag)
{
	FILE          *out;//, *in;
	char           line[LINESIZE];
	int            lineno;
	struct state  *stp;
	struct action *ap;
	struct rule   *rp;
	struct acttab *pActtab;
	int            i, j, n, sz;
	int            szActionType;	/* sizeof(YYACTIONTYPE) */
	int            szCodeType;		/* sizeof(YYCODETYPE)   */
	const char    *name;
	int            mnTknOfst, mxTknOfst;
	int            mnNtOfst, mxNtOfst;
	struct         axset *ax;

	lemp->minShiftReduce = lemp->nstate;
	lemp->errAction      = lemp->minShiftReduce + lemp->nrule;
	lemp->accAction      = lemp->errAction + 1;
	lemp->noAction       = lemp->accAction + 1;
	lemp->minReduce      = lemp->noAction + 1;
	lemp->maxAction      = lemp->minReduce + lemp->nrule;

	RES_HANDLE hTmplt = tplt_open(lemp);

	if (0==hTmplt || (0==hTmplt->base && hTmplt->fp == 0))
		return;

	out = file_open(lemp, ".c", "wb");

	if (out == 0)
	{
		if (hTmplt->fp)
			fclose(hTmplt->fp);
		else
			free(hTmplt);
		return;
	}

	lineno = 1;

	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate the include code, if any
	 */
	tplt_print(out, lemp, lemp->include, &lineno);

	if (mhflag)
	{
		char *incName = file_makename(lemp, ".h");

		fprintf(out, "#include \"%s\"\n", incName);
		lineno++;

		free(incName);
	}

	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate #defines for all tokens
	 */
	if (mhflag)
	{
		const char *prefix;

		fprintf(out, "#if INTERFACE\n");
		lineno++;

		if (lemp->tokenprefix)
			prefix = lemp->tokenprefix;
		else
			prefix = "";

		for (i = 1; i < lemp->nterminal; i++)
		{
			fprintf(out, "#define %s%-30s %2d\n", prefix, lemp->symbols[i]->name, i);
			lineno++;
		}

		fprintf(out, "#endif\n");
		lineno++;
	}

	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate the defines
	 */
	fprintf(out, "#define YYCODETYPE %s\n", minimum_size_type(0, lemp->nsymbol, &szCodeType));
	lineno++;

	fprintf(out, "#define YYNOCODE %d\n", lemp->nsymbol);
	lineno++;

	fprintf(out, "#define YYACTIONTYPE %s\n", minimum_size_type(0, lemp->maxAction, &szActionType));
	lineno++;

	if (lemp->wildcard)
	{
		fprintf(out, "#define YYWILDCARD %d\n", lemp->wildcard->index);
		lineno++;
	}

	print_stack_union(out, lemp, &lineno, mhflag);

	fprintf(out, "#ifndef YYSTACKDEPTH\n");
	lineno++;

	if (lemp->stacksize)
	{
		fprintf(out, "#define YYSTACKDEPTH %s\n", lemp->stacksize);
		lineno++;
	}
	else
	{
		fprintf(out, "#define YYSTACKDEPTH 100\n");
		lineno++;
	}

	fprintf(out, "#endif\n");
	lineno++;

	if (mhflag)
	{
		fprintf(out, "#if INTERFACE\n");
		lineno++;
	}

	name = lemp->name ? lemp->name : "Parse";

	if (lemp->arg && lemp->arg[0])
	{
		i = lemonStrlen(lemp->arg);

		while (i >= 1 && ISSPACE(lemp->arg[i - 1]))
			i--;

		while (i >= 1 && (ISALNUM(lemp->arg[i - 1]) || lemp->arg[i - 1] == '_'))
			i--;

		fprintf(out, "#define %sARG_SDECL %s;\n", name, lemp->arg);
		lineno++;

		fprintf(out, "#define %sARG_PDECL ,%s\n", name, lemp->arg);
		lineno++;

		fprintf(out, "#define %sARG_PARAM ,%s\n", name, &lemp->arg[i]);
		lineno++;

		fprintf(out, "#define %sARG_FETCH %s=yypParser->%s;\n", name, lemp->arg, &lemp->arg[i]);
		lineno++;

		fprintf(out, "#define %sARG_STORE yypParser->%s=%s;\n", name, &lemp->arg[i], &lemp->arg[i]);
		lineno++;
	}
	else
	{
		fprintf(out, "#define %sARG_SDECL\n", name);
		lineno++;

		fprintf(out, "#define %sARG_PDECL\n", name);
		lineno++;

		fprintf(out, "#define %sARG_PARAM\n", name);
		lineno++;

		fprintf(out, "#define %sARG_FETCH\n", name);
		lineno++;

		fprintf(out, "#define %sARG_STORE\n", name);
		lineno++;
	}

	if (lemp->ctx && lemp->ctx[0])
	{
		i = lemonStrlen(lemp->ctx);

		while (i >= 1 && ISSPACE(lemp->ctx[i - 1]))
			i--;

		while (i >= 1 && (ISALNUM(lemp->ctx[i - 1]) || lemp->ctx[i - 1] == '_'))
			i--;

		fprintf(out, "#define %sCTX_SDECL %s;\n", name, lemp->ctx);
		lineno++;

		fprintf(out, "#define %sCTX_PDECL ,%s\n", name, lemp->ctx);
		lineno++;

		fprintf(out, "#define %sCTX_PARAM ,%s\n", name, &lemp->ctx[i]);
		lineno++;

		fprintf(out, "#define %sCTX_FETCH %s=yypParser->%s;\n", name, lemp->ctx, &lemp->ctx[i]);
		lineno++;

		fprintf(out, "#define %sCTX_STORE yypParser->%s=%s;\n", name, &lemp->ctx[i], &lemp->ctx[i]);
		lineno++;
	}
	else
	{
		fprintf(out, "#define %sCTX_SDECL\n", name);
		lineno++;

		fprintf(out, "#define %sCTX_PDECL\n", name);
		lineno++;

		fprintf(out, "#define %sCTX_PARAM\n", name);
		lineno++;

		fprintf(out, "#define %sCTX_FETCH\n", name);
		lineno++;

		fprintf(out, "#define %sCTX_STORE\n", name);
		lineno++;
	}

	if (mhflag)
	{
		fprintf(out, "#endif\n");
		lineno++;
	}

	if (lemp->errsym && lemp->errsym->useCnt)
	{
		fprintf(out, "#define YYERRORSYMBOL %d\n", lemp->errsym->index);
		lineno++;

		fprintf(out, "#define YYERRSYMDT yy%d\n", lemp->errsym->dtnum);
		lineno++;
	}

	if (lemp->has_fallback)
	{
		fprintf(out, "#define YYFALLBACK 1\n");
		lineno++;
	}

	/*
	 * Compute the action table, but do not output it yet.\n
	 * The action table must be computed before generating the YYNSTATE
	 * macro because we need to know how many states can be eliminated.
	 */
	ax = (struct axset *)calloc(lemp->nxstate * 2, sizeof(ax[0]));

	if (ax == 0)
	{
		fprintf(stderr, "malloc failed\n");
		exit(1);
	}

	for (i = 0; i < lemp->nxstate; i++)
	{
		stp                   = lemp->sorted[i];
		ax[i * 2].stp         = stp;
		ax[i * 2].isTkn       = 1;
		ax[i * 2].nAction     = stp->nTknAct;
		ax[i * 2 + 1].stp     = stp;
		ax[i * 2 + 1].isTkn   = 0;
		ax[i * 2 + 1].nAction = stp->nNtAct;
	}

	mxTknOfst = mnTknOfst = 0;
	mxNtOfst  = mnNtOfst  = 0;

	/*
	 * In an effort to minimize the action table size, use the heuristic
	 * of placing the largest action sets first
	 */
	for (i = 0; i < lemp->nxstate * 2; i++)
		ax[i].iOrder = i;

	qsort(ax, lemp->nxstate * 2, sizeof(ax[0]), axset_compare);

	pActtab = acttab_alloc(lemp->nsymbol, lemp->nterminal);

	for (i = 0; i < lemp->nxstate * 2 && ax[i].nAction > 0; i++)
	{
		stp = ax[i].stp;

		if (ax[i].isTkn)
		{
			for (ap = stp->ap; ap; ap = ap->next)
			{
				int action;

				if (ap->sp->index >= lemp->nterminal)
					continue;

				action = compute_action(lemp, ap);

				if (action < 0)
					continue;

				acttab_action(pActtab, ap->sp->index, action);
			}

			stp->iTknOfst = acttab_insert(pActtab, 1);

			if (stp->iTknOfst < mnTknOfst)
				mnTknOfst = stp->iTknOfst;

			if (stp->iTknOfst > mxTknOfst)
				mxTknOfst = stp->iTknOfst;
		}
		else
		{
			for (ap = stp->ap; ap; ap = ap->next)
			{
				int action;

				if (ap->sp->index < lemp->nterminal)
					continue;

				if (ap->sp->index == lemp->nsymbol)
					continue;

				action = compute_action(lemp, ap);

				if (action < 0)
					continue;

				acttab_action(pActtab, ap->sp->index, action);
			}

			stp->iNtOfst = acttab_insert(pActtab, 0);

			if (stp->iNtOfst < mnNtOfst)
				mnNtOfst = stp->iNtOfst;

			if (stp->iNtOfst > mxNtOfst)
				mxNtOfst = stp->iNtOfst;
		}

#if 0
	/*
	 * Uncomment for a trace of how the yy_action[] table fills out
	 */
		{
			int jj, nn;

			for (jj = nn = 0; jj < pActtab->nAction; jj++)
			{
				if (pActtab->aAction[jj].action < 0)
					nn++;
			}

			printf("%4d: State %3d %s n: %2d size: %5d freespace: %d\n",
						i, stp->statenum, ax[i].isTkn ? "Token" : "Var  ",
										ax[i].nAction, pActtab->nAction, nn);
		}
#endif
	}

	free(ax);

	/*
	 * Mark rules that are actually used for reduce actions after all
	 * optimizations have been applied
	 */
	for (rp = lemp->rule; rp; rp = rp->next)
		rp->doesReduce = LEMON_FALSE;

	for (i = 0; i < lemp->nxstate; i++)
	{
		for (ap = lemp->sorted[i]->ap; ap; ap = ap->next)
		{
			if (ap->type == REDUCE || ap->type == SHIFTREDUCE)
			{
				ap->x.rp->doesReduce = 1;
			}
		}
	}

	/*
	 * Finish rendering the constants now that the action table has
	 * been computed
	 */
	fprintf(out, "#define YYNSTATE             %d\n", lemp->nxstate);
	lineno++;

	fprintf(out, "#define YYNRULE              %d\n", lemp->nrule);
	lineno++;

	fprintf(out, "#define YYNTOKEN             %d\n", lemp->nterminal);
	lineno++;

	fprintf(out, "#define YY_MAX_SHIFT         %d\n", lemp->nxstate - 1);
	lineno++;

	i = lemp->minShiftReduce;

	fprintf(out, "#define YY_MIN_SHIFTREDUCE   %d\n", i);
	lineno++;

	i += lemp->nrule;

	fprintf(out, "#define YY_MAX_SHIFTREDUCE   %d\n", i - 1);
	lineno++;

	fprintf(out, "#define YY_ERROR_ACTION      %d\n", lemp->errAction);
	lineno++;

	fprintf(out, "#define YY_ACCEPT_ACTION     %d\n", lemp->accAction);
	lineno++;

	fprintf(out, "#define YY_NO_ACTION         %d\n", lemp->noAction);
	lineno++;

	fprintf(out, "#define YY_MIN_REDUCE        %d\n", lemp->minReduce);
	lineno++;

	i = lemp->minReduce + lemp->nrule;

	fprintf(out, "#define YY_MAX_REDUCE        %d\n", i - 1);
	lineno++;

	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Now output the action table and its associates:
	 *
	 *  yy_action[]        A single table containing all actions.
	 *  yy_lookahead[]     A table containing the lookahead for each entry in
	 *                     yy_action.  Used to detect hash collisions.
	 *  yy_shift_ofst[]    For each state, the offset into yy_action for
	 *                     shifting terminals.
	 *  yy_reduce_ofst[]   For each state, the offset into yy_action for
	 *                     shifting non-terminals after a reduce.
	 *  yy_default[]       Default action for each state.
	 */

	/*
	 * Output the yy_action table
	 */
	lemp->nactiontab = n = acttab_action_size(pActtab);
	lemp->tablesize += n * szActionType;

	fprintf(out, "#define YY_ACTTAB_COUNT (%d)\n", n);
	lineno++;

	fprintf(out, "static const YYACTIONTYPE yy_action[] = {\n");
	lineno++;

	for (i = j = 0; i < n; i++)
	{
		int action = acttab_yyaction(pActtab, i);

		if (action < 0)
			action = lemp->noAction;

		if (j == 0)
			fprintf(out, " /* %5d */ ", i);

		fprintf(out, " %4d,", action);

		if (j == 9 || i == n - 1)
		{
			fprintf(out, "\n");
			lineno++;
			j = 0;
		}
		else
		{
			j++;
		}
	}

	fprintf(out, "};\n");
	lineno++;

	/*
	 * Output the yy_lookahead table
	 */
	lemp->nlookaheadtab = n = acttab_lookahead_size(pActtab);
	lemp->tablesize    += n * szCodeType;

	fprintf(out, "static const YYCODETYPE yy_lookahead[] = {\n");
	lineno++;

	for (i = j = 0; i < n; i++)
	{
		int la = acttab_yylookahead(pActtab, i);

		if (la < 0)
			la = lemp->nsymbol;

		if (j == 0)
			fprintf(out, " /* %5d */ ", i);

		fprintf(out, " %4d,", la);

		if (j == 9 || i == n - 1)
		{
			fprintf(out, "\n");
			lineno++;
			j = 0;
		}
		else
		{
			j++;
		}
	}

	fprintf(out, "};\n");
	lineno++;

	/*
	 * Output the yy_shift_ofst[] table
	 */
	n = lemp->nxstate;

	while (n > 0 && lemp->sorted[n - 1]->iTknOfst == NO_OFFSET)
		n--;

	fprintf(out, "#define YY_SHIFT_COUNT    (%d)\n", n - 1);
	lineno++;

	fprintf(out, "#define YY_SHIFT_MIN      (%d)\n", mnTknOfst);
	lineno++;

	fprintf(out, "#define YY_SHIFT_MAX      (%d)\n", mxTknOfst);
	lineno++;

	fprintf(out, "static const %s yy_shift_ofst[] = {\n", minimum_size_type(mnTknOfst, lemp->nterminal + lemp->nactiontab, &sz));
	lineno++;

	lemp->tablesize += n * sz;

	for (i = j = 0; i < n; i++)
	{
		int ofst;

		stp  = lemp->sorted[i];
		ofst = stp->iTknOfst;

		if (ofst == NO_OFFSET)
			ofst = lemp->nactiontab;

		if (j == 0)
			fprintf(out, " /* %5d */ ", i);

		fprintf(out, " %4d,", ofst);

		if (j == 9 || i == n - 1)
		{
			fprintf(out, "\n");
			lineno++;
			j = 0;
		}
		else
		{
			j++;
		}
	}

	fprintf(out, "};\n");
	lineno++;

	/*
	 * Output the yy_reduce_ofst[] table
	 */
	n = lemp->nxstate;

	while (n > 0 && lemp->sorted[n - 1]->iNtOfst == NO_OFFSET)
		n--;

	fprintf(out, "#define YY_REDUCE_COUNT (%d)\n", n - 1);
	lineno++;

	fprintf(out, "#define YY_REDUCE_MIN   (%d)\n", mnNtOfst);
	lineno++;

	fprintf(out, "#define YY_REDUCE_MAX   (%d)\n", mxNtOfst);
	lineno++;

	fprintf(out, "static const %s yy_reduce_ofst[] = {\n", minimum_size_type(mnNtOfst - 1, mxNtOfst, &sz));
	lineno++;

	lemp->tablesize += n * sz;

	for (i = j = 0; i < n; i++)
	{
		int ofst;

		stp  = lemp->sorted[i];
		ofst = stp->iNtOfst;

		if (ofst == NO_OFFSET)
			ofst = mnNtOfst - 1;

		if (j == 0)
			fprintf(out, " /* %5d */ ", i);

		fprintf(out, " %4d,", ofst);

		if (j == 9 || i == n - 1)
		{
			fprintf(out, "\n");
			lineno++;
			j = 0;
		}
		else
		{
			j++;
		}
	}

	fprintf(out, "};\n");
	lineno++;

	/*
	 * Output the default action table
	 */
	fprintf(out, "static const YYACTIONTYPE yy_default[] = {\n");
	lineno++;

	n = lemp->nxstate;

	lemp->tablesize += n * szActionType;

	for (i = j = 0; i < n; i++)
	{
		stp = lemp->sorted[i];

		if (j == 0)
			fprintf(out, " /* %5d */ ", i);

		if (stp->iDfltReduce < 0)
		{
			fprintf(out, " %4d,", lemp->errAction);
		}
		else
		{
			fprintf(out, " %4d,", stp->iDfltReduce + lemp->minReduce);
		}

		if (j == 9 || i == n - 1)
		{
			fprintf(out, "\n");
			lineno++;
			j = 0;
		}
		else
		{
			j++;
		}
	}

	fprintf(out, "};\n");
	lineno++;

	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate the table of fallback tokens.
	 */
	if (lemp->has_fallback)
	{
		int mx = lemp->nterminal - 1;

		while (mx > 0 && lemp->symbols[mx]->fallback == 0)
		{
			mx--;
		}

		lemp->tablesize += (mx + 1) * szCodeType;

		for (i = 0; i <= mx; i++)
		{
			struct symbol *p = lemp->symbols[i];

			if (p->fallback == 0)
			{
				fprintf(out, "    0,  /* %10s => nothing */\n", p->name);
			}
			else
			{
				fprintf(out, "  %3d,  /* %10s => %s */\n",
							p->fallback->index, p->name, p->fallback->name);
			}
			lineno++;
		}
	}

	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate a table containing the symbolic name of every symbol
	 */
	for (i = 0; i < lemp->nsymbol; i++)
	{
		lemon_sprintf(line, "\"%s\",", lemp->symbols[i]->name);
		fprintf(out, "  /* %4d */ \"%s\",\n", i, lemp->symbols[i]->name);
		lineno++;
	}

	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate a table containing a text string that describes every
	 * rule in the rule set of the grammar.\n
	 * This information is used when tracing REDUCE actions.
	 */
	for (i = 0, rp = lemp->rule; rp; rp = rp->next, i++)
	{
		assert(rp->iRule == i);
		fprintf(out, " /* %3d */ \"", i);
		writeRuleText(out, rp);
		fprintf(out, "\",\n");
		lineno++;
	}

	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate code which executes every time a symbol is popped from
	 * the stack while processing errors or while destroying the parser.
	 * (In other words, generate the %destructor actions)
	 */
	if (lemp->tokendest)
	{
		int once = 1;

		for (i = 0; i < lemp->nsymbol; i++)
		{
			struct symbol *sp = lemp->symbols[i];

			if (sp == 0 || sp->type != TERMINAL)
				continue;

			if (once)
			{
				fprintf(out, "      /* TERMINAL Destructor */\n");
				lineno++;
				once = 0;
			}

			fprintf(out, "    case %d: /* %s */\n", sp->index, sp->name);
			lineno++;
		}

		for (i = 0; i < lemp->nsymbol && lemp->symbols[i]->type != TERMINAL; i++)
			;

		if (i < lemp->nsymbol)
		{
			emit_destructor_code(out, lemp->symbols[i], lemp, &lineno);

			fprintf(out, "      break;\n");
			lineno++;
		}
	}

	if (lemp->vardest)
	{
		struct symbol *dflt_sp = 0;
		int            once    = 1;

		for (i = 0; i < lemp->nsymbol; i++)
		{
			struct symbol *sp = lemp->symbols[i];

			if (sp == 0 || sp->type == TERMINAL || sp->index <= 0 || sp->destructor != 0)
				continue;

			if (once)
			{
				fprintf(out, "      /* Default NON-TERMINAL Destructor */\n");
				lineno++;
				once = 0;
			}

			fprintf(out, "    case %d: /* %s */\n", sp->index, sp->name);
			lineno++;
			dflt_sp = sp;
		}

		if (dflt_sp != 0)
		{
			emit_destructor_code(out, dflt_sp, lemp, &lineno);
		}

		fprintf(out, "      break;\n");
		lineno++;
	}

	for (i = 0; i < lemp->nsymbol; i++)
	{
		struct symbol *sp = lemp->symbols[i];

		if (sp == 0 || sp->type == TERMINAL || sp->destructor == 0)
			continue;

		if (sp->destLineno < 0)
			continue;	/* Already emitted */

		fprintf(out, "    case %d: /* %s */\n", sp->index, sp->name);
		lineno++;

		/*
		 * Combine duplicate destructors into a single case
		 */
		for (j = i + 1; j < lemp->nsymbol; j++)
		{
			struct symbol *sp2 = lemp->symbols[j];

			if (sp2 && sp2->type != TERMINAL && sp2->destructor && sp2->dtnum == sp->dtnum && strcmp(sp->destructor, sp2->destructor) == 0)
			{
				fprintf(out, "    case %d: /* %s */\n", sp2->index, sp2->name);
				lineno++;
				sp2->destLineno = -1;	/* Avoid emitting this destructor again */
			}
		}

		emit_destructor_code(out, lemp->symbols[i], lemp, &lineno);

		fprintf(out, "      break;\n");
		lineno++;
	}

	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate code which executes whenever the parser stack overflows
	 */
	tplt_print(out, lemp, lemp->overflow, &lineno);
	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate the table of rule information
	 *
	 * Note: This code depends on the fact that rules are number
	 * sequentually beginning with 0.
	 */
	for (i = 0, rp = lemp->rule; rp; rp = rp->next, i++)
	{
		fprintf(out, "  { %4d, %4d }, /* (%d) ", rp->lhs->index, -rp->nrhs, i);

		rule_print(out, rp);

		fprintf(out, " */\n");
		lineno++;
	}

	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate code which execution during each REDUCE action
	 */
	i = 0;
	for (rp = lemp->rule; rp; rp = rp->next)
	{
		i += translate_code(lemp, rp);
	}

	if (i)
	{
		fprintf(out, "        YYMINORTYPE yylhsminor;\n");
		lineno++;
	}

	/*
	 * First output rules other than the default: rule
	 */
	for (rp = lemp->rule; rp; rp = rp->next)
	{
		struct rule *rp2;	/* Other rules with the same action */

		if (rp->codeEmitted)
			continue;

		if (rp->noCode)
		{
			/*
			 * No C code actions, so this will be part of the "default:" rule
			 */
			continue;
		}

		fprintf(out, "      case %d: /* ", rp->iRule);

		writeRuleText(out, rp);

		fprintf(out, " */\n");
		lineno++;

		for (rp2 = rp->next; rp2; rp2 = rp2->next)
		{
			if (rp2->code == rp->code && rp2->codePrefix == rp->codePrefix && rp2->codeSuffix == rp->codeSuffix)
			{
				fprintf(out, "      case %d: /* ", rp2->iRule);

				writeRuleText(out, rp2);

				fprintf(out, " */ yytestcase(yyruleno==%d);\n", rp2->iRule);
				lineno++;

				rp2->codeEmitted = 1;
			}
		}

		emit_code(out, rp, lemp, &lineno);

		fprintf(out, "        break;\n");
		lineno++;

		rp->codeEmitted = 1;
	}

	/*
	 * Finally, output the default: rule.
	 * We choose as the default: all empty actions.
	 */
	fprintf(out, "      default:\n");
	lineno++;

	for (rp = lemp->rule; rp; rp = rp->next)
	{
		if (rp->codeEmitted)
			continue;

		assert(rp->noCode);

		fprintf(out, "      /* (%d) ", rp->iRule);

		writeRuleText(out, rp);

		if (rp->doesReduce)
		{
			fprintf(out, " */ yytestcase(yyruleno==%d);\n", rp->iRule);
			lineno++;
		}
		else
		{
			fprintf(out, " (OPTIMIZED OUT) */ assert(yyruleno!=%d);\n", rp->iRule);
			lineno++;
		}
	}

	fprintf(out, "        break;\n");
	lineno++;

	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate code which executes if a parse fails
	 */
	tplt_print(out, lemp, lemp->failure, &lineno);
	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate code which executes when a syntax error occurs
	 */
	tplt_print(out, lemp, lemp->error, &lineno);
	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Generate code which executes when the parser accepts its input
	 */
	tplt_print(out, lemp, lemp->accept, &lineno);
	tplt_xfer(lemp->name, hTmplt, out, &lineno);

	/*
	 * Append any addition code the user desires
	 */
	tplt_print(out, lemp, lemp->extracode, &lineno);

	if(hTmplt->fp)
		fclose(hTmplt->fp);
	else
		free(hTmplt);
	fclose(out);

	return;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		ReportHeader
 \details	Generate a header file for the parser
 \date		Created  on Sat Sep  1 21:17:05 2018
 \date		Modified on Sat Sep  1 21:17:05 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void ReportHeader(struct lemon *lemp)
{
	const char *prefix;
	int         i;

	char ExtraArg[1024];

	if (lemp->arg)
	{
		sprintf(ExtraArg, "%sExtraArg", lemp->name ? lemp->name : "Parse");
	}

	if (lemp->tokenprefix)
		prefix = lemp->tokenprefix;
	else
		prefix = "";

	FILE *out = file_open(lemp, ".h", "wb");

	if (out)
	{
		char *filename = strrchr(lemp->filename, '\\');

		if (filename)
			filename++;
		else
			filename = lemp->filename;

		char *p = strchr(filename, '.');

		int lim = p ? p - filename : strlen(filename);

#ifdef __POCC__
#include <time.h>
#endif
		time_t t = time(NULL);

		fprintf(out, "/*\n"
					 " *\tfLemon - frankie Lemon parser.\n"
					 " *\tfile \"%.*s.h\"\n"
					 " *\tBuilt on: %s"
					 " *\tSource file: %s\n"
					 " *\tAuto-generated Lemon parser header.\n"
					 " *\tWarning: Don't hack by hand!\n"
					 " *\t         Instead modify suorce and recompile it.\n"
					 " *\n"
					 " *\tParser statistics:\n"
					 " *\t  terminal symbols        : %7d\n"
					 " *\t  non-terminal symbols    : %7d\n"
					 " *\t  total symbols           : %7d\n"
					 " *\t  rules                   : %7d\n"
					 " *\t  states                  : %7d\n"
					 " *\t  conflicts               : %7d\n"
					 " *\t  action table entries    : %7d\n"
					 " *\t  lookahead table entries : %7d\n"
					 " *\t  total table size (bytes): %7d\n"
					 " */\n" "#ifndef %.*s_h__\n"
					 "#define %.*s_h__\n"
					 "#pragma once\n\n", lim, filename, ctime(&t), filename,
					 lemp->nterminal, lemp->nsymbol - lemp->nterminal, lemp->nsymbol,
					 lemp->nrule, lemp->nxstate, lemp->nconflict, lemp->nactiontab,
					 lemp->nlookaheadtab, lemp->tablesize,
					 lim, filename, lim, filename);

		/*
		 * frankie: Emit an enumeration or #defines
		 */
		if (lemp->emitenumflag)
		{
			fprintf(out, "typedef enum\n{\n");
			for (i = 1; i < lemp->nterminal; i++)
			{
				fprintf(out, "\t%s%-16s = %5d,\n", prefix, lemp->symbols[i]->name, i);
			}
			fprintf(out, "} e%sTOKENS;\n", prefix);
		}
		else
		{
			for (i = 1; i < lemp->nterminal; i++)
			{
				fprintf(out, "#define %s%-30s %2d\n", prefix, lemp->symbols[i]->name, i);
			}
		}

		fprintf(out, "\n\n/*  Parser variables and functions definition  */\n");

		if (lemp->arg)
		{
			fprintf(out, "#define %s %s\n", ExtraArg, lemp->arg);
		}

		if (lemp->tokentype)
			fprintf(out, "#define %sTokenType %s \n",
							lemp->name ? lemp->name : "Parse", lemp->tokentype);

		if (lemp->vartype)
			fprintf(out, "#define %sNonTerminalDefaultType %s\n",
								lemp->name ? lemp->name : "Parse", lemp->vartype);

		for (i = lemp->nterminal; i < lemp->nsymbol; i++)
		{
			if (lemp->symbols[i]->type == NONTERMINAL && lemp->symbols[i]->datatype)
				fprintf(out, "#define t%s_%s %s\n",
							lemp->name ? lemp->name : "Parse",
							lemp->symbols[i]->name, lemp->symbols[i]->datatype);
		}

		fprintf(out, "void  %sParse(void *,  int,  %s%s%s);\n",
					lemp->name ? lemp->name : "", lemp->tokentype ? lemp->tokentype : "int",
												lemp->arg ? ", " : "", lemp->arg ? ExtraArg : "");
		fprintf(out, "void  %sTrace(FILE *, char *);\n",	
							lemp->name ? lemp->name : "Parse");
		fprintf(out, "void *%sAlloc(void *(*mallocProc)(size_t));\n",
										lemp->name ? lemp->name : "Parse");
		fprintf(out, "void  %sFree( void *,  void (*freeProc)(void*) );\n",
											lemp->name ? lemp->name : "Parse");

		fprintf(out, "\n#endif\t//%.*s_h__\n", lim, filename);

		fclose(out);
	}
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		CompressTables
 \details	Reduce the size of the action tables, if possible, by making 
            useof defaults.\n
            In this version, we take the most frequent REDUCE action and 
            make it the default.\n
            Except, there is no default if the wildcard token is a possible 
            look-ahead.
 \date		Created  on Sat Sep  1 21:25:33 2018
 \date		Modified on Sat Sep  1 21:25:33 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void CompressTables(struct lemon *lemp)
{
	struct state  *stp;
	struct action *ap, *ap2, *nextap;
	struct rule   *rp, *rp2, *rbest;
	int            nbest, n;
	int            i;
	int            usesWildcard;

	for (i = 0; i < lemp->nstate; i++)
	{
		stp          = lemp->sorted[i];
		nbest        = 0;
		rbest        = 0;
		usesWildcard = 0;

		for (ap = stp->ap; ap; ap = ap->next)
		{
			if (ap->type == SHIFT && ap->sp == lemp->wildcard)
			{
				usesWildcard = 1;
			}

			if (ap->type != REDUCE)
				continue;

			rp = ap->x.rp;

			if (rp->lhsStart)
				continue;

			if (rp == rbest)
				continue;

			n = 1;

			for (ap2 = ap->next; ap2; ap2 = ap2->next)
			{
				if (ap2->type != REDUCE)
					continue;

				rp2 = ap2->x.rp;

				if (rp2 == rbest)
					continue;

				if (rp2 == rp)
					n++;
			}

			if (n > nbest)
			{
				nbest = n;
				rbest = rp;
			}
		}

		/*
		 * Do not make a default if the number of rules to default
		 * is not at least 1 or if the wildcard token is a possible
		 * lookahead.
		 */
		if (nbest < 1 || usesWildcard)
			continue;

		/*
		 * Combine matching REDUCE actions into a single default
		 */
		for (ap = stp->ap; ap; ap = ap->next)
		{
			if (ap->type == REDUCE && ap->x.rp == rbest)
				break;
		}

		assert(ap);

		ap->sp = Symbol_new("{default}");

		for (ap = ap->next; ap; ap = ap->next)
		{
			if (ap->type == REDUCE && ap->x.rp == rbest)
				ap->type = NOT_USED;
		}

		stp->ap = Action_sort(stp->ap);

		for (ap = stp->ap; ap; ap = ap->next)
		{
			if (ap->type == SHIFT)
				break;

			if (ap->type == REDUCE && ap->x.rp != rbest)
				break;
		}

		if (ap == 0)
		{
			stp->autoReduce = 1;
			stp->pDfltReduce = rbest;
		}
	}

	/*
	 * Make a second pass over all states and actions.
	 * Convert every action that is a SHIFT to an autoReduce state into
	 * a SHIFTREDUCE action.
	 */
	for (i = 0; i < lemp->nstate; i++)
	{
		stp = lemp->sorted[i];

		for (ap = stp->ap; ap; ap = ap->next)
		{
			struct state *pNextState;

			if (ap->type != SHIFT)
				continue;

			pNextState = ap->x.stp;

			if (pNextState->autoReduce && pNextState->pDfltReduce != 0)
			{
				ap->type = SHIFTREDUCE;
				ap->x.rp = pNextState->pDfltReduce;
			}
		}
	}

	/*
	 * If a SHIFTREDUCE action specifies a rule that has a single RHS term
	 * (meaning that the SHIFTREDUCE will land back in the state where it
	 * started) and if there is no C-code associated with the reduce action,
	 * then we can go ahead and convert the action to be the same as the
	 * action for the RHS of the rule.
	 */
	for (i = 0; i < lemp->nstate; i++)
	{
		stp = lemp->sorted[i];

		for (ap = stp->ap; ap; ap = nextap)
		{
			nextap = ap->next;

			if (ap->type != SHIFTREDUCE)
				continue;

			rp = ap->x.rp;

			if (rp->noCode == 0)
				continue;

			if (rp->nrhs != 1)
				continue;
#if 1
			/*
			 * Only apply this optimization to non-terminals.
			 * It would be OK to apply it to terminal symbols too,
			 * but that makes the parser tables larger.
			 */
			if (ap->sp->index < lemp->nterminal)
				continue;
#endif
			/*
			 * If we reach this point, it means the optimization can be applied
			 */
			nextap = ap;

			for (ap2 = stp->ap; ap2 &&
						(ap2 == ap || ap2->sp != rp->lhs); ap2 = ap2->next)
			{
			}

			assert(ap2 != 0);

			ap->spOpt = ap2->sp;
			ap->type  = ap2->type;
			ap->x     = ap2->x;
		}
	}
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		stateResortCompare
 \details	Compare two states for sorting purposes.\n
            The smaller state is the one with the most non-terminal 
            actions.\n
            If they have the same number of non-terminal actions, then the 
            smaller is the one with the most token actions.
 \date		Created  on Sat Sep  1 21:30:40 2018
 \date		Modified on Sat Sep  1 21:30:40 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
static int stateResortCompare(const void *a, const void *b)
{
	const struct state *pA = *(const struct state **)a;
	const struct state *pB = *(const struct state **)b;
	int          n;

	n = pB->nNtAct - pA->nNtAct;

	if (n == 0)
	{
		n = pB->nTknAct - pA->nTknAct;

		if (n == 0)
		{
			n = pB->statenum - pA->statenum;
		}
	}

	assert(n != 0);

	return n;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		ResortStates
 \details	Renumber and resort states so that states with fewer choices 
            occur at the end.\n
            Except, keep state 0 as the first state.
 \date		Created  on Sat Sep  1 21:31:45 2018
 \date		Modified on Sat Sep  1 21:31:45 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void ResortStates(struct lemon *lemp)
{
	int            i;
	struct state  *stp;
	struct action *ap;

	for (i = 0; i < lemp->nstate; i++)
	{
		stp              = lemp->sorted[i];
		stp->nTknAct     = stp->nNtAct = 0;
		stp->iDfltReduce = -1;					/* Init dflt action to "syntax error" */
		stp->iTknOfst    = NO_OFFSET;
		stp->iNtOfst     = NO_OFFSET;

		for (ap = stp->ap; ap; ap = ap->next)
		{
			int iAction = compute_action(lemp, ap);

			if (iAction >= 0)
			{
				if (ap->sp->index < lemp->nterminal)
				{
					stp->nTknAct++;
				}
				else if (ap->sp->index < lemp->nsymbol)
				{
					stp->nNtAct++;
				}
				else
				{
					assert(stp->autoReduce == 0 || stp->pDfltReduce == ap->x.rp);
					stp->iDfltReduce = iAction;
				}
			}
		}
	}

	qsort(&lemp->sorted[1], lemp->nstate - 1,
						sizeof(lemp->sorted[0]), stateResortCompare);

	for (i = 0; i < lemp->nstate; i++)
	{
		lemp->sorted[i]->statenum = i;
	}

	lemp->nxstate = lemp->nstate;

	while (lemp->nxstate > 1 && lemp->sorted[lemp->nxstate - 1]->autoReduce)
	{
		lemp->nxstate--;
	}
}
