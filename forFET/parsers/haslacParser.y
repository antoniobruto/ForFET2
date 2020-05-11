%{	
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <ctype.h>
	#include "structs.h"

	//#include "featureCompute.h"
	//#define PARSER_DEBUG_ON
	//#define COND_DEBUG
	//#define ID_DEBUG
	
	
	#ifndef MAX_STR_LENGTH
		#define MAX_STR_LENGTH 1024
	#endif

	void yyerror(char *);
	extern int yylex();
	extern int haslac_lineNo,haslac_charPos,yyleng,yyhaslac_lineno,yychar,haslac_oldPos;
	extern char haslac_line[MAX_STR_LENGTH],haslac_oldLine[MAX_STR_LENGTH];
	extern double haslac_maxNumber;
	extern FILE *yyin;
	extern char* yytext;
	int haslac_errNo,haslac_err1No,haslac_err2No,haslac_err3No,haslac_err4No,haslac_err5No,haslac_err6No;
	int haslac_currentMatch;
	
	struct location* loc_root;
	struct automaton* hybridAut;
	extern struct phaver* HA;
	
	char err[MAX_STR_LENGTH];
	char syncLabelTemp[] = "hop\0";
	
	enum haslac_errorType {
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
	
	char P_dummy[1] = {'\0'};
%}

%token <string> NEWLINE MODULE ENDMODULE INPUT OUTPUT PARAM BEGINN END MODE MODE_NEXT DDT PROPERTY ENDPROPERTY INITIALL SETT OUTPUT_VAR TRANS INV TRUE ASSIGN EQ LEQ GEQ LT GT SEMICOLON OPENCURLY CLOSECURLY OPENSQUARE CLOSESQUARE AND ANY IDENTIFIER NUMBER NEXT OPENROUND CLOSEROUND COMMA COLON OP DOT IMPLIES COMMENT 
%start program
%union {
	char string[MAX_STR_LENGTH];
	struct phaver* pha;
	struct automaton* aut;
	struct condition* cond;
	struct identifier* ident;
	struct location* loc;
	struct transition* trans;
	struct initial* init;
	int code;		
}
	
%type <pha> program;
%type <aut> automatonDef locListBlock;
%type <cond> dynamics ddtExpr arith_expr rational invariantProperty conjunction comparison param_list params param assignments assignment reset reset_conjunction comparisons set_comparison;
%type <ident> interface inputs outputs id id_list mode_assignment;
%type <trans> transitionProperty ;
%type <loc> locList locn;
%type <init> initially initial_sets set;
%type <code> properties property ineq;

%%

program:
	interface automatonDef ENDMODULE
	{
		$$ = HA;
		
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: PROGRAM MATCH\n");
			displayAutomaton(HA);
			writeSpaceEx(HA,"batt.xml");
		#endif
	};

interface:	MODULE id OPENROUND id_list CLOSEROUND
	{
		struct identifier* signature = duplicateIdentifierList($4);
		if(hybridAut==NULL){			
			hybridAut = createAutomaton(hybridAut,P_dummy,NULL,createIdentifier(syncLabelTemp),NULL);
		}
		strcpy(hybridAut->name,$2->name);
		hybridAut->contr_vars = signature;
		hybridAut->synclabs = createIdentifier(syncLabelTemp);
		
		if(HA==NULL){
			HA = createPhaverStruct(HA,NULL,hybridAut,NULL);
		} else {
			HA->ha = hybridAut;
		}

		$$ = NULL;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: INTERFACE MATCH\n");
		#endif			
	};

automatonDef: locListBlock
	{
		#ifdef PARSER_DEBUG_ON
			printf("\n\n==========>>>  DONE HERE\n");
			printLocationList(loc_root);
		#endif
		
		HA->ha = hybridAut;
		
		#ifdef PARSER_DEBUG_ON		
			printf("HA->ha = [%p] , ha = [%p]\n",HA->ha,hybridAut);
			printf("HA->ha->locations = [%p]\n",HA->ha->locations);
			
		#endif
		
		
	} properties initially
	{	
		if(hybridAut==NULL){
			hybridAut = createAutomaton(hybridAut,P_dummy,NULL,NULL,NULL);
			#ifdef PARSER_DEBUG_ON
				printf("======>>>>>> HA WAS NULL\n");
			#endif
		}

		if(HA==NULL){
			HA = createPhaverStruct(HA,NULL,NULL,NULL);
		}
		HA->ha = hybridAut;
		HA->init = $4;
		
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: AUTOMATON MAvTCH\n");
			displayAutomaton(HA);
			printf("\n");
		#endif
	}|
	locListBlock
	{
		#ifdef PARSER_DEBUG_ON
			printf("\n\n==========>>>  DONE HERE\n");
			printLocationList(loc_root);
		#endif
		
		HA->ha = hybridAut;
		
		#ifdef PARSER_DEBUG_ON		
			printf("HA->ha = [%p] , ha = [%p]\n",HA->ha,hybridAut);
			printf("HA->ha->locations = [%p]\n",HA->ha->locations);
			
		#endif
		
		
	} initially
	{	
		if(hybridAut==NULL){
			hybridAut = createAutomaton(hybridAut,P_dummy,NULL,NULL,NULL);
			#ifdef PARSER_DEBUG_ON
				printf("======>>>>>> HA WAS NULL\n");
			#endif
		}

		if(HA==NULL){
			HA = createPhaverStruct(HA,NULL,NULL,NULL);
		}
		HA->ha = hybridAut;
		HA->init = $3;
		
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: AUTOMATON MAvTCH\n");
			displayAutomaton(HA);
			printf("\n");
		#endif
	};
	
locListBlock: inputs outputs param_list locList 
	{
		
		//Add the param_list into "pha"
		if(HA==NULL){
			HA = createPhaverStruct(HA,NULL,NULL,NULL);
		}
		HA->params = $3;
		
		//Add locations to the automaton
		loc_root = $4;
		if(hybridAut==NULL){
			hybridAut = createAutomaton(hybridAut,P_dummy,NULL,NULL,NULL);
		} else {
			char *ERR = containsIdentifier($1,hybridAut->contr_vars);
			if(ERR!=NULL){
				yyerror(ERR);
			} 
			
			ERR = containsIdentifier($2,hybridAut->contr_vars);
			if(ERR!=NULL){
				yyerror(ERR);
			}
		}
		#ifdef PARSER_DEBUG_ON
			if(HA->ha == hybridAut){
				printf("=========>>>>> THE ADDRESS IS THE SAME\n");
			} else {
				printf("=========>>>>> NO; THE ADDRESS IS NOT THE SAME\n");
			}
		#endif
		
		hybridAut->locations = $4;
		
		//exit(0);
		#ifdef PARSER_DEBUG_ON
			if(HA->ha->locations == hybridAut->locations){
				printf("=========>>>>> THE LOCATION ADDRESS IS THE SAME\n");
			} else {
				printf("=========>>>>> NO; THE LOCATION ADDRESS IS NOT THE SAME\n");
			}
			printf("PARSER - REDUCE: LOCATION LIST BLOCK MATCH\n");
		#endif
	}
	|outputs param_list locList 
	{	
		//Add the param_list into "pha"
		if(HA==NULL){
			HA = createPhaverStruct(HA,NULL,NULL,NULL);
		}
		HA->params = $2;
		
		//Add locations to the automaton
		loc_root = $3;
		if(hybridAut==NULL){
			hybridAut = createAutomaton(hybridAut,P_dummy,NULL,NULL,NULL);
		} else {
			char *ERR = containsIdentifier($1,hybridAut->contr_vars);
			if(ERR!=NULL){
				yyerror(ERR);
			} 
		}
		#ifdef PARSER_DEBUG_ON
			if(HA->ha == hybridAut){
				printf("=========>>>>> THE ADDRESS IS THE SAME\n");
			} else {
				printf("=========>>>>> NO; THE ADDRESS IS NOT THE SAME\n");
			}
		#endif
		
		hybridAut->locations = $3;
		
		//exit(0);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: LOCATION LIST BLOCK - NO INPUTS MATCH\n");
			if(HA->ha->locations == hybridAut->locations){
				printf("=========>>>>> THE LOCATION ADDRESS IS THE SAME\n");
			} else {
				printf("=========>>>>> NO; THE LOCATION ADDRESS IS NOT THE SAME\n");
			}
			
		#endif
	}|outputs locList 
	{	
		//Add the param_list into "pha"
		if(HA==NULL){
			HA = createPhaverStruct(HA,NULL,NULL,NULL);
		}
		HA->params = NULL;
		
		//Add locations to the automaton
		loc_root = $2;
		if(hybridAut==NULL){
			hybridAut = createAutomaton(hybridAut,P_dummy,NULL,NULL,NULL);
		} else {
			char *ERR = containsIdentifier($1,hybridAut->contr_vars);
			if(ERR!=NULL){
				yyerror(ERR);
			} 
		}
		#ifdef PARSER_DEBUG_ON
			if(HA->ha == hybridAut){
				printf("=========>>>>> THE ADDRESS IS THE SAME\n");
			} else {
				printf("=========>>>>> NO; THE ADDRESS IS NOT THE SAME\n");
			}
		#endif
		
		hybridAut->locations = $2;
		
		//exit(0);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: LOCATION LIST BLOCK - NO INPUTS MATCH\n");
			if(HA->ha->locations == hybridAut->locations){
				printf("=========>>>>> THE LOCATION ADDRESS IS THE SAME\n");
			} else {
				printf("=========>>>>> NO; THE LOCATION ADDRESS IS NOT THE SAME\n");
			}
			
		#endif
	};

inputs: INPUT id_list SEMICOLON
	{
		$$ = $2;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: INPUT LIST MATCH\n");
			printIdentifierList($$);
			printf("\n");
		#endif
	};
	
outputs: OUTPUT id_list SEMICOLON
	{
		$$ = $2;
		//printIdentifierList($$);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: OUTPUT LIST MATCH\n");
			printIdentifierList($$);
			printf("\n");
		#endif
	};
	
param_list: PARAM params SEMICOLON
	{
		$$ = $2;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: PARAM LIST MATCH\n");
			printConditionList($$);
			printf("\n");
		#endif
	};

params:	
	params COMMA param
	{
		$$ = addConditionToList($1,$3);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: PARAMS MATCH\n");
			printConditionList($$);
			printf("\n");
		#endif
	}	
	| param 
	{
		$$ = $1;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: PARAM LIST - PARAM MATCH\n");
			printCondition($$);
			printf("\n");
		#endif
	};
	
param:	id ASSIGN arith_expr
	{
		$$ = createCondition($1->name,$3->LHS,2);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: PARAM MATCH\n");
			printCondition($$);
			printf("\n");
		#endif
	};
	
	
locList: locList locn
	{
		$$ = addLocationToList($1,$2);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: LOCATION LIST - MATCH\n");
		#endif
	}	
	| locn
	{
		$$ = $1;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: LOCATION LIST - LOCATION MATCH\n");
			printf("\n");
		#endif
	};

locn: MODE id BEGINN dynamics END
	{
		$$ = createLocation($2->name,NULL,$4,NULL);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: LOCATION MATCH\n");
		#endif
	};

dynamics: dynamics ddtExpr
	{
		$$ = addConditionToList($1,$2);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: DYNAMICS LIST MATCH\n");
		#endif
	}
	| ddtExpr
	{
		$$ = $1;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: DYNAMICS LIST DDTEXPR MATCH\n");
		#endif
	};
	
ddtExpr: DDT id ASSIGN arith_expr SEMICOLON
	{
		strcat($2->name,"'");
		$$ = createCondition($2->name,$4->LHS,0);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: DDTEXPR MATCH\n");
		#endif
	};
	


properties: properties property 
	{
		#ifdef PARSER_DEBUG_ON 
			printf("PARSER: PROPERTY LIST MATCH\n");
		#endif
	}
	| property
	{
		#ifdef PARSER_DEBUG_ON 
			printf("PARSER: PROPERTY LIST - PROPERTY MATCH\n");
		#endif
	}
	;

property: invariantProperty
	{	
		#ifdef PARSER_DEBUG_ON 
			printf("PARSER: PROPERTY INVARIANT MATCH\n");
		#endif
	}
	| transitionProperty
	{
		#ifdef PARSER_DEBUG_ON 
			printf("PARSER: PROPERTY TRANSITION MATCH\n");
			printTransition($1);
			printf("\n");
		#endif
	}
	;

invariantProperty: PROPERTY INV id MODE EQ id IMPLIES conjunction SEMICOLON ENDPROPERTY
	{
		/*
		 * $3 = Property Name
		 * $6 = Location Name
		 * $8 = Condition
		 *
		 */
		struct location* from = getLocationByName(hybridAut->locations,$6->name);
		if(from==NULL){
			sprintf(err,"\nLocation [%s] of Transition Property [%s] NOT FOUND.\n",$6->name,$3->name);
			yyerror(err);
		} 
		#ifdef PARSER_DEBUG_ON 
			else {
				printf("\nLocation [%p] = [%s]\n",from, from->name);
			}
		#endif
		
		from->invariant = $8;
		$$ = from->invariant;
		
		
		
		#ifdef PARSER_DEBUG_ON
			printf("PARSER: INVARIANT MATCH\n");
		#endif
	};

transitionProperty: PROPERTY TRANS id MODE EQ id AND MODE_NEXT EQ id AND conjunction IMPLIES reset_conjunction SEMICOLON ENDPROPERTY
	{
		/*
		 * $3 = Property Name
		 * $6 = From Location Name
		 * $10 = To Location Name
		 * $12 = Guard Condition
		 * $14 = Reset List
		 *
		 */
		
		struct location* fro = getLocationByName(loc_root,$6->name);
		if(fro==NULL){
			sprintf(err,"\nLocation [%s] of Transition Property [%s] NOT FOUND.\n",$6->name,$3->name);
			yyerror(err);
		} 
		#ifdef PARSER_DEBUG_ON 
		else {
			printf("\nLocation [%p] = [%s]\n",fro, fro->name);
		}
		#endif
		
		struct transition* t = createTransition($12,syncLabelTemp,$14,$10->name);
		//printf("Created Transition\n");fflush(stdout);
		
		//exit(0);
		//printTransition
		fro->transitions = addTransitionToLocationTransitionList(fro,t);
		//printf("Added Transition To Location List\n");fflush(stdout);	
		
		$$ = t;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER: TRANSITION MATCH\n");
			printTransition($$);
			printf("\n");
		#endif
	};
	
id_list: id_list COMMA id
	{
		$$ = addIdentifierToList($1,$3);
		#if defined(PARSER_DEBUG_ON) || defined(ID_DEBUG)
			printf("PARSER - REDUCE: ID LIST\n");
		#endif
	}
	
	|id
	{	
		$$ = $1;
		#if defined(PARSER_DEBUG_ON) || defined(ID_DEBUG)
			printf("PARSER - REDUCE: ID LIST - SINGLE ID\n");
		#endif
	};
	
id:
	IDENTIFIER
	{
		$$ = createIdentifier($1);
		#if defined(PARSER_DEBUG_ON) || defined(ID_DEBUG)
			printf("PARSER - REDUCE: IDENTIFIER [%s] ID Created As [%s]\n",$1,$$->name);
		#endif
	};

rational: 
	 NUMBER
	 {	
			$$ = createCondition($1,P_dummy,-1);
			#ifdef PARSER_DEBUG_ON
				printf("PARSER - REDUCE: RATIONAL NUMBER MATCH\n");
			#endif
	 }
	 |OP NUMBER
         {              
                        char *tempConcat = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
                        sprintf(tempConcat,"%s%s",$1,$2);
                        $$ = createCondition(tempConcat,P_dummy,-1);
                        #ifdef PARSER_DEBUG_ON
                                printf("PARSER - REDUCE: SIGNED RATIONAL NUMBER MATCH\n");
                        #endif
         }
	;
	
conjunction: comparison AND conjunction
	{	
		$$ = addConditionToList($3,$1);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: CONJUNCTION LIST MATCH\n");
		#endif
	}
	| comparison 
	{	
		$$=$1;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: CONJUNCTION LIST - LOGICAL MATCH\n");
		#endif
	}
	;	

comparison: { 	
				#ifdef PARSER_DEBUG_ON
					printf("***************************(1) Expecting an inequality\n"); fflush(stdout);
				#endif
			} arith_expr 
			{ 
				#ifdef PARSER_DEBUG_ON
					printf("***************************(2) Expecting an inequality\n"); fflush(stdout);
				#endif
			} ineq arith_expr
	{	
		#ifdef PARSER_DEBUG_ON
			printf("*************************** (3) Expecting an inequality\n"); fflush(stdout);
		#endif
		$$ = createCondition($2->LHS,$5->LHS,$4);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: LOGICAL MATCH\n");
		#endif		
	}
	| TRUE	
	{	
		$$ = createCondition($1,P_dummy,-1);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: LOGICAL TRUE MATCH\n");
		#endif
	}
	;

arith_expr: 
         OPENROUND      {
                                        #ifdef PARSER_DEBUG_ON
                                                printf("PARSER: OPEN ROUND arith_expr start\n");
                                        #endif
                        } arith_expr 
                        {
                                        #ifdef PARSER_DEBUG_ON
                                                printf("PARSER: Bracketed Arith Expr Found [%s]\n",$3->LHS);
                                        #endif
                        }
                        CLOSEROUND {
                                        $$ = $3;
                                        char *tempConcat = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
                                        sprintf(tempConcat,"( %s )",$3->LHS);
                                        strcpy($$->LHS,tempConcat);
                                                
                                        #ifdef PARSER_DEBUG_ON
                                                printf("PARSER - REDUCE: ARITH EXPR TYPE 1 MATCH\n");
                                        #endif
                                }
        | arith_expr OP arith_expr       {
                                        sprintf($1->LHS,"%s %s %s",$1->LHS,$2,$3->LHS);
                                        $$ = $1;
                                        #ifdef PARSER_DEBUG_ON
                                                printf("PARSER - REDUCE: ARITH EXPR TYPE 2 MATCH\n");
                                        #endif
                                }        
        | id                    {       
                                        $$=createCondition($1->name,P_dummy,-1);//strcpy($$,$1);
                                        #ifdef PARSER_DEBUG_ON
                                                printf("PARSER - REDUCE: ARITH EXPR TYPE 3 MATCH\n");
                                        #endif
                
                                }        
        | rational              {       
                                        $$ = $1;
                                        #ifdef PARSER_DEBUG_ON
                                                printf("PARSER - REDUCE: ARITH EXPR TYPE 5 MATCH\n");
                                        #endif
                                }
        | OP id                 {       //printf("OP = [%s] [%s]\n",$1,$2->name);
                                        char *tempConcat = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
                                        strcpy(tempConcat,$1);
                                        strcat(tempConcat,$2->name);
                                        
                                        $$=createCondition(tempConcat,P_dummy,-1);//strcat($1,$2);strcpy($$,$1);
                                        #ifdef PARSER_DEBUG_ON
                                                printf("PARSER - REDUCE: ARITH EXPR TYPE 4 MATCH\n");
                                        #endif
                                }
        /*| OP arith_expr         {
                                        sprintf($2->LHS,"%s %s",$2,$2->LHS);
                                        $$ = $2;
                                        #ifdef PARSER_DEBUG_ON
                                                printf("PARSER - REDUCE: ARITH EXPR TYPE 4 MATCH\n");
                                        #endif
                                }*/
        
        ;	
	
comparisons: comparisons set_comparison 	
	{
		$$ = addConditionToList($2,$1);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: COMPARISON LIST MATCH\n");
		#endif
	}
	| set_comparison 
	{	
		$$=$1;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: COMPARISON LIST - SET_COMPARTISON MATCH\n");
		#endif
	}
	;

set_comparison: comparison SEMICOLON
	{	
		$$=$1;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: SET COMPARTISON MATCH\n");
		#endif
	};



ineq:EQ	
	{	
		$$ = 0;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: INEQUALITY EQ MATCH\n");
		#endif		
	}
	|LEQ
	{			
		$$ = 1;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: INEQUALITY LEQ MATCH\n");
		#endif
	}
	|GEQ
	{	
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: INEQUALITY GEQ MATCH\n");
		#endif
		$$ =3;		
	}
	|LT
	{	
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: INEQUALITY LT MATCH\n");
		#endif
		$$ = 4;
	}
	|GT	
	{	
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: INEQUALITY GT MATCH\n");
		#endif
		$$ = 5;
	}
	;

initially: INITIALL BEGINN initial_sets END
	{	
		$$ = $3;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: INITIAL LIST  PARAM MATCH\n");
		#endif
	}
	;


initial_sets: initial_sets set
	{
		$$ = addInitialToList($1,$2);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: INITIAL SET LIST MATCH\n");
		#endif
	}
	| set 
	{
		$$ = $1;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: INITIAL SET - SET MATCH\n");
		#endif
	};
	
set: SETT BEGINN mode_assignment comparisons END 
	{
		$$=createInitial($3,$4);
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: INITIAL SET MATCH\n");
		#endif
	};

mode_assignment: MODE EQ id SEMICOLON
	{
		$$ = $3;
		#ifdef PARSER_DEBUG_ON
			printf("PARSER - REDUCE: MODE ASSIGNMENT MATCH\n");
		#endif
	};

assignments: assignments assignment
	{	
		$$ = addConditionToList($1,$2);
		#if defined(PARSER_DEBUG_ON) || defined(COND_DEBUG)
			printf("PARSER - REDUCE: ASSIGNMENT LIST MATCH\n");
			printConditionList($$);
			
			printf("\n\n---------------CONDITION LIST-----------------\n");
			struct condition* tcond = $$;
			while(tcond!=NULL){
				printf("\n");
				printf("%s %s %s ",tcond->LHS,operatorMap(tcond->op),tcond->RHS);	
				tcond = tcond->next;
			}
			printf("\n----------------------------------------------\n");
			printf("\n\n Printing Condition List from structs.h\n");
			printConditionList($$);
		#endif
	}
	| assignment 
	{
		$$ = $1;		
		#if defined(PARSER_DEBUG_ON) || defined(COND_DEBUG)
			printf("PARSER - REDUCE: ASSIGNMENT LIST - ASSIGNMENT MATCH\n");
			printCondition($$);
			printf("\n");
		#endif
	};
	
assignment: id ASSIGN arith_expr SEMICOLON
	{
		$$ = createCondition($1->name,$3->LHS,0);
		#if defined(PARSER_DEBUG_ON) || defined(COND_DEBUG)
			printf("PARSER - REDUCE: ASSIGNMENT MATCH\n");
			printCondition($$);
			printf("\n");
		#endif
	};

reset_conjunction: reset_conjunction AND reset  {	
							$$ = addConditionToList($1,$3);
							#if defined(PARSER_DEBUG_ON) || defined(COND_DEBUG)
								printf("PARSER - REDUCE: RESET CONJUNCTION\n");
								printf("\n\n---------------CONDITION LIST-----------------\n");
								struct condition* tcond = $$;
								while(tcond!=NULL){
									printf("\n");
									printf("%s %s %s ",tcond->LHS,operatorMap(tcond->op),tcond->RHS);	
									tcond = tcond->next;
								}
								printf("\n----------------------------------------------\n");
								printf("\n\n Printing Condition List from structs.h\n");
								printConditionList($$);
								printf("\n");
							#endif
						}
					| reset {	
							$$ = $1;
							#if defined(PARSER_DEBUG_ON) || defined(COND_DEBUG)
								printf("PARSER - REDUCE: RESET CONJUNCTION - RESET\n");
								printCondition($$);
								printf("\n");
							#endif
						};
					
reset: id NEXT EQ arith_expr			{	
							strcat($1->name,$2);
							$$ = createCondition($1->name,$4->LHS,0);
							#if defined(PARSER_DEBUG_ON) || defined(COND_DEBUG)
								printf("PARSER - REDUCE: RESET\n");
								//printf("%s[%p] %s %s [%p] (%s)\n",$$->LHS,$$->LHS,operatorMap($$->op),$$->RHS,$$->RHS,$4->LHS);	
								
								
								printf("2 ---- %s %s %s [%p]\n",$$->LHS,operatorMap($$->op),$$->RHS,$$->RHS);	
								//strcpy($$->RHS,$4->LHS);
								//printf("2* ---- %s %s %s [%p]\n",$$->LHS,operatorMap($$->op),$$->RHS,$$->RHS);	
								//strcpy($$->RHS,syncLabelTemp);
								//printf("2^ ---- %s %s %s [%p] \n",$$->LHS,operatorMap($$->op),$$->RHS,$$->RHS);	
								printCondition($$);
								printf("\n");
								//exit(0);
							#endif
						};

%%
/*char* trim(char *str) {
	  char *end;

	// Trim leading space
	while(isspace(*str)) str++;

	if(*str == 0)  // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;

	// Write new null terminator
	*(end+1) = 0;

	return str;
}
*/

void yyerror(char *s) {

	char errorMsg[256];
	int flag = 0;
	switch(haslac_errNo){
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
	if(strlen(haslac_line)==0){
		strcpy(haslac_line,haslac_oldLine);
		haslac_charPos = haslac_oldPos;
		haslac_lineNo--;
	}
	if(flag==1){
		haslac_currentMatch = haslac_charPos+1;
	}
	//fprintf(stderr, "%s at Line %d ; %s Match [%d], haslac_line :[%s],  ERROR CODES: [%d,%d,%d,%d,%d,%d,%d]\n", s,haslac_lineNo,errorMsg,haslac_currentMatch,haslac_line,haslac_errNo,haslac_err1No,haslac_err2No,haslac_err3No,haslac_err4No,haslac_err5No,haslac_err6No);
	fprintf(stderr, "%s at Line %d : %s \n", s,haslac_lineNo,errorMsg);
	char *line = trim(haslac_line);
	fprintf(stderr, "%s\n",line);
	int i=0;
	for(i=1;i<haslac_currentMatch;i++){
		printf(" ");
	}
	printf("^\n");
	exit(0);
	
	
}

/*int main() {
	yyin = fopen("featureAutOut.pha", "r");
	yyparse();
	//printf("MAX = %lf\n",haslac_maxNumber);
	return 0;
	
}
*/

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
