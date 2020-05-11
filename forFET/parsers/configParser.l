/*****************************************************************
 *                      CODE WRITTEN BY                          *
 *                                                               *
 *             Antonio Anastasio Bruto da Costa                  *
 *     Formal Method Laboratory, Dept. of CSE, IIT Kharagpur     *
 *                                                               *
 *****************************************************************/

%{
        //#define LEX_DEBUG_ON
        
        #ifndef MAX_STR_LENGTH
                #define MAX_STR_LENGTH 10240
        #endif
        
        #include "configParser.tab.h"
        #include <stdlib.h>
        #include <string.h>
        
        void yyerror(char *);
        int config_lineNo = 1;
        int config_charPos, config_oldPos;
        int config_multiLine = 0;
        char config_line[MAX_STR_LENGTH];
        char config_oldLine[MAX_STR_LENGTH];
%}

%%

[\n\r]                  {
								#ifdef LEX_DEBUG_ON
									printf("lex: NEWLINE\n");
								#endif
								
								if(*yytext=='\n') ++config_lineNo;
								config_oldPos=config_charPos;
								config_charPos = 0;
								if(config_multiLine==0){
									strcpy(config_oldLine,config_line);
									bzero(config_line,sizeof(config_line));
									config_multiLine = 1;
								}
								//return NEWLINE;
                        }

[ \t]+                  {
                                config_charPos+=yyleng;
				strcat(config_line,yytext); 
                                /*if(*yytext!='\t') 
                                        strcat(config_line,yytext); 
                                else {
                                        int len = strlen(config_line);
                                        config_line[len++] = ' ';
                                        config_line[len] = '\0';
                                }
                                */
                                //printf("**********************************   STRLEN = [%d], yyleng = [%d] for text [%s]\n",strlen(yytext),yyleng,yytext);
                                //if(*yytext=='\t'){printf("Tab Found\n");}
                                #ifdef LEX_DEBUG_ON
                                printf("lex: Whitespace found\n");
                                #endif
                                
                        }                    
                        
"spaceEx_PATH"		{       config_multiLine=0;
                                #ifdef LEX_DEBUG_ON
                                printf("lex: keyword [traceFile]\n");
                                #endif
                                config_charPos+=yyleng;
                                strcat(config_line,yytext);
                                strcpy(yylval.string,yytext);
                                return SPACEEX;
                        }
                        
"dReach_PATH"		{       config_multiLine=0;
                                #ifdef LEX_DEBUG_ON
                                printf("lex: keyword [seqLength]\n");
                                #endif
                                config_charPos+=yyleng;
                                strcat(config_line,yytext);
                                strcpy(yylval.string,yytext);
                                return DREACH;
                        }

"work_PATH"			{       config_multiLine=0;
                                #ifdef LEX_DEBUG_ON
                                printf("lex: keyword [delayRes]\n");
                                #endif
                                config_charPos+=yyleng;
                                strcat(config_line,yytext);
                                strcpy(yylval.string,yytext);
                                return WORK;
                        }
                        
"libs_PATH"			{       config_multiLine=0;
                                #ifdef LEX_DEBUG_ON
                                printf("lex: keyword [depth]\n");
                                #endif
                                config_charPos+=yyleng;
                                strcat(config_line,yytext);
                                strcpy(yylval.string,yytext);
                                return LIBS;
                        }
                        
[A-Za-z\_\.\/][0-9A-Za-z_\.\-\/]* {
								config_multiLine=0;
								#ifdef LEX_DEBUG_ON
									printf("lex: PATH\n");
								#endif
								config_charPos+=yyleng;
								strcat(config_line,yytext);
								strcpy(yylval.string,yytext);
								return PATH;
					}

                        
"="                    {	config_multiLine=0;
                                config_charPos+=yyleng;
                                #ifdef LEX_DEBUG_ON
                                printf("lex: EEQ\n");
                                #endif
                                strcat(config_line,yytext);
                                strcpy(yylval.string,yytext);
                                return EEQ;
                        }
                        
","                    {	config_multiLine=0;
                                config_charPos+=yyleng;
                                #ifdef LEX_DEBUG_ON
                                printf("lex: COMMA\n");
                                #endif
                                strcat(config_line,yytext);
                                strcpy(yylval.string,yytext);
                                return COMMA;
                        }


.                       {       
                                printf("Lexical Error at Line : [%d], Character Position : [%d], Token = [%s], \n",config_lineNo,config_charPos+1,yytext);                            
                                //exit(0);
                        }
%%


int yywrap(void) {
    return 1;
}





