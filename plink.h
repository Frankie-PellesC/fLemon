#ifndef PLINK_H__
#define PLINK_H__
#pragma once

struct plink *Plink_new(void);
void          Plink_add(struct plink **, struct config *);
void          Plink_copy(struct plink **, struct plink *);
void          Plink_delete(struct plink *);

#endif	//PLINK_H__
