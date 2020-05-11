%{
	//#define LEX_DEBUG_ON
	#ifndef MAX_STR_LENGTH
		#define MAX_STR_LENGTH 4086 
	#endif
	#include "drhOutParser.tab.h"
	#include <stdlib.h>
	#include <string.h>
	#include <float.h>

	void yyerror(char *);
	int drhOut_lineNo=1;
	int drhOut_charPos,drhOut_oldPos;
	char drhOut_line[MAX_STR_LENGTH];
	char drhOut_oldLine[MAX_STR_LENGTH];
	double drhOut_maxNumber=DBL_MIN;
%}
%%

[\n\r]			{
				#ifdef LEX_DEBUG_ON
					//printf("lex: NEWLINE\n");
				#endif
				if(*yytext=='\n') ++drhOut_lineNo;
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 1\n");
				#endif
				drhOut_oldPos=drhOut_charPos;
				drhOut_charPos = 0;
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 2\n");
					//printf("drhOut_oldLine = [%s]\n",drhOut_oldLine);
					//printf("drhOut_line = [%s]\n",drhOut_line);
				#endif
				strcpy(drhOut_oldLine,drhOut_line);
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 3\n");
				#endif
					
				bzero(drhOut_line,sizeof(drhOut_line));
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 4\n");
					//printf("lex: Newline No Return\n");
				#endif
				return NEWLINE;
			}
			
[ \t] 		{	
				}

"unsat"	{
				#ifdef LEX_DEBUG_ON
					printf("lex: UNSAT\n");
				#endif
				
				drhOut_charPos+=yyleng;
				strcat(drhOut_line,yytext);
				strcpy(yylval.string,yytext);
				return UNSAT;
			}
			

"SAT"	{
				#ifdef LEX_DEBUG_ON
					printf("lex: SAT\n");
				#endif
				
				drhOut_charPos+=yyleng;
				strcat(drhOut_line,yytext);
				strcpy(yylval.string,yytext);
				return SAT;
			}
			
"For k = " {
				#ifdef LEX_DEBUG_ON
					printf("lex: KVAL\n");
				#endif
				
				drhOut_charPos+=yyleng;
				strcat(drhOut_line,yytext);
				strcpy(yylval.string,yytext);
				return KVAL;
			}
			
"dReal" {
				#ifdef LEX_DEBUG_ON
					printf("lex: DREAL\n");
				#endif
				
				drhOut_charPos+=yyleng;
				strcat(drhOut_line,yytext);
				strcpy(yylval.string,yytext);
				return DREAL;
			}
			
"SMT" {
				#ifdef LEX_DEBUG_ON
					printf("lex: SMT\n");
				#endif
				
				drhOut_charPos+=yyleng;
				strcat(drhOut_line,yytext);
				strcpy(yylval.string,yytext);
				return SMT;
			}
			
			
"/" {
				#ifdef LEX_DEBUG_ON
					printf("lex: FSLASH\n");
				#endif
				
				drhOut_charPos+=yyleng;
				strcat(drhOut_line,yytext);
				strcpy(yylval.string,yytext);
				return FSLASH;
			}			
			
":" {
				#ifdef LEX_DEBUG_ON
					printf("lex: COLON \n");
				#endif
				
				drhOut_charPos+=yyleng;
				strcat(drhOut_line,yytext);
				strcpy(yylval.string,yytext);
				return COLON;
			}			

"delta-sat" {
				#ifdef LEX_DEBUG_ON
					printf("lex: DELTA \n");
				#endif
				
				drhOut_charPos+=yyleng;
				strcat(drhOut_line,yytext);
				strcpy(yylval.string,yytext);
				return DELTA;
			}			

([0-9]+(\.[0-9]+)?)	{
				#ifdef LEX_DEBUG_ON
				printf("lex: NUMBER\n");
				#endif
				drhOut_charPos+=yyleng;
				strcat(drhOut_line,yytext);
				strcpy(yylval.string,yytext);
				return NUMBER;
			}
			
"."			{

				#ifdef LEX_DEBUG_ON
				printf("lex: DOT\n");
				#endif
				drhOut_charPos+=yyleng;
				strcat(drhOut_line,yytext);
				strcpy(yylval.string,yytext);
				return DOT;
			}
			
([a-zA-Z0-9_]+\.smt2) {
				#ifdef LEX_DEBUG_ON
				printf("lex: FILENAME [%s]\n",yytext);
				#endif
				drhOut_charPos+=yyleng;
				strcat(drhOut_line,yytext);
				strcpy(yylval.string,yytext);
				return FILENAME;
			}

([a-zA-Z]|[\[\]\-=,])*	{
				#ifdef LEX_DEBUG_ON
				printf("lex: TEXT [%s]\n",yytext);
				#endif
				drhOut_charPos+=yyleng;
				strcat(drhOut_line,yytext);
				strcpy(yylval.string,yytext);
				return TEXT;
			}
			
.			{	
				printf("Lexical Error at Line : [%d], Character Position : [%d], Token = [%s], \n",drhOut_lineNo,drhOut_charPos+1,yytext);				
				//exit(0);
			}
%%


int yywrap(void) {
    return 1;
}
