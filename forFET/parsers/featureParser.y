%{
	//#define YACC_DEBUG_ON
	
	#ifndef MAX_STR_LENGTH
		#define MAX_STR_LENGTH 256
	#endif
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <float.h>
	#include "structs.h"
	//#include "featureCompute.h"
	//#include "featureStructs.h"
	
	//Function Declarations
	void yyerror(char *);
	
	//External Objects
	extern int yylex();
	extern int lineNo,charPos,yyleng,yylineno,yychar,oldPos;
	extern char line[MAX_STR_LENGTH],oldLine[MAX_STR_LENGTH];
	extern FILE *yyin;
	extern char* yytext;
	extern struct phaver* HA;
	extern struct phaver* featureHA;
	
	//Global Objects
	extern struct feature* sysFeature;
	int errNo,err1No,err2No,err3No,err4No,err5No,err6No;
	int currentMatch;
	int porvID = 1;
	int temporalFlag = 0;
	char F_dummy[1] = {'\0'};
	char* syncLabel = NULL;			//Will eventually be a user input
	
	//Error Types : Will be eventually useful when the parsers show detailed, helpful parse error messages
	enum errorType {
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
%}

//Lexical Tokens
%token <string> FEATURE FBEGIN FEND FVAR FCOMPUTE FLEFT_SQR_BRKT FRIGHT_SQR_BRKT FOPENROUND FCLOSEROUND FCOMMA FATPOSEDGE FATNEGEDGE FATANYEDGE FINTIME FAND FOR FRATIONAL FARITHOP FTRUE FASSIGN FEQ FLEQ FGEQ FLT FGT FSEMICOLON FCOLON FANY FATOM FDOLLARTIME FSTATE FIRSTMATCH;

//Start Production Rule
%start featureSpec

//Data Types Used for Tokens and Reduction Rules
%union {
		char string[1024];
		struct identifier* id;
		struct condition* cond;
		struct PORV* porvType;
		struct eventType* event;
		struct PORVExpression* porvExpr;
		struct expression* subSeqExpr;
		struct timeDelay* delay;
		struct sequenceExpr* seq;
		struct featureDef* def;
		struct feature* featureSkeleton;
		int code;
	}
%type <string> param rational;
%type <featureSkeleton> featureSpec;
%type <id> params varDecl;
%type <def> featureDefinition featureBody featureStatement;
%type <cond> assignments assignment arithExpr arithStatement;
%type <seq> sequenceExpression subExpression;
%type <delay> timing time;
%type <subSeqExpr> mixedExpr;
%type <porvExpr> PORVExprWrapper PORVExpr;
%type <event> eventExpr;
%type <porvType> porv BPORVconjunct PORVconjunct;
%type <code> ineq eventType;
%%
featureSpec://struct feature*
	FEATURE FATOM FOPENROUND params FCLOSEROUND FSEMICOLON featureDefinition	{	
												#ifdef YACC_DEBUG_ON 
													printf("PARSER: Finito - featureSpec (A) \n");
												#endif
												//printf("HERE0\n");fflush(stdout);
												$$ = createFeature($2,$4,$7);
												//printFeature($$);
												sysFeature = $$;
												//printf("HERE1\n");fflush(stdout);
												if(temporalFlag == 1){
													tuneForTemporalProperties(HA);
												}
												//printf("HERE2\n");fflush(stdout);
												//displayAutomaton(HA);
											}	
	|FEATURE FATOM FSEMICOLON featureDefinition					{	
												#ifdef YACC_DEBUG_ON 
													printf("PARSER: Finito - featureSpec (B)\n");
												#endif
												$$ = createFeature($2,NULL,$4);
												//printFeature($$);
												sysFeature = $$;
												
												if(temporalFlag == 1){
													tuneForTemporalProperties(HA);
												}
												//displayAutomaton(HA);
											}
	;

params: 
	param FCOMMA params			{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: Parameter List\n");
							#endif
							$$=addIdentifierToList(createIdentifier($1),$3);
							//$$=addToIdentifierList($3,$1);
						}	
	
	| param					{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: Single Parameter [%s]\n",$1);
							#endif
							
							$$=createIdentifier($1);
						}
	;
param:	
	FATOM					{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: Single FATOM [  %s  ]\n",$1);
							#endif
							strcpy($$,$1);
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	;
featureDefinition:
	FBEGIN featureBody FEND			{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: Feature Definition : BEGIN featureBody END\n");
							#endif
							$$ = $2;
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	;

featureBody:
	varDecl featureStatement		{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: featureBody: varDecl featureStatement\n");
							#endif
							$2->localVars = $1;
							$$ = $2;
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	| featureStatement			{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: featureBody: featureStatement\n");
							#endif
							$$ = $1;
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	;

varDecl:
	FVAR params FSEMICOLON			{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: varDecl: FVAR params : [   %s  ]\n",$1);
								printIdentifierList($2);
							#endif
							$$ = $2;
							addLocalVariables(HA,$$,DBL_MIN,0);
							/*
							//Add each local variable to the set of control variable of the HA
							struct identifier *var = $$;
							printf("\n------------------Local Variables--------------\n");fflush(stdout);
							while(var!=NULL){								
								printIdentifier(var); printf("[%p] ->next [%p]\n",var,var->next);
								printf("\n");
								//Add each local variable to the set of control variable of the HA
								addLocalVariable(HA,var,DBL_MIN,0);
								var = var->next;
							}
							*/
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	;

featureStatement:
	sequenceExpression FCOMPUTE assignments FSEMICOLON
						{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: featureStatement: sequenceExpression FCOMPUTE assignments\n");
							#endif
							$$ = createFeatureDef(NULL,$1,$3);
						}
	;

sequenceExpression: 
	sequenceExpression timing subExpression	{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: sequenceExpression: subExpression timing sequenceExpression\n");
							#endif
							$3->delay = $2;
							
							#ifdef YACC_DEBUG_ON 
								printf("Setting the Time Delay $2 = %p\n",$2);
								printf("After Setting the Time Delay $3->delay = %p\n",$3->delay);
								
								printf("[FINTIME --------------- ] CRETED NODE [%f %f %d]\n",$3->delay->lower,$3->delay->upper,$3->delay->type);
							#endif
							$$ = addSequenceExprToEOfList($1,$3);
							
						}
	| subExpression				{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: sequenceExpression: subExpression \n");
							#endif
							$$ = $1;
						}
	;

timing:	FINTIME time				{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: timing:	FINTIME time\n");
							#endif
							$$ = $2;
							
							#ifdef YACC_DEBUG_ON 
								printf("$2 = %p\n",$2);
								printf("$$ = %p\n",$$);
							
								printf("HA->temporal = %d\n",HA->temporal);
							#endif
							temporalFlag = 1;
							/*if(HA->temporal==0)
								tuneForTemporalProperties(HA);
							
							#ifdef YACC_DEBUG_ON 
							if(HA->temporal==0){
								printf("ERROR TEMPORAL\n");
								exit(0);
							}
							#endif 
							*/
							#ifdef YACC_DEBUG_ON 
								printf("[FINTIME --------------- ] CRETED NODE [%f %f %d]\n",$$->lower,$$->upper,$$->type);
							#endif
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	;

time:	FLEFT_SQR_BRKT rational FCOLON rational FRIGHT_SQR_BRKT	{	
									#ifdef YACC_DEBUG_ON 
										printf("PARSER: time");//: %s %s %s %s %s\n",$1,$2,$3,$4,$5);
									#endif
									
									$$ = createTimeDelay(atof($2),atof($4),0);
									
									#ifdef YACC_DEBUG_ON 
										printf("Time = ##[%f:%f]\n",$$->lower,$$->upper);
										printf("Return %p\n",$$);
									#endif
									//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
								}
	| FLEFT_SQR_BRKT rational FCOLON FANY FRIGHT_SQR_BRKT	{	
									#ifdef YACC_DEBUG_ON 
										printf("PARSER: time");//: %s %s %s %s %s\n",$1,$2,$3,$4,$5);
									#endif
									
									$$ = createTimeDelay(atof($2),0,1);
									#ifdef YACC_DEBUG_ON 
										printf("Time = ##[%f:]\n",$$->lower);
										printf("Return %p\n",$$);
									#endif
									//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
								}
	| rational				{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: time: %s\n",$1);
							#endif
							
							$$ = createTimeDelay(atof($1),0,2);printf("Time = ##%f\n",$$->lower);
							#ifdef YACC_DEBUG_ON 
								printf("Return %p\n",$$);
							#endif
							
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	;
subExpression://struct sequenceExpr*
	mixedExpr 				{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: subExpression: mixedExpr\n");
							#endif
							$$ = createSequenceExpr($1,NULL,NULL);
							//Split on Event
							if(syncLabel==NULL){
								syncLabel = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
								strcpy(syncLabel,HA->ha->synclabs->name);
							}
							//splitOnEvent(HA,$$,syncLabel);
						}
	| mixedExpr FCOMMA assignments		{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: subExpression: mixedExpr FCOMMA assignments\n");
							#endif
							$$ = createSequenceExpr($1,NULL,$3);
							// Split on event
							#ifdef YACC_DEBUG_ON 
								printf("Created Sequence Expr HERE");
							#endif
							
							if(syncLabel==NULL){
								syncLabel = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
								#ifdef YACC_DEBUG_ON 
									printf("syncLabel HERE");
								#endif
								strcpy(syncLabel,HA->ha->synclabs->name);
								#ifdef YACC_DEBUG_ON 
									printf("copied HERE");
								#endif
							}
							#ifdef YACC_DEBUG_ON 
								printf("before split HERE");
							#endif
							//splitOnEvent(HA,$$,syncLabel);
							#ifdef YACC_DEBUG_ON 
								printf("split HERE");
							#endif
						}
	;

mixedExpr://TYPE : struct expression* 
	PORVExprWrapper				{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: mixedExpr: PORVExprWrapper\n");
							#endif
							$$ = createExpression($1,NULL);
						}
	| PORVExprWrapper FAND eventExpr	{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: mixedExpr:  PORVExprWrapper FAND eventExpr\n");
							#endif
							$$=createExpression($1,$3);
						}
	| eventExpr				{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: mixedExpr: eventExprs\n");
							#endif
							$$ = createExpression(NULL,$1);		
							
						}
	| eventExpr FAND PORVExprWrapper 	{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: mixedExpr:  PORVExprWrapper FAND eventExpr\n");
							#endif
							$$=createExpression($3,$1);
						}
	;

PORVExprWrapper:
	PORVExpr				{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: PORVExprWrapper: PORVExpr\n");
							#endif
							$$ = $1;
						}
	| FOPENROUND PORVExpr FCLOSEROUND	{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: PORVExprWrapper: FOPENROUND PORVExpr FCLOSEROUND	\n");
							#endif
							$$ = $2;
						}
	| FOPENROUND PORVExprWrapper FCLOSEROUND{
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: PORVExprWrapper: FOPENROUND PORVExprWrapper FCLOSEROUND	\n");
							#endif
							$$ = $2;
						}
	;

PORVExpr:
	BPORVconjunct		{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: PORVExpr: BPORVconjunct\n");
							#endif
							$$ = createPORVExpression($1);
							
						}
	| FIRSTMATCH FOPENROUND BPORVconjunct FCLOSEROUND {
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: PORVExpr: FirstMatch BPORVconjunct\n");
							#endif
							$$ = createPORVExpression($3);
							$$->firstMatch = 1;
						}
	| BPORVconjunct FOR PORVExpr {	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: PORVExpr: BPORVconjunct FOR PORVExpr\n");
							#endif
							struct PORVExpression* porvExpr = createPORVExpression($1);
							$$ = addPORVExpressionToEOfList($3,porvExpr);	
						}
	| FIRSTMATCH FOPENROUND BPORVconjunct FCLOSEROUND FOR PORVExpr {	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: PORVExpr: BPORVconjunct FOR PORVExpr\n");
							#endif
							struct PORVExpression* porvExpr = createPORVExpression($3);
							porvExpr->firstMatch = 1;
							$$ = addPORVExpressionToEOfList($6,porvExpr);	
						}
	;

BPORVconjunct:
	FOPENROUND PORVconjunct FCLOSEROUND	{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: BPORVconjunct: FOPENROUND PORVconjunct FCLOSEROUND\n");
							#endif
							
							$$ = $2;
							
							#ifdef YACC_DEBUG_ON 
								printPORVConjunct($$);
							#endif
							
							struct PORV* porvConjunct = $$;
							//printPORVConjunct($$);
							
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
							if(syncLabel==NULL){
								syncLabel = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
								sprintf(syncLabel,"hop");
							}
							//printf("HERE\n");fflush(stdout);
							//HA = splitPORVConjunct(HA,$$,syncLabel);
						}
	;

PORVconjunct:
	porv FAND PORVconjunct			{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: PORVconjunct: porv FAND PORVconjunct\n");
							#endif
							$$ = addPORVToList($3,$1);
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	|porv					{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: PORVconjunct: porv\n");
							#endif
							
							$$=$1;
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	;
	
porv:	arithExpr ineq arithExpr		{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: porv: arithExpr ineq arithExpr\n");
							#endif
							struct condition* temp = createCondition($1->LHS,$3->LHS,$2);
							//$1->op = $2;
							//strcpy($1->RHS,$3->LHS);
							//$$ = createPORV($1,porvID++);
							$$ = createPORV(temp,porvID++);
							#ifdef YACC_DEBUG_ON 
								printf("====================== [%p] [%p]==============\n",$1,$1->next);
								if($1 == $1->next){
									exit(0);
								}
							#endif
							//strcpy($$,$1);
						}
	| FTRUE					{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: porv: FTRUE\n");
							#endif
							
							struct condition* temp = createCondition($1,F_dummy,-1);
							$$ = createPORV(temp,-1);
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	| FSTATE ineq param			{
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: porv: FSTATE ineq param\n");
							#endif
							struct condition* stateCond = createCondition($1,$3,$2);
							$$ = createPORV(stateCond,0);
						}
	
	;

eventExpr:
	eventType FOPENROUND porv FCLOSEROUND	{
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: eventExpr: eventType FOPENROUND porv FCLOSEROUND\n");
							#endif
							
							$$=createEvent($1,$3);
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	| FIRSTMATCH FOPENROUND eventType FOPENROUND porv FCLOSEROUND FCLOSEROUND{
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: eventExpr: eventType FOPENROUND porv FCLOSEROUND\n");
							#endif
							
							$$=createEvent($3,$5);
							$$->firstMatch = 1;
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	;

ineq:
	FLT					{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: ineq: FLT [  %s  ]\n",$1);
							#endif
							yyerror("A '<' inequality is not allowed\n");
							$$ =4;//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	|FGT					{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: ineq: FGT [  %s  ]\n",$1);
							#endif
							yyerror("A '>' inequality is not allowed\n");
							$$ = 5;//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	|FLEQ					{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: ineq: FLEQ [  %s  ]\n",$1);
							#endif
							$$ = 1;//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	|FGEQ					{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: ineq: FGEQ [  %s  ]\n",$1);
							#endif
							$$ = 3;//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	|FEQ					{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: ineq: FEQ [  %s  ]\n",$1);
							#endif
							$$ = 0;//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	;
	
eventType:
	FATPOSEDGE				{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: eventType: FATPOSEDGE [  %s  ]\n",$1);
							#endif
							$$ = 1;
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	|FATNEGEDGE				{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: eventType: FATNEGEDGE [  %s  ]\n",$1);
							#endif
							$$ = -1;
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	|FATANYEDGE				{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: eventType: FATANYEDGE [  %s  ]\n",$1);
							#endif
							
							$$ = 0;
							//$$=createCondition($1->name,F_dummy,-1);//strcpy($$,$1);
						}
	;

arithExpr:
	arithStatement FARITHOP arithExpr		{	
								#ifdef YACC_DEBUG_ON 
									printf("PARSER: Complex Arith Expr\n");	
								#endif
								sprintf($1->LHS,"%s %s %s",$1->LHS,$2,$3->LHS);
								$$ = $1;
								//strcat($2,$3);strcat($1,$2);strcpy($$,$1);
							}
	| arithStatement				{
								#ifdef YACC_DEBUG_ON 
									printf("PARSER: Simple Arith Statement\n");
								#endif
								
								$$ = $1;
								//strcpy($$,$1);
							}
	
	;

arithStatement:
	  param						{	
								#ifdef YACC_DEBUG_ON 
									printf("PARSER: Linear Expr is an ID\n");
								//strcpy($$,$1);
								#endif
							
								$$=createCondition($1,F_dummy,-1);
								//strcpy($$,$1);
							}
	| FARITHOP param				{
								#ifdef YACC_DEBUG_ON 
									printf("PARSER: Linear Expr is an ID\n");
								//strcpy($2,$1);//strcpy($$,$1);
								#endif
								
								sprintf($1,"%s %s",$1,$2);
								$$=createCondition($1,F_dummy,-1);
								
								//strcat($1,$2);strcpy($$,$1);
							}
	| rational					{	
								#ifdef YACC_DEBUG_ON 
									printf("PARSER: Linear Expr is a Rational\n");
								#endif
								
								$$=createCondition($1,F_dummy,-1);
								//strcpy($$,$1);
							}
	| FDOLLARTIME					{	
								#ifdef YACC_DEBUG_ON 
									printf("PARSER: Linear Expr is $TIME\n");
								#endif
								
								$$=createCondition("time",F_dummy,-1);
								#ifdef YACC_DEBUG_ON 
									printf("HA->temporal = %d\n",HA->temporal);
								#endif
								
								temporalFlag = 1;
								/*if(HA->temporal==0)
									tuneForTemporalProperties(HA);
								
								#ifdef YACC_DEBUG_ON 
								if(HA->temporal==0){
									printf("ERROR TEMPORAL\n");
									exit(0);
								}
								#endif
								*/
								
								//strcpy($$,$1);
							}
	;

assignments:
	assignment FCOMMA assignments			{	
								#ifdef YACC_DEBUG_ON 
									printf("PARSER: assignments: assignment FCOMMA assignments\n");
								#endif
								
								$$ = addConditionToList($3,$1);
								
								//$$=createCondition($1->name,F_dummy,-1);
								//strcpy($$,$1);
							}
	| assignment					{	
								#ifdef YACC_DEBUG_ON 
									printf("PARSER: assignments: assignment\n");
								#endif
								
								$$ = $1;
								
								//$$=createCondition($1->name,F_dummy,-1);
								//strcpy($$,$1);
							}
	;
	
assignment: 
	FATOM FASSIGN arithExpr				{	
								#ifdef YACC_DEBUG_ON 
									printf("PARSER: assignment: FATOM FASSIGN arithExpr [  %s %s %s  ]\n",$1,$2,$3);
								#endif
								
								$$=createCondition($1,$3->LHS,2);//strcpy($$,$1);
							}
	;
	
rational: FRATIONAL			{	
								#ifdef YACC_DEBUG_ON 
									printf("PARSER: Linear Expr is a Rational\n");
								#endif
								
								//$$=createCondition($1,F_dummy,-1);
								strcpy($$,$1);
							}
		| FARITHOP FRATIONAL
							{	
								#ifdef YACC_DEBUG_ON 
									printf("PARSER: Linear Expr is a Rational\n");
								#endif
								
								//$$=createCondition($1,F_dummy,-1);
								sprintf($$,"%s%s",$1,$2);
								//strcpy($$,$1);
							}
		;

%%


void yyerror(char *s) {

	char errorMsg[MAX_STR_LENGTH];
	int flag = 0;
	switch(errNo){
		/*case BLOCK_NO_NEWLINE:	sprintf(errorMsg,"New statements must fall on a new line.");
					currentMatch = charPos;
					break;*/
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
	if(strlen(line)==0){
		strcpy(line,oldLine);
		charPos = oldPos;
		lineNo--;
	}
	if(flag==1){
		currentMatch = charPos+1;
	}
	//fprintf(stderr, "%s at Line %d ; %s Match [%d], line :[%s],  ERROR CODES: [%d,%d,%d,%d,%d,%d,%d]\n", s,lineNo,errorMsg,currentMatch,line,errNo,err1No,err2No,err3No,err4No,err5No,err6No);
	fprintf(stderr, "%s at Line %d : %s \n", s,lineNo,errorMsg);
	fprintf(stderr, "%s\n",line);
	int i=0;
	for(i=1;i<currentMatch;i++){
		printf(" ");
	}
	printf("^\n");
	exit(0);
	
	
}
/*
int main(int argc, char *argv[]) {
	yyin = fopen(argv[1], "r");
	setbuf(stdout, NULL);
	yyparse();
}

*/
