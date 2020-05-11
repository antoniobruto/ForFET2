%{
	//#define LEX_DEBUG_ON
	#ifndef MAX_STR_LENGTH
		#define MAX_STR_LENGTH 1024 
	#endif
	#include "spaceExOutputParser.tab.h"
	#include <stdlib.h>
	#include <string.h>
	#include <float.h>

	void yyerror(char *);
	int spaceEx_lineNo=1;
	int spaceEx_charPos,spaceEx_oldPos;
	char spaceEx_line[MAX_STR_LENGTH];
	char spaceEx_oldLine[MAX_STR_LENGTH];
	double spaceEx_maxNumber=DBL_MIN;
%}
%%

[\n\r]			{
				#ifdef LEX_DEBUG_ON
					//printf("lex: NEWLINE\n");
				#endif
				if(*yytext=='\n') ++spaceEx_lineNo;
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 1\n");
				#endif
				spaceEx_oldPos=spaceEx_charPos;
				spaceEx_charPos = 0;
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 2\n");
					//printf("spaceEx_oldLine = [%s]\n",spaceEx_oldLine);
					//printf("spaceEx_line = [%s]\n",spaceEx_line);
				#endif
				strcpy(spaceEx_oldLine,spaceEx_line);
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 3\n");
				#endif
					
				bzero(spaceEx_line,sizeof(spaceEx_line));
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 4\n");
					//printf("lex: Newline No Return\n");
				#endif
				//return NEWLINE;
			}

"FINAL"	{
				#ifdef LEX_DEBUG_ON
					printf("lex: MODULE\n");
				#endif
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				strcpy(yylval.string,yytext);
				return FINAL;
			}

":"			{
				#ifdef LEX_DEBUG_ON
				printf("lex: COLON\n");
				#endif
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				strcpy(yylval.string,yytext);
				return COLON;
			}


"=="			{	
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				#ifdef LEX_DEBUG_ON
					printf("lex: EQ\n");
				#endif
				strcpy(yylval.string,yytext);
				return EQ;
			}
			
"["			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: OPENSQUARE\n");
				#endif
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				strcpy(yylval.string,yytext);
				return OPENSQUARE;
			}

"]"			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: CLOSESQUARE\n");
				#endif
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				strcpy(yylval.string,yytext);
				return CLOSESQUARE;
			}


			
\-?[0-9]+(\.[0-9]+)?[eE]\-?[0-9]+	{	
				#ifdef LEX_DEBUG_ON
				printf("lex: NUMBER with Exponent : %s\n",yytext);
				#endif
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				strcpy(yylval.string,yytext);
				double d = atof(yytext);
				if(d>spaceEx_maxNumber) spaceEx_maxNumber = d;
				return NUMBER;
			}	

\-?[0-9]+\.[0-9]+	{	
				#ifdef LEX_DEBUG_ON
				printf("lex: NUMBER with DECIMAL\n");
				#endif
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				strcpy(yylval.string,yytext);
				double d = atof(yytext);
				if(d>spaceEx_maxNumber) spaceEx_maxNumber = d;
				//printf("lex: Returning NUMBER with DECIMAL\n");
				return NUMBER;

			}
			
\-?[0-9]+	{	
				#ifdef LEX_DEBUG_ON
				printf("lex: NUMBER NO DECIMAL\n");
				#endif
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				strcpy(yylval.string,yytext);
				double d = atof(yytext);
				if(d>spaceEx_maxNumber) spaceEx_maxNumber = d;
				return NUMBER;
			}

[a-zA-Z][0-9a-zA-Z_\-]*	{
				#ifdef LEX_DEBUG_ON
				printf("lex: VARIABLE [%s]\n",yytext);
				#endif
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				strcpy(yylval.string,yytext);
				return IDENTIFIER;
			}
			
			
[ \t]+			{
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				#ifdef LEX_DEBUG_ON
				printf("lex: Whitespace found\n");
				#endif
			}
"("			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: (\n");
				#endif
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				strcpy(yylval.string,yytext);
				return OPENROUND;
			}

")"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: )\n");
				#endif
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				strcpy(yylval.string,yytext);
				return CLOSEROUND;
			}

","			{
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				#ifdef LEX_DEBUG_ON
				printf("lex: COMMA\n");
				#endif
				strcpy(yylval.string,yytext);
				return COMMA;
			}

\.			{
				#ifdef LEX_DEBUG_ON
				printf("lex: DOT\n");
				#endif
				spaceEx_charPos+=yyleng;
				strcat(spaceEx_line,yytext);
				strcpy(yylval.string,yytext);
				return DOT;
			}
			
.			{	
				printf("Lexical Error at Line : [%d], Character Position : [%d], Token = [%s], \n",spaceEx_lineNo,spaceEx_charPos+1,yytext);				
				//exit(0);
			}
%%


int yywrap(void) {
    return 1;
}
