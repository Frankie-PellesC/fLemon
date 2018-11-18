#ifndef CONFIGLIST_H__
#define CONFIGLIST_H__
#pragma once

void           Configlist_init(void);
struct config *Configlist_add(struct rule *, int);
struct config *Configlist_addbasis(struct rule *, int);
void           Configlist_closure(struct lemon *);
void           Configlist_sort(void);
void           Configlist_sortbasis(void);
struct config *Configlist_return(void);
struct config *Configlist_basis(void);
void           Configlist_eat(struct config *);
void           Configlist_reset(void);

#endif	//CONFIGLIST_H__
