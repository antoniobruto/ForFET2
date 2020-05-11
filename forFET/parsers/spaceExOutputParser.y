%{	
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include "structs.h"

	//#include "featureCompute.h"
	//#define PARSER_DEBUG_ON
		
	#ifndef MAX_STR_LENGTH
		#define MAX_STR_LENGTH 1024
	#endif

	void yyerror(char *);
	extern int yylex();
	extern int spaceEx_lineNo,spaceEx_charPos,yyleng,yyspaceEx_lineno,yychar,spaceEx_oldPos;
	extern char spaceEx_line[MAX_STR_LENGTH],spaceEx_oldLine[MAX_STR_LENGTH];
	extern double spaceEx_maxNumber;
	
	extern FILE *yyin;
	extern char* yytext;
	int spaceEx_errNo,spaceEx_err1No,spaceEx_err2No,spaceEx_err3No,spaceEx_err4No,spaceEx_err5No,spaceEx_err6No;
	int spaceEx_currentMatch;
	

	
	enum spaceEx_errorType {
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
	
	
	extern struct interval* featureRange;
%}

%token <string> NEWLINE EQ OPENSQUARE CLOSESQUARE IDENTIFIER NUMBER OPENROUND CLOSEROUND COMMA COLON DOT FINAL

%start file
%union {
	char string[MAX_STR_LENGTH];
	struct interval* intrvl;
}
%type <string> file text alternation locLine;
%type <intrvl> rangeLine range;


%%

file: text COLON rangeLine text COLON alternation
	{
		#ifdef PARSER_DEBUG_ON
			if(featureRange!=NULL){
				printf("Range found [%lf,%lf]\n",featureRange->left,featureRange->right);
			}
		#endif
	};

text: IDENTIFIER text | IDENTIFIER;

alternation:  locLine rangeLine 
				{ 
					if(strcmp($1,"FINAL")==0){
						#ifdef PARSER_DEBUG_ON
							printf("FINAL FOUND 1 \n");
						#endif
						featureRange = $2;
					} 
				} alternation 
			| locLine rangeLine 
			{
				if(strcmp($1,"FINAL")==0){
					#ifdef PARSER_DEBUG_ON
						printf("FINAL FOUND 2 \n");
					#endif
					featureRange = $2;
				} 
			};

locLine: IDENTIFIER COLON IDENTIFIER OPENROUND IDENTIFIER CLOSEROUND EQ IDENTIFIER { 
			#ifdef PARSER_DEBUG_ON 
				printf("%s\n",$8); 
			#endif
		}
	   | IDENTIFIER COLON IDENTIFIER OPENROUND IDENTIFIER CLOSEROUND EQ FINAL { sprintf($$,"FINAL");};

rangeLine: IDENTIFIER DOT IDENTIFIER COLON range { $$ = $5;};

range: OPENSQUARE NUMBER COMMA NUMBER CLOSESQUARE
		{
			$$ = (struct interval*)malloc(sizeof(struct interval));
			$$->left = atof($2);
			$$->right = atof($4);
		};

%%

void yyerror(char *s) {

	char errorMsg[256];
	int flag = 0;
	switch(spaceEx_errNo){
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
	if(strlen(spaceEx_line)==0){
		strcpy(spaceEx_line,spaceEx_oldLine);
		spaceEx_charPos = spaceEx_oldPos;
		spaceEx_lineNo--;
	}
	if(flag==1){
		spaceEx_currentMatch = spaceEx_charPos+1;
	}
	//fprintf(stderr, "%s at Line %d ; %s Match [%d], spaceEx_line :[%s],  ERROR CODES: [%d,%d,%d,%d,%d,%d,%d]\n", s,spaceEx_lineNo,errorMsg,spaceEx_currentMatch,spaceEx_line,spaceEx_errNo,spaceEx_err1No,spaceEx_err2No,spaceEx_err3No,spaceEx_err4No,spaceEx_err5No,spaceEx_err6No);
	fprintf(stderr, "%s at Line %d : %s \n", s,spaceEx_lineNo,errorMsg);
	char *line = trim(spaceEx_line);
	fprintf(stderr, "%s\n",line);
	int i=0;
	for(i=1;i<spaceEx_currentMatch;i++){
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
