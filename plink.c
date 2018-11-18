/*+@@file@@----------------------------------------------------------------*//*!
 \file		plink.c
 \par Description 
            Routines processing configuration follow-set propagation links 
            in the LEMON parser generator.
 \par  Status: 
            
 \par Project: 
            Lemon parser
 \date		Created  on Sat Sep  1 20:00:13 2018
 \date		Modified on Sat Sep  1 20:00:13 2018
 \author	
\*//*-@@file@@----------------------------------------------------------------*/
#include "main.h"
#include "struct.h"
#include "plink.h"

static struct plink *plink_freelist = 0;

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Plink_new
 \details	Allocate a new plink
 \date		Created  on Sat Sep  1 20:01:45 2018
 \date		Modified on Sat Sep  1 20:01:45 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
struct plink *Plink_new(void)
{
	struct plink *newlink;

	if (plink_freelist == 0)
	{
		int i;
		int amt = 100;
		plink_freelist = (struct plink *)calloc(amt, sizeof(struct plink));
		if (plink_freelist == 0)
		{
			fprintf(stderr, "Unable to allocate memory for a new follow-set propagation link.\n");
			exit(1);
		}
		for (i = 0; i < amt - 1; i++)
			plink_freelist[i].next = &plink_freelist[i + 1];
		plink_freelist[amt - 1].next = 0;
	}

	newlink        = plink_freelist;
	plink_freelist = plink_freelist->next;

	return newlink;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Plink_add
 \details	Add a plink to a plink list
 \date		Created  on Sat Sep  1 20:01:59 2018
 \date		Modified on Sat Sep  1 20:01:59 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void Plink_add(struct plink **plpp, struct config *cfp)
{
	struct plink *newlink;

	newlink       = Plink_new();
	newlink->next = *plpp;
	*plpp         = newlink;
	newlink->cfp  = cfp;
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Plink_copy
 \details	Transfer every plink on the list "from" to the list "to"
 \date		Created  on Sat Sep  1 20:02:30 2018
 \date		Modified on Sat Sep  1 20:02:30 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void Plink_copy(struct plink **to, struct plink *from)
{
	struct plink *nextpl;

	while (from)
	{
		nextpl     = from->next;
		from->next = *to;
		*to        = from;
		from       = nextpl;
	}
}

/*+@@fnc@@----------------------------------------------------------------*//*!
 \brief		Plink_delete
 \details	Delete every plink on the list
 \date		Created  on Sat Sep  1 20:03:04 2018
 \date		Modified on Sat Sep  1 20:03:04 2018
\*//*-@@fnc@@----------------------------------------------------------------*/
void Plink_delete(struct plink *plp)
{
	struct plink *nextpl;

	while (plp)
	{
		nextpl         = plp->next;
		plp->next      = plink_freelist;
		plink_freelist = plp;
		plp            = nextpl;
	}
}
