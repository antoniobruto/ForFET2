%{
	//#define LEX_DEBUG_ON
	#ifndef MAX_STR_LENGTH
		#define MAX_STR_LENGTH 1024 
	#endif
	#include "haslacParser.tab.h"
	#include <stdlib.h>
	#include <string.h>
	#include <float.h>

	void yyerror(char *);
	int haslac_lineNo=1;
	int haslac_charPos,haslac_oldPos;
	char haslac_line[MAX_STR_LENGTH];
	char haslac_oldLine[MAX_STR_LENGTH];
	double haslac_maxNumber=DBL_MIN;
%}
%%

[\n\r]			{
				#ifdef LEX_DEBUG_ON
					//printf("lex: NEWLINE\n");
				#endif
				if(*yytext=='\n') ++haslac_lineNo;
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 1\n");
				#endif
				haslac_oldPos=haslac_charPos;
				haslac_charPos = 0;
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 2\n");
					//printf("haslac_oldLine = [%s]\n",haslac_oldLine);
					//printf("haslac_line = [%s]\n",haslac_line);
				#endif
				strcpy(haslac_oldLine,haslac_line);
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 3\n");
				#endif
					
				bzero(haslac_line,sizeof(haslac_line));
				
				#ifdef LEX_DEBUG_ON
					//printf("lex: Newline 4\n");
					//printf("lex: Newline No Return\n");
				#endif
				//return NEWLINE;
			}

"module"	{
				#ifdef LEX_DEBUG_ON
					printf("lex: MODULE\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return MODULE;
			}

"endmodule"	{
				#ifdef LEX_DEBUG_ON
					printf("lex: ENDMODULE\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return ENDMODULE;
			}

"input"	{
				#ifdef LEX_DEBUG_ON
					printf("lex: INPUT\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return INPUT;
			}

"output"	{
				#ifdef LEX_DEBUG_ON
					printf("lex: OUTPUT\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return OUTPUT;
			}			


"parameter"	{
				#ifdef LEX_DEBUG_ON
					printf("lex: PARAM\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return PARAM;
			}

"begin"	{
				#ifdef LEX_DEBUG_ON
					printf("lex: BEGIN\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return BEGINN;
			}
				
"end"			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: END\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return END;
			}
			

"mode"			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: MODE\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return MODE;
			}

"mode'"		{	
				#ifdef LEX_DEBUG_ON
					printf("lex: MODE_NEXT\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return MODE_NEXT;
			}

"ddt"			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: DDT\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return DDT;
			}
			

"property"	{
				#ifdef LEX_DEBUG_ON
					printf("lex: PROPERTY\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return PROPERTY;
			}
			

"endproperty" {	
				#ifdef LEX_DEBUG_ON
					printf("lex: ENDPROPERTY\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return ENDPROPERTY;
			}
			
"trans"			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: TRANS\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return TRANS;
			}
			

"inv"			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: INV\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return INV;
			}
			
			
"initial"	{	
				#ifdef LEX_DEBUG_ON
					printf("lex: INITIAL\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return INITIALL;
			}
			
"set"		{	
				#ifdef LEX_DEBUG_ON
					printf("lex: SET\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return SETT;
			}
			
"output-var" {	
				#ifdef LEX_DEBUG_ON
					printf("lex: OUTPUT_VAR\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return OUTPUT_VAR;
			}

"true"|"True"|"TRUE"	{	
				#ifdef LEX_DEBUG_ON
				printf("lex: true\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return TRUE;
			}	

"=="			{	
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				#ifdef LEX_DEBUG_ON
					printf("lex: EQ\n");
				#endif
				strcpy(yylval.string,yytext);
				return EQ;
			}
			
"<="			{
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				#ifdef LEX_DEBUG_ON
					printf("lex: LEQ\n");
				#endif
				strcpy(yylval.string,yytext);
				return LEQ;
			}
			
">="			{
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				#ifdef LEX_DEBUG_ON
					printf("lex: GEQ\n");
				#endif
				strcpy(yylval.string,yytext);
				return GEQ;
			}
			
"<"			{
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				#ifdef LEX_DEBUG_ON
					printf("lex: LT\n");
				#endif
				strcpy(yylval.string,yytext);
				return LT;
			}
			
			
">"			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: GT\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return GT;
			}

"="			{	
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				#ifdef LEX_DEBUG_ON
					printf("lex: ASSIGN\n");
				#endif
				strcpy(yylval.string,yytext);
				return ASSIGN;
			}

			
			
";"			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: SEMICOLON\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return SEMICOLON;
			}

"{"			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: OPENCURLY\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return OPENCURLY;
			}

"}"			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: CLOSECURLY\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return CLOSECURLY;
			}

"["			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: OPENSQUARE\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return OPENSQUARE;
			}

"]"			{	
				#ifdef LEX_DEBUG_ON
					printf("lex: CLOSESQUARE\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return CLOSESQUARE;
			}

"&&"			{
				#ifdef LEX_DEBUG_ON
					printf("lex: AND\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return AND;
			}

"$"			{
				#ifdef LEX_DEBUG_ON
					printf("lex: ANY\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return ANY;
			}
			

"|=>"		{
				#ifdef LEX_DEBUG_ON
					printf("lex: IMPLIES\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return IMPLIES;
			}
			
"//".*		{	
				#ifdef LEX_DEBUG_ON
				printf("lex: COMMENT\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
			}			

[a-zA-Z][0-9a-zA-Z_]*	{	
				#ifdef LEX_DEBUG_ON
				printf("lex: VARIABLE [%s]\n",yytext);
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return IDENTIFIER;
			}

\*|\+|\-|\/             {       
                                #ifdef LEX_DEBUG_ON
                                printf("lex: OP [%s]\n",yytext);
                                #endif
                                haslac_charPos+=yyleng;
                                strcat(haslac_line,yytext);
                                strcpy(yylval.string,yytext);
                                return OP;
                        }	
			
			
[0-9]+(\.[0-9]+)?[eE]\-?[0-9]+	{	
				#ifdef LEX_DEBUG_ON
				printf("lex: NUMBER with Exponent\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				double d = atof(yytext);
				if(d>haslac_maxNumber) haslac_maxNumber = d;
				return NUMBER;
			}	

[0-9]+\.[0-9]+	{	
				#ifdef LEX_DEBUG_ON
				printf("lex: NUMBER with DECIMAL [%s]\n",yytext);
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				double d = atof(yytext);
				if(d>haslac_maxNumber) haslac_maxNumber = d;
				//printf("lex: Returning NUMBER with DECIMAL\n");
				return NUMBER;

			}
			
[0-9]+		{	
				#ifdef LEX_DEBUG_ON
				printf("lex: NUMBER NO DECIMAL\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				double d = atof(yytext);
				if(d>haslac_maxNumber) haslac_maxNumber = d;
				return NUMBER;
			}
			
			
"'"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: NEXT\n");
				#endif
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return NEXT;
			}

[ \t]+			{
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				#ifdef LEX_DEBUG_ON
				printf("lex: Whitespace found\n");
				#endif
			}
"("			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: (\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return OPENROUND;
			}

")"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: )\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return CLOSEROUND;
			}

","			{
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				#ifdef LEX_DEBUG_ON
				printf("lex: COMMA\n");
				#endif
				strcpy(yylval.string,yytext);
				return COMMA;
			}

":"			{
				#ifdef LEX_DEBUG_ON
				printf("lex: COLON\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return COLON;
			}

\.			{
				#ifdef LEX_DEBUG_ON
				printf("lex: DOT\n");
				#endif
				haslac_charPos+=yyleng;
				strcat(haslac_line,yytext);
				strcpy(yylval.string,yytext);
				return DOT;
			}
			
.			{	
				printf("Lexical Error at Line : [%d], Character Position : [%d], Token = [%s], \n",haslac_lineNo,haslac_charPos+1,yytext);				
				//exit(0);
			}
%%


int yywrap(void) {
    return 1;
}
