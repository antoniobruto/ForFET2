%{	
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include "structs.h"
	//#define PARSER_DEBUG_ON
		
	#ifndef MAX_STR_LENGTH
		#define MAX_STR_LENGTH 4086
	#endif

	void yyerror(char *);
	extern int yylex();
	extern int drhOut_lineNo,drhOut_charPos,yyleng,yydrhOut_lineno,yychar,drhOut_oldPos;
	extern char drhOut_line[MAX_STR_LENGTH],drhOut_oldLine[MAX_STR_LENGTH];
	extern double drhOut_maxNumber;
	
	extern FILE *yyin;
	extern char* yytext;
	int drhOut_errNo,drhOut_err1No,drhOut_err2No,drhOut_err3No,drhOut_err4No,drhOut_err5No,drhOut_err6No;
	int drhOut_currentMatch;
	

	
	enum drhOut_errorType {
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
	//FILE *drhOutOut = fopen("models/drhOutTemplate.drhOut","w");
	extern int kValue;
	extern int lValue;
%}

%token <string> NEWLINE TEXT NUMBER SAT DELTA UNSAT KVAL DREAL COLON SMT FILENAME FSLASH DOT

%start fileLine
%union {
	char string[MAX_STR_LENGTH];
}
%type <string> fileLine text path;

%%

fileLine: fileLine fileLine
	|DREAL text NEWLINE{
		#ifdef PARSER_DEBUG_ON
			printf("DREAL SPEC: %s\n",drhOut_line);
		#endif
	}
	|KVAL NUMBER text UNSAT DOT NEWLINE{
		#ifdef PARSER_DEBUG_ON
			printf("Type1: %s\n",drhOut_line);
		#endif
	}
	|KVAL NUMBER text SAT NEWLINE
	{
		kValue = atoi($2); 
		#ifdef PARSER_DEBUG_ON
			printf("\n************************\n kValue = %d\n",kValue); 
			printf("Type2: %s\n",drhOut_line);
		#endif
	}
	|SMT COLON path text NEWLINE
	{	
		/*if(strncmp($1,"goal",4)==0){
			#ifdef PARSER_DEBUG_ON
				printf("FOUND GOAL");
			#endif
			fclose(drhOutOut);
			//exit(0);
		}
		fprintf(drhOutOut,"%s",$1);
		*/
		
		#ifdef PARSER_DEBUG_ON
			printf("$3 = %s, $4 = %s\n",$3,$4);
		#endif
		char *pos = $3;
		char *num = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
		bzero(num,sizeof(char)*MAX_STR_LENGTH);
		char *temp = num;
		int uf = 0;
		while(*pos!='\0' && uf<3){
			if(*pos == '_' && (++uf==2)){
				while(*pos!='.'){
					pos++;
					*temp=*pos;
					temp++;
				}
				*temp='\0';
				lValue=atoi(num);
				break;
			}
			pos++;
		}
		
		#ifdef PARSER_DEBUG_ON
			printf("\n************************\n lValue = %d\n",lValue); 
			printf("Type 3: Text: %s \n",drhOut_line);
		#endif
	}
	| DELTA text NEWLINE
	{
		#ifdef PARSER_DEBUG_ON
			printf("Type2: %s\n",drhOut_line);
		#endif
	}
	| NUMBER
	{	
		//fprintf(drhOutOut,"\n");
		#ifdef PARSER_DEBUG_ON
			printf("Number: %s \n",drhOut_line);
		#endif
	}
	| UNSAT NEWLINE
	{	
		//fprintf(drhOutOut,"\n");
		#ifdef PARSER_DEBUG_ON
			printf("UNSAT: %s \n",drhOut_line);
		#endif
	};
	
path: text FSLASH path { sprintf($$,"%s %s %s",$1,$2,$3);}
	//| FILENAME FSLASH path {sprintf($$,"%s %s %s",$1, $2, $3);}
	//| DOT FSLASH path { sprintf($$,"%s %s %s",$1,$2,$3);}	
	| FILENAME { sprintf($$,"%s",$1);};
	

text: text TEXT { sprintf($$,"%s %s",$1,$2);}
	| TEXT { sprintf($$,"%s",$1);}
	| text NUMBER { sprintf($$,"%s %s",$1,$2);}
	| NUMBER text { sprintf($$,"%s %s",$1,$2);}
	| NUMBER { sprintf($$,"%s",$1);}
	| DOT { sprintf($$,"%s",$1);}
	| text COLON { sprintf($$,"%s %s ",$1,$2);}
	| COLON text { sprintf($$,"%s %s",$1,$2);}
	| COLON { sprintf($$,"%s",$1);};
%%

void yyerror(char *s) {

	char errorMsg[256];
	int flag = 0;
	switch(drhOut_errNo){
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
	if(strlen(drhOut_line)==0){
		strcpy(drhOut_line,drhOut_oldLine);
		drhOut_charPos = drhOut_oldPos;
		drhOut_lineNo--;
	}
	if(flag==1){
		drhOut_currentMatch = drhOut_charPos+1;
	}
	//fprintf(stderr, "%s at Line %d ; %s Match [%d], drhOut_line :[%s],  ERROR CODES: [%d,%d,%d,%d,%d,%d,%d]\n", s,drhOut_lineNo,errorMsg,drhOut_currentMatch,drhOut_line,drhOut_errNo,drhOut_err1No,drhOut_err2No,drhOut_err3No,drhOut_err4No,drhOut_err5No,drhOut_err6No);
	fprintf(stderr, "%s at Line %d Character Position : [%d], Token = [%s]\n%s \n", s,drhOut_lineNo,drhOut_charPos+1,yytext,errorMsg);
	char *line = trim(drhOut_line);
	fprintf(stderr, "%s\n",line);
	int i=0;
	for(i=1;i<drhOut_currentMatch;i++){
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
