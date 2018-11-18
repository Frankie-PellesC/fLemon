%token_prefix DECLSPEC_
%name declSpec_

decl_specifier ::= DECLSPEC LPAREN  extended_decl_modifier_seq  RPAREN.

//extended_decl_modifier_seq ::= extended_decl_modifieropt.
extended_decl_modifier_seq ::= extended_decl_modifier extended_decl_modifier_seq.
constant_expression ::= NUMBER OP NUMBER.
extended_decl_modifier ::= ALIGN LPAREN constant_expression RPAREN.
extended_decl_modifier ::= ALLOCATE LPAREN STRING_LITERAL RPAREN.
extended_decl_modifier ::= APPDOMAIN.
extended_decl_modifier ::= CODE_SEG LPAREN STRING_LITERAL RPAREN.
extended_decl_modifier ::= DEPRECATED.
extended_decl_modifier ::= DLLIMPORT.
extended_decl_modifier ::= DLLEXPORT.
extended_decl_modifier ::= JITINTRINSIC.
extended_decl_modifier ::= NAKED.
extended_decl_modifier ::= NOALIAS.
extended_decl_modifier ::= NOINLINE.
extended_decl_modifier ::= NORETURN.
extended_decl_modifier ::= NOTHROW.
extended_decl_modifier ::= NOVTABLE.
extended_decl_modifier ::= PROCESS.
extended_decl_modifier ::= PROPERTY LPAREN LBRACE GET ASSIGN ID BAR COMMA PUT ASSIGN ID RBRACE RPAREN.
extended_decl_modifier ::= RESTRICT.
extended_decl_modifier ::= SAFEBUFFERS.
extended_decl_modifier ::= SELECTANY.
extended_decl_modifier ::= THREAD.
extended_decl_modifier ::= UUID LPAREN STRING_LITERAL RPAREN.
