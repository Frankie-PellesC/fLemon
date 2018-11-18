#ifndef ACTION_H__
#define ACTION_H__
#pragma once

struct action *Action_sort(struct action *ap);
void           Action_add(struct action **app, enum e_action type, struct symbol *sp, char *arg);

/*!
 * The state of the yy_action table under construction is an instance of
 * the lookahead_action structure.
 *
 * The yy_action table maps the pair (state_number, lookahead) into an
 * action_number.\n
 * The table is an array of integers pairs.\n
 * The state_number determines an initial offset into the yy_action array.\n
 * The lookahead value is then added to this initial offset to get an
 * index X into the yy_action array.\n
 * If the aAction[X].lookahead equals the value of the lookahead input,
 * then the value of the action_number output is aAction[X].action.\n
 * If the lookaheads do not match then the default action for the
 * state_number is returned.
 *
 * All actions associated with a single state_number are first entered
 * into aLookahead[] using multiple calls to acttab_action().\n
 * Then the actions for that single state_number are placed into the
 * aAction[] array with a single call to acttab_insert().\n
 * The acttab_insert() call also resets the aLookahead[] array in
 * preparation for the next state number.
 */
struct lookahead_action
{
	int lookahead;	/*!< Value of the lookahead token */
	int action;		/*!< Action to take on the given lookahead */
};

typedef struct acttab acttab;

struct acttab
{
	int    nAction;			/*!< Number of used slots in aAction[]			*/
	int    nActionAlloc;	/*!< Slots allocated for aAction[]				*/
	struct lookahead_action
	      *aAction,			/*!< The yy_action[] table under construction	*/
	      *aLookahead;		/*!< A single new transaction set				*/
	int    mnLookahead;		/*!< Minimum aLookahead[].lookahead				*/
	int    mnAction;		/*!< Action associated with mnLookahead			*/
	int    mxLookahead;		/*!< Maximum aLookahead[].lookahead				*/
	int    nLookahead;		/*!< Used slots in aLookahead[]					*/
	int    nLookaheadAlloc;	/*!< Slots allocated in aLookahead[]			*/
	int    nterminal;		/*!< Number of terminal symbols					*/
	int    nsymbol;			/*!< total number of symbols					*/
};

/*!
 * Return the number of entries in the yy_action table
 */
#define acttab_lookahead_size(X) ((X)->nAction)

/*!
 * The value for the N-th entry in yy_action
 */
#define acttab_yyaction(X,N)  ((X)->aAction[N].action)

/*!
 * The value for the N-th entry in yy_lookahead
 */
#define acttab_yylookahead(X,N)  ((X)->aAction[N].lookahead)

void    acttab_free(acttab *p);
acttab *acttab_alloc(int nsymbol, int nterminal);
void    acttab_action(acttab *p, int lookahead, int action);
int     acttab_insert(acttab *p, int makeItSafe);
int     acttab_action_size(acttab *p);

#endif	// ACTION_H__
