/*+@@file@@----------------------------------------------------------------*//*!
 \file		action.c
 \par Description 
            Routines processing parser actions in the LEMON parser 
            generator.
 \par  Status: 
            
 \par Project: 
            Lemon parser
 \date		Created  on Sat Sep  1 17:21:24 2018
 \date		Modified on Sat Sep  1 17:21:24 2018
 \author	
\*//*-@@file@@----------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include "struct.h"
#include "action.h"

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Action_new
 \details	Allocate a new parser action
 \date		Created  on Sat Sep  1 17:23:15 2018
 \date		Modified on Sat Sep  1 17:23:15 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
static struct action *Action_new(void)
{
	static struct action *freelist = 0;
	struct action *newaction;

	if (freelist == 0)
	{
		int i;
		int amt = 100;
		freelist = (struct action *)calloc(amt, sizeof(struct action));
		if (freelist == 0)
		{
			fprintf(stderr, "Unable to allocate memory for a new parser action.");
			exit(1);
		}
		for (i = 0; i < amt - 1; i++)
			freelist[i].next = &freelist[i + 1];
		freelist[amt - 1].next = 0;
	}
	newaction = freelist;
	freelist = freelist->next;
	return newaction;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		actioncmp
 \details	Compare two actions for sorting purposes.
 \return	Return negative, zero, or positive if the first action is less 
            than, equal to, or greater than the first
 \date		Created  on Sat Sep  1 17:24:15 2018
 \date		Modified on Sat Sep  1 17:24:15 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
static int actioncmp(struct action *ap1, struct action *ap2)
{
	int rc;
	rc = ap1->sp->index - ap2->sp->index;
	if (rc == 0)
	{
		rc = (int)ap1->type - (int)ap2->type;
	}
	if (rc == 0 && (ap1->type == REDUCE || ap1->type == SHIFTREDUCE))
	{
		rc = ap1->x.rp->index - ap2->x.rp->index;
	}
	if (rc == 0)
	{
		rc = (int)(ap2 - ap1);
	}
	return rc;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Action_sort
 \details	Sort parser actions
 \date		Created  on Sat Sep  1 17:24:34 2018
 \date		Modified on Sat Sep  1 17:24:34 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
struct action *Action_sort(struct action *ap)
{
	ap = (struct action *)msort((char *)ap, (char **)&ap->next,
			(int (*)(const char *, const char *))actioncmp);
	return ap;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Action_add
 \date		Created  on Sat Sep  1 17:25:02 2018
 \date		Modified on Sat Sep  1 17:25:02 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void Action_add(struct action **app, enum e_action type, struct symbol *sp, char *arg)
{
	struct action *newaction;
	newaction = Action_new();
	newaction->next = *app;
	*app = newaction;
	newaction->type = type;
	newaction->sp = sp;
	newaction->spOpt = 0;
	if (type == SHIFT)
	{
		newaction->x.stp = (struct state *)arg;
	}
	else
	{
		newaction->x.rp = (struct rule *)arg;
	}
}
/*
 * *********** New code to implement the "acttab" module **********
 *
 * This module implements routines use to construct the yy_action[] table.
 */

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		acttab_free
 \details	Free all memory associated with the given acttab
 \date		Created  on Sat Sep  1 17:34:52 2018
 \date		Modified on Sat Sep  1 17:34:52 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void acttab_free(acttab *p)
{
	free(p->aAction);
	free(p->aLookahead);
	free(p);
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		acttab_alloc
 \details	Allocate a new acttab structure
 \date		Created  on Sat Sep  1 17:35:02 2018
 \date		Modified on Sat Sep  1 17:35:02 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
acttab *acttab_alloc(int nsymbol, int nterminal)
{
	acttab *p = (acttab *)calloc(1, sizeof(*p));
	if (p == 0)
	{
		fprintf(stderr, "Unable to allocate memory for a new acttab.");
		exit(1);
	}
	memset(p, 0, sizeof(*p));
	p->nsymbol = nsymbol;
	p->nterminal = nterminal;
	return p;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		acttab_action
 \details	Add a new action to the current transaction set.\n
            This routine is called once for each lookahead for a particular 
            state.
 \date		Created  on Sat Sep  1 17:35:46 2018
 \date		Modified on Sat Sep  1 17:35:46 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void acttab_action(acttab *p, int lookahead, int action)
{
	if (p->nLookahead >= p->nLookaheadAlloc)
	{
		p->nLookaheadAlloc += 25;
		p->aLookahead = (struct lookahead_action *)realloc(p->aLookahead, sizeof(p->aLookahead[0]) * p->nLookaheadAlloc);
		if (p->aLookahead == 0)
		{
			fprintf(stderr, "malloc failed\n");
			exit(1);
		}
	}
	if (p->nLookahead == 0)
	{
		p->mxLookahead = lookahead;
		p->mnLookahead = lookahead;
		p->mnAction = action;
	}
	else
	{
		if (p->mxLookahead < lookahead)
			p->mxLookahead = lookahead;
		if (p->mnLookahead > lookahead)
		{
			p->mnLookahead = lookahead;
			p->mnAction = action;
		}
	}
	p->aLookahead[p->nLookahead].lookahead = lookahead;
	p->aLookahead[p->nLookahead].action = action;
	p->nLookahead++;
}

/*!
 * 
 */
/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		acttab_insert
 \details	Add the transaction set built up with prior calls to 
            acttab_action() into the current action table.\n
            Then reset the transaction set back to an empty set in 
            preparation for a new round of acttab_action() calls.\n
            If the makeItSafe parameter is true, then the offset is chosen 
            so that it is impossible to overread the yy_lookaside[] table 
            regardless of the lookaside token.\n
            This is done for the terminal symbols, as they come from 
            external inputs and can contain syntax errors.\n
            When makeItSafe is false, there is more flexibility in 
            selecting offsets, resulting in a smaller table.\n
            For non-terminal symbols, which are never syntax errors, 
            makeItSafe can be false.
 \return	Return the offset into the action table of the new transaction.
 \date		Created  on Sat Sep  1 17:40:44 2018
 \date		Modified on Sat Sep  1 17:40:44 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
int acttab_insert(acttab *p, int makeItSafe)
{
	int i, j, k, n, end;
	assert(p->nLookahead > 0);

	/*
	 * Make sure we have enough space to hold the expanded action table
	 * in the worst case.
	 * The worst case occurs if the transaction set must be appended to
	 * the current action table
	 */
	n = p->nsymbol + 1;
	if (p->nAction + n >= p->nActionAlloc)
	{
		int oldAlloc = p->nActionAlloc;
		p->nActionAlloc = p->nAction + n + p->nActionAlloc + 20;
		p->aAction = (struct lookahead_action *)realloc(p->aAction, sizeof(p->aAction[0]) * p->nActionAlloc);
		if (p->aAction == 0)
		{
			fprintf(stderr, "malloc failed\n");
			exit(1);
		}
		for (i = oldAlloc; i < p->nActionAlloc; i++)
		{
			p->aAction[i].lookahead = -1;
			p->aAction[i].action = -1;
		}
	}

	/*
	 * Scan the existing action table looking for an offset that is a
	 * duplicate of the current transaction set.
	 * Fall out of the loop if and when the duplicate is found.
	 *
	 * i is the index in p->aAction[] where p->mnLookahead is inserted.
	 */
	end = makeItSafe ? p->mnLookahead : 0;
	for (i = p->nAction - 1; i >= end; i--)
	{
		if (p->aAction[i].lookahead == p->mnLookahead)
		{
			/*
			 * All lookaheads and actions in the aLookahead[] transaction
			 * must match against the candidate aAction[i] entry.
			 */
			if (p->aAction[i].action != p->mnAction)
				continue;
			for (j = 0; j < p->nLookahead; j++)
			{
				k = p->aLookahead[j].lookahead - p->mnLookahead + i;
				if (k < 0 || k >= p->nAction)
					break;
				if (p->aLookahead[j].lookahead != p->aAction[k].lookahead)
					break;
				if (p->aLookahead[j].action != p->aAction[k].action)
					break;
			}
			if (j < p->nLookahead)
				continue;

			/*
			 * No possible lookahead value that is not in the aLookahead[]
			 * transaction is allowed to match aAction[i]
			 */
			n = 0;
			for (j = 0; j < p->nAction; j++)
			{
				if (p->aAction[j].lookahead < 0)
					continue;
				if (p->aAction[j].lookahead == j + p->mnLookahead - i)
					n++;
			}
			if (n == p->nLookahead)
			{
				break;	/* An exact match is found at offset i */
			}
		}
	}

	/*
	 * If no existing offsets exactly match the current transaction, find an
	 * an empty offset in the aAction[] table in which we can add the
	 * aLookahead[] transaction.
	 */
	if (i < end)
	{
		/*
		 *  Look for holes in the aAction[] table that fit the current
		 * aLookahead[] transaction.  Leave i set to the offset of the hole.
		 * If no holes are found, i is left at p->nAction, which means the
		 * transaction will be appended.
		 */
		i = makeItSafe ? p->mnLookahead : 0;
		for (; i < p->nActionAlloc - p->mxLookahead; i++)
		{
			if (p->aAction[i].lookahead < 0)
			{
				for (j = 0; j < p->nLookahead; j++)
				{
					k = p->aLookahead[j].lookahead - p->mnLookahead + i;
					if (k < 0)
						break;
					if (p->aAction[k].lookahead >= 0)
						break;
				}
				if (j < p->nLookahead)
					continue;
				for (j = 0; j < p->nAction; j++)
				{
					if (p->aAction[j].lookahead == j + p->mnLookahead - i)
						break;
				}
				if (j == p->nAction)
				{
					break;	/* Fits in empty slots */
				}
			}
		}
	}
	/*
	 *Insert transaction set at index i.
	 */
#if 0
	printf("Acttab:");
	for (j = 0; j < p->nLookahead; j++)
	{
		printf(" %d", p->aLookahead[j].lookahead);
	}
	printf(" inserted at %d\n", i);
#endif
	for (j = 0; j < p->nLookahead; j++)
	{
		k = p->aLookahead[j].lookahead - p->mnLookahead + i;
		p->aAction[k] = p->aLookahead[j];
		if (k >= p->nAction)
			p->nAction = k + 1;
	}
	if (makeItSafe && i + p->nterminal >= p->nAction)
		p->nAction = i + p->nterminal + 1;
	p->nLookahead = 0;

	/*
	 * Return the offset that is added to the lookahead in order to get the
	 * index into yy_action of the action
	 */
	return i - p->mnLookahead;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		acttab_action_size
 \details	Return the size of the action table without the trailing syntax 
            error entries.
 \date		Created  on Sat Sep  1 17:45:00 2018
 \date		Modified on Sat Sep  1 17:45:00 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
int acttab_action_size(acttab *p)
{
	int n = p->nAction;
	while (n > 0 && p->aAction[n - 1].lookahead < 0)
	{
		n--;
	}
	return n;
}
