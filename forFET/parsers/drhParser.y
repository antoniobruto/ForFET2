%{	
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include "structs.h"
	//#define PARSER_DEBUG_ON
		
	#ifndef MAX_STR_LENGTH_DRH
		#define MAX_STR_LENGTH_DRH 4086
	#endif

	void yyerror(char *);
	extern int yylex();
	extern int drh_lineNo,drh_charPos,yyleng,yydrh_lineno,yychar,drh_oldPos;
	extern char drh_line[MAX_STR_LENGTH],drh_oldLine[MAX_STR_LENGTH];
	extern double drh_maxNumber;
	extern float maxNumber;
	
	extern FILE *yyin;
	extern char* yytext;
	int drh_errNo,drh_err1No,drh_err2No,drh_err3No,drh_err4No,drh_err5No,drh_err6No;
	int drh_currentMatch;

	
	enum drh_errorType {
		//BLOCK_NO_NEWLINE = 1,
		IF_NO_TEST = 2,
		IF_NO_COLON = 3,
		TEST_EQ_EXPR = 4,
		WHILE_NO_COLON = 5,
		WHILE_NO_TEST = 6,
		PRINT_INVALID = 7,
		TEST_NO_LOGICAL = 8,
		EXPR_INVALID = 9
	};
	
	
	//struct interval* featureRange = NULL;
	extern FILE *drhOut;// = fopen("models/drhTemplate.drh","w");
%}

%token <string> NEWLINE TEXT COLON GOAL LBRACK RBRACK DRATIONAL COMMA

%start file
%union {
	char string[MAX_STR_LENGTH_DRH];
}
%type <string> file text;

%%

file: text;
	

text: text text	
	|GOAL COLON {
		#ifdef PARSER_DEBUG_ON
			printf("FOUND REAL GOAL\n");
		#endif
		fclose(drhOut);
		//exit(0);
	}
	| TEXT
	{	
		if(strncmp($1,"goal",4)==0){
			#ifdef PARSER_DEBUG_ON
				printf("FOUND  TEXT GOAL\n");
			#endif
			fclose(drhOut);
			//exit(0);
		}
		
		if(strncmp($1,"[",1)==0){
			#ifdef PARSER_DEBUG_ON
				printf("FOUND INTERVAL for %s\n",$1);
			#endif
			char temp[MAX_STR_LENGTH_DRH];
			sprintf(temp,"%s",index($1,']')+2);
			fprintf(drhOut,"[-%f,%f] %s",maxNumber,maxNumber,temp);
		}
		else  fprintf(drhOut,"%s",$1);
	}
	| NEWLINE
	{	
		fprintf(drhOut,"\n");
	}
	| LBRACK interval RBRACK TEXT {
		#ifdef PARSER_DEBUG_ON
				printf("FOUND INTERVAL for %s\n",$4);
			#endif
		fprintf(drhOut,"[%f,%f] %s",drh_maxNumber,drh_maxNumber,$4);
	}
	;

interval: DRATIONAL COMMA DRATIONAL ;

%%

void yyerror(char *s) {

	char errorMsg[256];
	int flag = 0;
	switch(drh_errNo){
		case IF_NO_TEST:	sprintf(errorMsg,"Expected a logical condition expression AFTER \"if\".");
					break;
		case IF_NO_COLON:	sprintf(errorMsg,"Expected a \":\" after the condition expression.");
					flag = 1;
					break;
		case TEST_EQ_EXPR:	sprintf(errorMsg,"The condition expression can't contain an assignment operator. Use Logical Operators only.");
					break;
		case WHILE_NO_COLON:	sprintf(errorMsg,"Expected a \":\" after the condition expression.");
					flag = 1;
					break;
		case WHILE_NO_TEST:	sprintf(errorMsg,"Expected a logical condition expression AFTER \"while\".");
					break;
		case PRINT_INVALID :	sprintf(errorMsg,"Invalid print statement syntax.");
					break;
		case TEST_NO_LOGICAL:	sprintf(errorMsg,"Expected a logical operator between the two expressions.");
					flag = 1;
					break;
		case EXPR_INVALID:	sprintf(errorMsg,"Invalid Expression Format.");
					break;
		default:		sprintf(errorMsg,"Hey Didn't expect to see this here.");
					flag = 1;
					break;
	}
	if(strlen(drh_line)==0){
		strcpy(drh_line,drh_oldLine);
		drh_charPos = drh_oldPos;
		drh_lineNo--;
	}
	if(flag==1){
		drh_currentMatch = drh_charPos+1;
	}
	//fprintf(stderr, "%s at Line %d ; %s Match [%d], drh_line :[%s],  ERROR CODES: [%d,%d,%d,%d,%d,%d,%d]\n", s,drh_lineNo,errorMsg,drh_currentMatch,drh_line,drh_errNo,drh_err1No,drh_err2No,drh_err3No,drh_err4No,drh_err5No,drh_err6No);
	fprintf(stderr, "%s at Line %d Character Position : [%d], Token = [%s]\n%s \n", s,drh_lineNo,drh_charPos+1,yytext,errorMsg);
	char *line = trim(drh_line);
	fprintf(stderr, "%s\n",line);
	int i=0;
	for(i=1;i<drh_currentMatch;i++){
		printf(" ");
	}
	printf("^\n");
	exit(0);
	
	
}

/*
int main(int argc, char **argv){
	
	if( argc > 1 ) {
		FILE *fin = fopen(argv[1],"r");
		if(fin){
			yyin=fin;
			
		}
		yyparse();
	} else {
		printf("Incorrect Syntax\n");
		
	}
	return 0;
}

*/
