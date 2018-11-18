/*+@@file@@----------------------------------------------------------------*//*!
 \file		configlist.c
 \par Description 
            Routines to processing a configuration list and building a 
            state in the LEMON parser generator.
 \par  Status: 
            
 \par Project: 
            Lemon parser
 \date		Created  on Sat Sep  1 18:35:49 2018
 \date		Modified on Sat Sep  1 18:35:49 2018
 \author	
\*//*-@@file@@----------------------------------------------------------------*/
#include "main.h"
#include "struct.h"
#include "table.h"
#include "set.h"
#include "plink.h"

static struct config  *freelist   = 0;	/* List of free configurations */
static struct config  *current    = 0;	/* Top of list of configurations */
static struct config **currentend = 0;	/* Last on list of configs */
static struct config  *basis      = 0;	/* Top of list of basis configs */
static struct config **basisend   = 0;	/* End of list of basis configs */

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		newconfig
 \details	Return a pointer to a new configuration
 \date		Created  on Sat Sep  1 18:36:25 2018
 \date		Modified on Sat Sep  1 18:36:25 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE struct config *newconfig(void)
{
	struct config *newcfg;
	if (freelist == 0)
	{
		int i;
		int amt = 3;
		freelist = (struct config *)calloc(amt, sizeof(struct config));
		if (freelist == 0)
		{
			fprintf(stderr, "Unable to allocate memory for a new configuration.");
			exit(1);
		}
		for (i = 0; i < amt - 1; i++)
			freelist[i].next = &freelist[i + 1];
		freelist[amt - 1].next = 0;
	}
	newcfg = freelist;
	freelist = freelist->next;
	return newcfg;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		deleteconfig
 \details	The configuration "old" is no longer used
 \date		Created  on Sat Sep  1 18:36:41 2018
 \date		Modified on Sat Sep  1 18:36:41 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
PRIVATE void deleteconfig(struct config *old)
{
	old->next = freelist;
	freelist = old;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Configlist_init
 \details	Initialized the configuration list builder
 \date		Created  on Sat Sep  1 18:36:55 2018
 \date		Modified on Sat Sep  1 18:36:55 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void Configlist_init(void)
{
	current    = 0;
	currentend = &current;
	basis      = 0;
	basisend   = &basis;
	Configtable_init();
	return;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Configlist_reset
 \details	Initialized the configuration list builder
 \date		Created  on Sat Sep  1 18:37:20 2018
 \date		Modified on Sat Sep  1 18:37:20 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void Configlist_reset(void)
{
	current    = 0;
	currentend = &current;
	basis      = 0;
	basisend   = &basis;
	Configtable_clear(0);
	return;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Configlist_add
 \details	Add another configuration to the configuration list

 \param		[in] rp The rule

 \param		[in] dot Index into the RHS of the rule where the dot goes
 \date		Created  on Sat Sep  1 18:39:40 2018
 \date		Modified on Sat Sep  1 18:39:40 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
struct config *Configlist_add(struct rule *rp, int dot)
{
	struct config *cfp, model;

	assert(currentend != 0);
	model.rp  = rp;
	model.dot = dot;
	cfp       = Configtable_find(&model);
	if (cfp == 0)
	{
		cfp         = newconfig();
		cfp->rp     = rp;
		cfp->dot    = dot;
		cfp->fws    = SetNew();
		cfp->stp    = 0;
		cfp->fplp   = cfp->bplp = 0;
		cfp->next   = 0;
		cfp->bp     = 0;
		*currentend = cfp;
		currentend  = &cfp->next;
		Configtable_insert(cfp);
	}
	return cfp;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Configlist_addbasis
 \details	Add a basis configuration to the configuration list
 \date		Created  on Sat Sep  1 18:40:03 2018
 \date		Modified on Sat Sep  1 18:40:03 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
struct config *Configlist_addbasis(struct rule *rp, int dot)
{
	struct config *cfp, model;

	assert(basisend   != 0);
	assert(currentend != 0);
	model.rp  = rp;
	model.dot = dot;
	cfp       = Configtable_find(&model);
	if (cfp == 0)
	{
		cfp         = newconfig();
		cfp->rp     = rp;
		cfp->dot    = dot;
		cfp->fws    = SetNew();
		cfp->stp    = 0;
		cfp->fplp   = cfp->bplp = 0;
		cfp->next   = 0;
		cfp->bp     = 0;
		*currentend = cfp;
		currentend  = &cfp->next;
		*basisend   = cfp;
		basisend    = &cfp->bp;
		Configtable_insert(cfp);
	}
	return cfp;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Configlist_closure
 \details	Compute the closure of the configuration list
 \date		Created  on Sat Sep  1 18:41:00 2018
 \date		Modified on Sat Sep  1 18:41:00 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void Configlist_closure(struct lemon *lemp)
{
	struct config *cfp, *newcfp;
	struct rule   *rp,  *newrp;
	struct symbol *sp,  *xsp;
	int            i,    dot;

	assert(currentend != 0);
	for (cfp = current; cfp; cfp = cfp->next)
	{
		rp  = cfp->rp;
		dot = cfp->dot;
		if (dot >= rp->nrhs)
			continue;
		sp = rp->rhs[dot];
		if (sp->type == NONTERMINAL)
		{
			if (sp->rule == 0 && sp != lemp->errsym)
			{
				ErrorMsg(lemp->filename, rp->line, "Nonterminal \"%s\" has no rules.", sp->name);
				lemp->errorcnt++;
			}
			for (newrp = sp->rule; newrp; newrp = newrp->nextlhs)
			{
				newcfp = Configlist_add(newrp, 0);
				for (i = dot + 1; i < rp->nrhs; i++)
				{
					xsp = rp->rhs[i];
					if (xsp->type == TERMINAL)
					{
						SetAdd(newcfp->fws, xsp->index);
						break;
					}
					else if (xsp->type == MULTITERMINAL)
					{
						int k;
						for (k = 0; k < xsp->nsubsym; k++)
						{
							SetAdd(newcfp->fws, xsp->subsym[k]->index);
						}
						break;
					}
					else
					{
						SetUnion(newcfp->fws, xsp->firstset);
						if (xsp->lambda == LEMON_FALSE)
							break;
					}
				}
				if (i == rp->nrhs)
					Plink_add(&cfp->fplp, newcfp);
			}
		}
	}
	return;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Configlist_sort
 \details	Sort the configuration list
 \date		Created  on Sat Sep  1 18:41:45 2018
 \date		Modified on Sat Sep  1 18:41:45 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void Configlist_sort(void)
{
	current = (struct config *)msort((char *)current, (char **)&(current->next), Configcmp);
	currentend = 0;
	return;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Configlist_sortbasis
 \details	Sort the basis configuration list
 \date		Created  on Sat Sep  1 18:42:04 2018
 \date		Modified on Sat Sep  1 18:42:04 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void Configlist_sortbasis(void)
{
	basis = (struct config *)msort((char *)current, (char **)&(current->bp), Configcmp);
	basisend = 0;
	return;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Configlist_return
 \details	Return a pointer to the head of the configuration list and 
            reset the list
 \date		Created  on Sat Sep  1 18:42:23 2018
 \date		Modified on Sat Sep  1 18:42:23 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
struct config *Configlist_return(void)
{
	struct config *old;
	old        = current;
	current    = 0;
	currentend = 0;
	return old;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Configlist_basis
 \details	Return a pointer to the head of the configuration list and 
            reset the list
 \date		Created  on Sat Sep  1 18:42:48 2018
 \date		Modified on Sat Sep  1 18:42:48 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
struct config *Configlist_basis(void)
{
	struct config *old;
	old      = basis;
	basis    = 0;
	basisend = 0;
	return old;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Configlist_eat
 \details	Free all elements of the given configuration list
 \date		Created  on Sat Sep  1 18:43:13 2018
 \date		Modified on Sat Sep  1 18:43:13 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void Configlist_eat(struct config *cfp)
{
	struct config *nextcfp;
	for (; cfp; cfp = nextcfp)
	{
		nextcfp = cfp->next;
		assert(cfp->fplp == 0);
		assert(cfp->bplp == 0);
		if (cfp->fws)
			SetFree(cfp->fws);
		deleteconfig(cfp);
	}
	return;
}
