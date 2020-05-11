%{
	//#define LEX_DEBUG_ON
	#ifndef MAX_STR_LENGTH_DRH
		#define MAX_STR_LENGTH_DRH 4086 
	#endif
	#include "drhParser.tab.h"
	#include <stdlib.h>
	#include <string.h>
	#include <float.h>

	void yyerror(char *);
	int drh_lineNo=1;
	int drh_charPos,drh_oldPos;
	char drh_line[MAX_STR_LENGTH_DRH];
	char drh_oldLine[MAX_STR_LENGTH_DRH];
	double drh_maxNumber=DBL_MAX;
%}
%%

[\n\r]			{
				#ifdef LEX_DEBUG_ON
					//printf("lex: NEWLINE\n");
				#endif
				if(*yytext=='\n') ++drh_lineNo;
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 1\n");
				#endif
				drh_oldPos=drh_charPos;
				drh_charPos = 0;
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 2\n");
					//printf("drh_oldLine = [%s]\n",drh_oldLine);
					//printf("drh_line = [%s]\n",drh_line);
				#endif
				strcpy(drh_oldLine,drh_line);
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 3\n");
				#endif
					
				bzero(drh_line,sizeof(drh_line));
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 4\n");
					//printf("lex: Newline No Return\n");
				#endif
				return NEWLINE;
			}

"["		{
				#ifdef LEX_DEBUG_ON
					printf("lex: LBRACK\n");
				#endif
				printf("LBRACK\n");
				drh_charPos+=yyleng;
				strcat(drh_line,yytext);
				strcpy(yylval.string,yytext);
				return LBRACK;
			}

"]"		{
				#ifdef LEX_DEBUG_ON
					printf("lex: RBRACK\n");
				#endif
				printf("RBRACK\n");
				drh_charPos+=yyleng;
				strcat(drh_line,yytext);
				strcpy(yylval.string,yytext);
				return RBRACK;
			}

\-?((([0-9]*)\.([0-9]+))|([0-9]+))((([eE])[\+\-]?[0-9]+)?) {	
				#ifdef LEX_DEBUG_ON
				printf("lex: DRATIONAL\n");
				#endif
				drh_charPos+=yyleng;
				strcat(drh_line,yytext);
				strcpy(yylval.string,yytext);
				return DRATIONAL;
			}

","		{
				#ifdef LEX_DEBUG_ON
					printf("lex: COMMA\n");
				#endif
				printf("GOAL\n");
				drh_charPos+=yyleng;
				strcat(drh_line,yytext);
				strcpy(yylval.string,yytext);
				return COMMA;
			}


"goal"	{
				#ifdef LEX_DEBUG_ON
					printf("lex: GOAL\n");
				#endif
				printf("GOAL\n");
				drh_charPos+=yyleng;
				strcat(drh_line,yytext);
				strcpy(yylval.string,yytext);
				return GOAL;
			}

":"			{
				#ifdef LEX_DEBUG_ON
				printf("lex: COLON\n");
				#endif
				drh_charPos+=yyleng;
				strcat(drh_line,yytext);
				strcpy(yylval.string,yytext);
				return COLON;
			}

([a-zA-Z0-9]|[eE@:\(\)\[\]\{\}\-+=<>;/,&|*'\._ \t])*	{
				#ifdef LEX_DEBUG_ON
				printf("lex: TEXT [%s]\n",yytext);
				#endif
				drh_charPos+=yyleng;
				strcat(drh_line,yytext);
				strcpy(yylval.string,yytext);
				return TEXT;
			}
			
.			{	
				printf("Lexical Error at Line : [%d], Character Position : [%d], Token = [%s], \n",drh_lineNo,drh_charPos+1,yytext);				
				//exit(0);
			}
%%


int yywrap(void) {
    return 1;
}
