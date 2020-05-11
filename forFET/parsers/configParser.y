%{
        //#define YACC_DEBUG_ON
        
        #ifndef MAX_STR_LENGTH
                #define MAX_STR_LENGTH 10240
        #endif
        
        #include <stdio.h>
        #include <stdlib.h>
        #include <string.h>
        #include <float.h>
        #include "structs.h"
        
        //Function Declarations
        void yyerror(char *);
        
        //External Objects
        extern int yylex();
        extern int config_lineNo,config_charPos,yyleng,yylineno,yychar,config_oldPos;
        extern char config_line[MAX_STR_LENGTH],config_oldLine[MAX_STR_LENGTH];
        extern FILE *yyin;
        extern char* yytext;
	extern int config_multiLine;
	extern struct config* forFETConfig;
        int config_errNo,config_err1No,config_err2No,config_err3No,config_err4No,config_err5No,config_err6No;
        int config_currentMatch;
        char EF_dummy[1] = {'\0'};

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
%token <string> EEQ COMMA SPACEEX DREACH WORK LIBS PATH; 

//Start Production Rule
%start inputList

//Data Types Used for Tokens and Reduction Rules
%union {
                char string[MAX_STR_LENGTH];                
        }
        
%type <string> spaceExPATH dReachPATH libPATH workPATH inputList;

%%

inputList:
		inputList spaceExPATH {
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: Matched input-spaceExPATH statement\n");
							#endif
							if(!forFETConfig){
								forFETConfig = createConfig();
							}
							setSpaceExPath(forFETConfig,$2);
							//sprintf(forFETConfig->spaceExPATH,"%s",$2);
							
						}
	|	inputList dReachPATH		{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: Matched input-dReachPATH statement\n");
							#endif
							if(!forFETConfig){
								forFETConfig = createConfig();
							}
							setDReachPath(forFETConfig,$2);
							//sprintf(forFETConfig->dReachPATH,"%s",$2);
						}
	|	inputList libPATH	  	{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: Matched input-libPATH statement\n");
							#endif
							if(!forFETConfig){
								forFETConfig = createConfig();
							}
							setLibPath(forFETConfig,$2);
							//sprintf(forFETConfig->libPATH,"%s",$2);
						}
	|	inputList workPATH 	{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: Matched input-delayResolution statement\n");
							#endif
							if(!forFETConfig){
								forFETConfig = createConfig();
							}
							setWorkPath(forFETConfig,$2);
							//sprintf(forFETConfig->workPATH,"%s",$2);
						}
	|	spaceExPATH			{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: Matched input-spaceExPATH statement\n");
							#endif
							if(!forFETConfig){
								forFETConfig = createConfig();
							}
							setSpaceExPath(forFETConfig,$1);
							//sprintf(forFETConfig->spaceExPATH,"%s",$1);
							
						}
	|	dReachPATH  		{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: Matched input-dReachPATH statement\n");
							#endif
							if(!forFETConfig){
								forFETConfig = createConfig();
							}
							setDReachPath(forFETConfig,$1);
							//sprintf(forFETConfig->dReachPATH,"%s",$1);
						}
	|	libPATH 		{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: Matched input-libPATH statement\n");
							#endif
							if(!forFETConfig){
								forFETConfig = createConfig();
							}
							setLibPath(forFETConfig,$1);
							//sprintf(forFETConfig->libPATH,"%s",$1);
							
						}
	|	workPATH  			{	
							#ifdef YACC_DEBUG_ON 
								printf("PARSER: Matched input-delayResolution statement\n");
							#endif
							if(!forFETConfig){
								forFETConfig = createConfig();
							}
							setWorkPath(forFETConfig,$1);
							//sprintf(forFETConfig->workPATH,"%s",$1);
						};
	
spaceExPATH: SPACEEX EEQ PATH {
					        #ifdef YACC_DEBUG_ON 
                                                        printf("PARSER: Matched spaceExPATH statement\n");
                                                #endif
                                                //printf("[%s]\n",$3);
                                                strcpy($$,$3); 
					};
	
dReachPATH: DREACH EEQ PATH {
					        #ifdef YACC_DEBUG_ON 
                                                        printf("PARSER: Matched dReachPATH statement\n");
                                                #endif
                                                //printf("[%s]\n",$3);
                                                strcpy($$,$3); 
					};

libPATH: LIBS EEQ PATH {
					        #ifdef YACC_DEBUG_ON 
                                                        printf("PARSER: Matched libPATH statement\n");
                                                #endif
                                                //printf("[%s]\n",$3);
                                                strcpy($$,$3); 
					};

workPATH: WORK EEQ PATH {
					        #ifdef YACC_DEBUG_ON 
                                                        printf("PARSER: Matched workPATH statement\n");
                                                #endif
                                                //printf("[%s]\n",$3);
                                                strcpy($$,$3); 
					};

	
	
	
%%


void yyerror(char *s) {

        char errorMsg[MAX_STR_LENGTH];
        int flag = 0;
        switch(config_errNo){
                /*case BLOCK_NO_NEWLINE:        sprintf(errorMsg,"New statements must fall on a new line.");
                                        config_currentMatch = config_charPos;
                                        break;*/
                case IF_NO_TEST:        sprintf(errorMsg,"Expected a logical condition expression AFTER \"if\".");
                                        break;
                case IF_NO_COLON:       sprintf(errorMsg,"Expected a \":\" after the condition expression.");
                                        flag = 1;
                                        break;
                case TEST_EQ_EXPR:      sprintf(errorMsg,"The condition expression can't contain an assignment operator. Use Logical Operators only.");
                                        break;
                case WHILE_NO_COLON:    sprintf(errorMsg,"Expected a \":\" after the condition expression.");
                                        flag = 1;
                                        break;
                case WHILE_NO_TEST:     sprintf(errorMsg,"Expected a logical condition expression AFTER \"while\".");
                                        break;
                case PRINT_INVALID :    sprintf(errorMsg,"Invalid print statement syntax.");
                                        break;
                case TEST_NO_LOGICAL:   sprintf(errorMsg,"Expected a logical operator between the two expressions.");
                                        flag = 1;
                                        break;
                case EXPR_INVALID:      sprintf(errorMsg,"Invalid Expression Format.");
                                        break;
                default:                sprintf(errorMsg,"Hey Didn't expect to see this here.");
                                        flag = 1;
                                        break;
        }
        if(strlen(config_line)==0){
                strcpy(config_line,config_oldLine);
                config_charPos = config_oldPos;
                config_lineNo--;
        }
        if(flag==1){
                config_currentMatch = config_charPos+1;
        }
        //fprintf(stderr, "%s at Line %d ; %s Match [%d], line :[%s],  ERROR CODES: [%d,%d,%d,%d,%d,%d,%d]\n", s,config_lineNo,errorMsg,config_currentMatch,line,config_errNo,config_err1No,config_err2No,config_err3No,config_err4No,config_err5No,config_err6No);
        fprintf(stderr, "[ConfigParser] %s at Line %d : %s \n", s,config_lineNo,errorMsg);
        fprintf(stderr, "%s\n",config_line);
        int i=0;
        for(i=1;i<config_currentMatch;i++){
                printf(" ");
        }
        printf("^\n");
        exit(0);
        
        
}

/*
int main(int argc, char *argv[]) {
        if(argc<3){
                
                printf("\nINCORRECT SYNTAX\n");
                
                printf("\nExpected Syntax: Booleanize <Config File Name> <CSV File Name>\n\n");
                
                exit(0);
        }
                
        yyin = fopen(argv[1], "r");
        traceFileName = argv[2];
        setbuf(stdout, NULL);
        yyparse();
}
*/
