
/*
 *
 * CS-413 Spring 98
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <string.h>
#include "y.tab.h"

%}

%%

\n 	{
		return NEWLINE;
	}
"|" 	{
		return OR;
	}	
[ \t] 	{
		/* Discard spaces and tabs */
     	}	
"<" 	{
		return SMALLER;
	}
">" 	{
		return GREAT;
	}
">>" 	{
		return BIGBIG;
	}	
"&" 	{
		return FYON;
	}
">&"		{
		return GREATFYON;
		}
">>&"		{
		return BIGBIGFYON;
		}
"&>"		{
		return FYONGREAT;
		}
"&>>"		{
		return FYONBIGBIG;
		}		
[^ \t\n\>\|\&][^ \t\n]+ {
		/* Assume that file names have only alpha chars */
		yylval.string_val = strdup(yytext);
		return WORD;
	}
.	{
		/* Invalid character in input */
		return NOTOKEN;
	}

%%
	
