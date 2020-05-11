%{
	//#define LEX_DEBUG_ON
	
	#ifndef MAX_STR_LENGTH
		#define MAX_STR_LENGTH 256
	#endif
	
	#include "featureParser.tab.h"
	#include <stdlib.h>
	#include <string.h>
	
	void yyerror(char *);
	int lineNo = 1;
	int charPos, oldPos;
	char line[MAX_STR_LENGTH];
	char oldLine[MAX_STR_LENGTH];
%}

%%

[\n\r]			{
				#ifdef LEX_DEBUG_ON
				printf("lex: NEWLINE\n");
				#endif
				if(*yytext=='\n') ++lineNo;
				oldPos=charPos;
				charPos = 0;
				strcpy(oldLine,line);
				bzero(line,sizeof(line));
				
				//return NEWLINE;
			}

[ \t]+			{
				charPos+=yyleng;
				if(*yytext!='\t') 
					strcat(line,yytext); 
				else {
					int len = strlen(line);
					line[len++] = ' ';
					line[len] = '\0';
				}
				//printf("**********************************   STRLEN = [%d], yyleng = [%d] for text [%s]\n",strlen(yytext),yyleng,yytext);
				//if(*yytext=='\t'){printf("Tab Found\n");}
				#ifdef LEX_DEBUG_ON
				printf("lex: Whitespace found\n");
				#endif
				
			}
			
"feature"		{	
				#ifdef LEX_DEBUG_ON
				printf("lex: feature\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FEATURE;
			}

"begin"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: begin\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FBEGIN;
			}

"end"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: end\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FEND;
			}

"var"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: var\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FVAR;
			}

"state"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: state\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FSTATE;
			}

"first_match"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: first_match\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FIRSTMATCH;
			}
			
"|->"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: |->\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FCOMPUTE;
			}

"["			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: [\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FLEFT_SQR_BRKT;
			}

"]"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: ]\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FRIGHT_SQR_BRKT;
			}


"("			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: (\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FOPENROUND;
			}

")"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: )\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FCLOSEROUND;
			}

","			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: COMMA\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FCOMMA;
			}
			
"@+"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: @+\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FATPOSEDGE;
			}

"@-"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: @-\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FATNEGEDGE;
			}

"@"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: @\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FATANYEDGE;
			}

"##"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: ##\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FINTIME;
			}
			
"&&"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: &&\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FAND;
			}
			
"||"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: OR\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FOR;
			}

((([0-9]*)\.([0-9]+))|([0-9]+))((([eE])[\+\-]?[0-9]+)?) {	
				#ifdef LEX_DEBUG_ON
				printf("lex: RATIONAL\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FRATIONAL;
			}

\+|\-|\*|\/		{	
				#ifdef LEX_DEBUG_ON
				printf("lex: ARITHOP\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FARITHOP;
			}
			
"true"|"True"|"TRUE"	{	
				#ifdef LEX_DEBUG_ON
				printf("lex: true\n");
				#endif
				charPos+=yyleng;strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FTRUE;
			}	

"=="			{	
				charPos+=yyleng;strcat(line,yytext);
				#ifdef LEX_DEBUG_ON
				printf("lex: EQ\n");
				#endif
				strcpy(yylval.string,yytext);
				return FEQ;
			}
			
"<="			{
				charPos+=yyleng;strcat(line,yytext);
				#ifdef LEX_DEBUG_ON
				printf("lex: LEQ\n");
				#endif
				strcpy(yylval.string,yytext);
				return FLEQ;
			}
			
">="			{
				charPos+=yyleng;strcat(line,yytext);
				#ifdef LEX_DEBUG_ON
				printf("lex: GEQ\n");
				#endif
				strcpy(yylval.string,yytext);
				return FGEQ;
			}
			
"<"			{
				charPos+=yyleng;strcat(line,yytext);
				#ifdef LEX_DEBUG_ON
				printf("lex: LT\n");
				#endif
				strcpy(yylval.string,yytext);
				return FLT;
			}
			
			
">"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: GT\n");
				#endif
				charPos+=yyleng;strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FGT;
			}
			
"="			{	
				charPos+=yyleng;strcat(line,yytext);
				#ifdef LEX_DEBUG_ON
				printf("lex: ASSIGN\n");
				#endif
				strcpy(yylval.string,yytext);
				return FASSIGN;
			}

";"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: SEMICOLON\n");
				#endif
				charPos+=yyleng;strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FSEMICOLON;
			}

":"			{	
				#ifdef LEX_DEBUG_ON
				printf("lex: COLON\n");
				#endif
				charPos+=yyleng;strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FCOLON;
			}

"$time"			{
				#ifdef LEX_DEBUG_ON
				printf("lex: $time\n");
				#endif
				charPos+=yyleng;strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FDOLLARTIME;
			}



"$"			{
				#ifdef LEX_DEBUG_ON
				printf("lex: ANY\n");
				#endif
				charPos+=yyleng;strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FANY;
			}


(([_a-zA-Z]+)([_a-zA-Z0-9\.]*)) {	
				#ifdef LEX_DEBUG_ON
				printf("lex: ATOM\n");
				#endif
				charPos+=yyleng;
				strcat(line,yytext);
				strcpy(yylval.string,yytext);
				return FATOM;
			}

.			{	
				printf("Lexical Error at Line : [%d], Character Position : [%d], Token = [%s], \n",lineNo,charPos+1,yytext);				
				//exit(0);
			}
%%


int yywrap(void) {
    return 1;
}





