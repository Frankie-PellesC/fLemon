#ifndef BUILD_H__
#define BUILD_H__
#pragma once

void FindRulePrecedences(struct lemon*);
void FindFirstSets(struct lemon*);
void FindStates(struct lemon*);
void FindLinks(struct lemon*);
void FindFollowSets(struct lemon*);
void FindActions(struct lemon*);
int same_symbol(struct symbol *a, struct symbol *b);

#endif	//BUILD_H__
