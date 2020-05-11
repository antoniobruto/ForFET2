%{
	//#define LEX_DEBUG_ON
	#ifndef MAX_STR_LENGTH
		#define MAX_STR_LENGTH 4086 
	#endif
	#include "JSONTraceParser.tab.h"
	#include <stdlib.h>
	#include <string.h>
	#include <float.h>

	void yyerror(char *);
	int JT_lineNo=1;
	int JT_charPos,JT_oldPos;
	char JT_line[MAX_STR_LENGTH];
	char JT_oldLine[MAX_STR_LENGTH];
	double JT_maxNumber=DBL_MIN;
%}
%%

[\n\r]			{
				#ifdef LEX_DEBUG_ON
					//printf("lex: NEWLINE\n");
				#endif
				if(*yytext=='\n') ++JT_lineNo;
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 1\n");
				#endif
				JT_oldPos=JT_charPos;
				JT_charPos = 0;
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 2\n");
					//printf("JT_oldLine = [%s]\n",JT_oldLine);
					//printf("JT_line = [%s]\n",JT_line);
				#endif
				strcpy(JT_oldLine,JT_line);
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 3\n");
				#endif
					
				bzero(JT_line,sizeof(JT_line));
				
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
				JT_charPos+=yyleng;
				strcat(JT_line,yytext);
				strcpy(yylval.string,yytext);
				return LBRACK;
			}

"]"		{
				#ifdef LEX_DEBUG_ON
					printf("lex: RBRACK\n");
				#endif
				printf("RBRACK\n");
				JT_charPos+=yyleng;
				strcat(JT_line,yytext);
				strcpy(yylval.string,yytext);
				return RBRACK;
			}

"\""		{
				#ifdef LEX_DEBUG_ON
				printf("lex: DRATIONAL\n");
				#endif
				JT_charPos+=yyleng;
				strcat(JT_line,yytext);
				strcpy(yylval.string,yytext);
				return QUOTE;
			}

"step"		{
				#ifdef LEX_DEBUG_ON
				printf("lex: DRATIONAL\n");
				#endif
				JT_charPos+=yyleng;
				strcat(JT_line,yytext);
				strcpy(yylval.string,yytext);
				return STEP;
			}

\-?((([0-9]*)\.([0-9]+))|([0-9]+))((([eE])[\+\-]?[0-9]+)?) {	
				#ifdef LEX_DEBUG_ON
				printf("lex: DRATIONAL\n");
				#endif
				JT_charPos+=yyleng;
				strcat(JT_line,yytext);
				strcpy(yylval.string,yytext);
				return JRATIONAL;
			}

","		{
				#ifdef LEX_DEBUG_ON
					printf("lex: COMMA\n");
				#endif
				printf("GOAL\n");
				JT_charPos+=yyleng;
				strcat(JT_line,yytext);
				strcpy(yylval.string,yytext);
				return COMMA;
			}


"goal"	{
				#ifdef LEX_DEBUG_ON
					printf("lex: GOAL\n");
				#endif
				printf("GOAL\n");
				JT_charPos+=yyleng;
				strcat(JT_line,yytext);
				strcpy(yylval.string,yytext);
				return GOAL;
			}

":"			{
				#ifdef LEX_DEBUG_ON
				printf("lex: COLON\n");
				#endif
				JT_charPos+=yyleng;
				strcat(JT_line,yytext);
				strcpy(yylval.string,yytext);
				return COLON;
			}

([a-zA-Z0-9]|[eE@:\(\)\[\]\{\}\-+=<>;/,&|*'\._ \t])*	{
				#ifdef LEX_DEBUG_ON
				printf("lex: TEXT [%s]\n",yytext);
				#endif
				JT_charPos+=yyleng;
				strcat(JT_line,yytext);
				strcpy(yylval.string,yytext);
				return TEXT;
			}
			
.			{	
				printf("Lexical Error at Line : [%d], Character Position : [%d], Token = [%s], \n",JT_lineNo,JT_charPos+1,yytext);				
				//exit(0);
			}
%%


int yywrap(void) {
    return 1;
}
