#ifndef MAX_STR_LENGTH
	#define MAX_STR_LENGTH 1024
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <ctype.h>
#include <map>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */
#include <sys/time.h> /* for time */
#include <sys/stat.h> /* for file/directory status */
#include <fcntl.h>

#include <glib-object.h>
#include <json-glib/json-glib.h>

#include "structs.h"
//#include "featureCompute.h"
#include "drhOutParser.tab.h"

extern FILE* drhOutin;
extern FILE* drhOutout;
extern int drhOutparse(void);
extern int kValue;
extern int lValue;
extern FILE* drhOut;
extern FILE* runFile;
static int traceCount = 0;

/*-----------------------Operator Maps-------------------------*/
char* SpaceExOpMap(int op)
{
	char* ret = (char*)malloc(sizeof(char)*6);
	switch(op)
	{
		case -99 : sprintf(ret, " ");
		break;
		case 0 : sprintf(ret, "==");
		break;
		case 1 : sprintf(ret, "&lt;=");
		break;
		case 2 : sprintf(ret, "=");
		break;
		case 3 : sprintf(ret, "&gt;=");
		break;
		case 4 : sprintf(ret, "&lt;");
		break;
		case 5 : sprintf(ret, "&gt;");
		break;
		case 6 : sprintf(ret, "&amp;");
		break;
		default : sprintf(ret, "==");
	}
	return ret;
}

char* operatorMap(int op){	
	char *p=(char *)malloc(sizeof(char)*3);
	switch(op){
		case 0: sprintf(p,"==");break;
		case 1: sprintf(p,"<=");break;
		case 2: sprintf(p,":=");break;
		case 3: sprintf(p,">=");break;
		case 4: sprintf(p,"<");break;
		case 5: sprintf(p,">");break;
		default:sprintf(p,"==");break;
		
	}
	//printf("%s",p);
	return p;
}

/*-----------------------CONDITION----------------------------*/

struct condition* createCondition(char *LHS, char* RHS, int op){
	#ifdef DEBUG_ON 
		printf("[createCondition] STARTED\n");
	#endif
	struct condition* cond = (struct condition*)malloc(sizeof(struct condition));
		
	if(LHS!=NULL)
		strcpy(cond->LHS,LHS);
	if(RHS!=NULL)
		strcpy(cond->RHS,RHS);
	cond->op = op;
	cond->next = NULL;
	#ifdef DEBUG_ON 
		printf("[createCondition] CREATED NODE\n");
	#endif
	return cond;
}

struct condition* addConditionToList(struct condition* root, struct condition* cond){
	#ifdef DEBUG_ON 
		printf("[addConditionToList] STARTED\n");	
		printf("ROOT = %p ; cond = %p\n",root,cond);
	#endif
	struct condition* temp = root;
		
	if(root!=NULL){
		#ifdef DEBUG_ON 
			printf("[addConditionToList] CONDITION LIST NOT EMPTY\n");
		#endif
		
		while(temp->next!=NULL){
			if(temp == cond){
				return root;
			}
			temp=temp->next;
		}
		if(temp == cond){
			return root;
		}
		
		#ifdef DEBUG_ON 
			printf("[addConditionToList] ADDING TO CONDITION LIST\n");
			printf("temp = %p ; cond = %p\n",temp ,cond);
		#endif
			
		temp->next = cond;
	} else {
		#ifdef DEBUG_ON 
			printf("[addConditionToList] CONDITION LIST EMPTY\n");
			printf("[addConditionToList] ADDING TO CONDITION LIST\n");
		#endif
		
		root = cond;
		#ifdef DEBUG_ON 
			printf("[addConditionToList] ROOT CHANGED\n");
		#endif
		
	}
	#ifdef DEBUG_ON 
		printf("[addConditionToList] ADDED CONDITION\n");
	#endif
	
	return root;
}

struct condition* addToConditionList(struct condition* root, char *LHS, char* RHS, int op){
	#ifdef DEBUG_ON 
	printf("[addToConditionList] STARTED\n");
	#endif
	
	struct condition* cond = (struct condition*)malloc(sizeof(struct condition));
	struct condition* temp = root;
	
	strcpy(cond->LHS,LHS);
	strcpy(cond->RHS,RHS);
	cond->op = op;
	cond->next = NULL;
	#ifdef DEBUG_ON 
	printf("[addToConditionList] CRETED NODE\n");
	#endif
	
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[addToConditionList] CONDITION LIST NOT EMPTY\n");
		#endif
		
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
		printf("[addToConditionList] ADDING TO CONDITION LIST\n");
		#endif
		
		temp->next = cond;
		
	} else {
		#ifdef DEBUG_ON 
		printf("[addToConditionList] CONDITION LIST EMPTY\n");
		printf("[addToConditionList] ADDING TO CONDITION LIST\n");
		#endif
		
		root = cond;
		
		#ifdef DEBUG_ON 
		printf("[addToConditionList] ROOT CHANGED\n");
		printf("[addToConditionList] ROOT = %p, NEXT = %p\n",root,root->next);
		#endif
		
	}
	#ifdef DEBUG_ON 
	printf("[addToConditionList] ADDED CONDITION %s %d %s\n",LHS,op,RHS);
	#endif
	
	return root;
}

struct condition* duplicateConditionList(struct condition* root){
	#ifdef DEBUG_ON 
	printf("[duplicateCondition] STARTED\n");
	#endif
	
	struct condition* temp = root;
	struct condition* newRoot = NULL;
	#ifdef DEBUG_ON 
	printf("[root = %p] %p \n",root,root!=NULL?root->next:NULL);
	#endif
	
	while(temp!=NULL){
		newRoot = addToConditionList(newRoot, temp->LHS, temp->RHS, temp->op);
		#ifdef DEBUG_ON 
		printf("[duplicateCondition] DUPLICATING NEXT CONDITION [%p] %s %s %d [%p]\n",temp,temp->LHS,temp->RHS,temp->op,temp->next);
		printf("[root = %p]\n",root);
		//if(temp == temp->next) temp->next=NULL;//exit(0);
		#endif
		
		temp = temp->next;
	}
	#ifdef DEBUG_ON 
	printf("[duplicateCondition] COMPLETED\n");
	#endif
	
	return newRoot;
}

struct condition* createIdentityResets(struct identifier* root){
	struct condition* reset = NULL;
	struct identifier* temp = root;
	char nameD[MAX_STR_LENGTH];
	
	
	while(temp!=NULL){
		sprintf(nameD,"%s'",temp->name);
		reset = addToConditionList(reset,nameD,temp->name,0);
		temp = temp->next;
	}
	return reset;
}

void printCondition(struct condition* cond){
	if(cond!=NULL){
		//printf("[%p] %s %s %s ",cond,cond->LHS,operatorMap(cond->op),cond->RHS);	
		printf("%s %s %s ",cond->LHS,operatorMap(cond->op),cond->RHS);	
	}
}

void printConditionList(struct condition* cond){
	printf("\n\n---------------CONDITION LIST-----------------\n");
	while(cond!=NULL){
		printf("\n");
		printCondition(cond);
		cond = cond->next;
	}
	printf("\n\n----------------------------------------------\n");
}

struct condition* reverseConditionList(struct condition* root){

	if(root==NULL){
		#ifdef DEBUG_ON 
			printf("[reverseConditionList] STARTED AND ENDED - NULL ROOT\n");
		#endif
	
		return NULL;
	}
	#ifdef DEBUG_ON 
		printf("[reverseConditionList] STARTED\n");
	#endif
	struct condition *prev, *current, *next;
	
	current = root;
	next=root->next;
	root->next = NULL;
	
	while(next!=NULL){
		prev = current;
		current = next;
		next = current->next;
		current->next = prev;
	}
	#ifdef DEBUG_ON 
		printf("[reverseConditionList] ENDED - returning current = %d\n",current);
	#endif
	
	return current;
}


void printInvariant(struct condition* invariant){
	while(invariant!=NULL){
		if(invariant->op == -1){
			printf("true");
		} else {
			printf("%s %s %s",invariant->LHS,phOperatorMap(invariant->op),invariant->RHS);
		}
		//printf("%s %s %s",invariant->LHS,phOperatorMap(invariant->op),invariant->RHS);
		if(invariant->next != NULL){
			printf(" & ");
		} else {
			printf("\n\t\t\t");
		}
		invariant = invariant->next;
	}
}

/*
 * Compares if a condition has a specified LHS with a specified RHS
 */
int conditionCompare(struct condition* cond, char* LHS, char* RHS){
        #ifdef DEBUG_ON 
        printf("[conditionCompare] STARTED\n");
        #endif

        if(cond){
                if(LHS){
                        if(strcmp(LHS,cond->LHS)==0){
                                if(RHS){
                                        if(strcmp(RHS,cond->RHS)==0)
                                                return 1;
                                }
                        }
                }
        }
        
        return 0;
        
        #ifdef DEBUG_ON 
        printf("[conditionCompare] STARTED\n");
        #endif
}


/*
 * Checks if a given LHS,RHS pair is present in the list of conditions
 */
int inConditionList(struct condition* list, char* LHS, char* RHS){
        #ifdef DEBUG_ON 
        printf("[inConditionList] STARTED\n");
        #endif

        while(list){
                if(conditionCompare(list,LHS,RHS)){
                        return 1;
                }
                list = list->next;
        }
        
        return 0;
        
        #ifdef DEBUG_ON 
        printf("[inConditionList] STARTED\n");
        #endif
}

int removeConditionFromList(struct condition** root, char* LHS){
        #ifdef DEBUG_ON 
        printf("[removeConditionFromList] STARTED\n");
        #endif
        
        if(*root && LHS){
                //printConditionList(*root);
                struct condition* prev = *root;
                struct condition* curr = *root;
                while(curr){
                        //printf("Comparing [%s] with [%s]\n",curr->LHS,LHS);
                        if(strncmp(curr->LHS,LHS,strlen(LHS))==0){
                                //printf("FOUND\n");
                                if(curr == *root){
                                        //printf("CURRENT IS ROOT\n");
                                        *root = curr->next;
                                        free(curr);
                                } else {
                                        prev->next = curr->next;
                                        free(curr);
                                }
                                break;
                        }
                        prev = curr;
                        curr = curr->next;
                }
                
                //printf("\nNow it is: \n");
                //printConditionList(*root);
        }
        
        #ifdef DEBUG_ON 
        printf("[removeConditionFromList] ENDED\n");
        #endif
}

/*-----------------------IDENTIFIER-----------------------------*/
struct identifier* createIdentifier(char *id){
	#ifdef DEBUG_ON 
	printf("[createIdentifier] STARTED\n");
	#endif
	
	struct identifier* ID = (struct identifier*)malloc(sizeof(struct identifier));
	ID->next = NULL;
	strcpy(ID->name,id);
	#ifdef DEBUG_ON 
	printf("[createIdentifier] CRETED NODE\n");
	#endif
	
	
	return ID;
}

struct identifier* addIdentifierToList(struct identifier* root, struct identifier* ID){
	#ifdef DEBUG_ON 
	printf("[addIdentifierToList] STARTED\n");
		#endif
	
	struct identifier* temp = root;
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[addIdentifierToList] ID LIST NOT EMPTY\n");
		#endif
		
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
		printf("[addIdentifierToList] ADDING TO ID LIST\n");
		#endif
		
		temp->next = ID;
	} else {
		#ifdef DEBUG_ON 
		printf("[addIdentifierToList] ID LIST EMPTY\n");
		printf("[addIdentifierToList] ADDING TO ID LIST\n");
		#endif
		
		root = ID;
		#ifdef DEBUG_ON 
		printf("[addIdentifierToList] ROOT CHANGED: ROOT->NAME = %s\n",root->name);
		#endif
		
	}
	#ifdef DEBUG_ON 
	printf("[addIdentifierToList] ADDED IDENTIFIER [%s]\n",ID->name);
	#endif
	
	return root;
}

struct identifier* addToIdentifierList(struct identifier* root, char *id){
	#ifdef DEBUG_ON 
	printf("[addToIdentifierList] STARTED\n");
	#endif
	
	struct identifier* ID = (struct identifier*)malloc(sizeof(struct identifier));
	struct identifier* temp = root;
	ID->next = NULL;
	strcpy(ID->name,id);
	#ifdef DEBUG_ON 
	printf("[addToIdentifierList] CRETED NODE\n");
	#endif
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[addToIdentifierList] ID LIST NOT EMPTY\n");
		#endif
		
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
		printf("[addToIdentifierList] ADDING TO ID LIST\n");
		#endif
		
		temp->next = ID;
	} else {
		#ifdef DEBUG_ON 
		printf("[addToIdentifierList] ID LIST EMPTY\n");
		printf("[addToIdentifierList] ADDING TO ID LIST\n");
		#endif
		
		root = ID;
		#ifdef DEBUG_ON 
		printf("[addToIdentifierList] ROOT CHANGED: ROOT->NAME = %s\n",root->name);
		#endif
		
	}
	#ifdef DEBUG_ON 
	printf("[addToIdentifierList] ADDED IDENTIFIER [%s]\n",id);
	#endif
	
	return root;
}

char* getIdentifierName(struct automaton* ha){
	//printf("list = [%p]\n",ha->synclabs);
	//printIdentifier(ha->synclabs);
	return ha->synclabs->name;
}

int getIdentifierID(struct identifier* list, char *name){
	struct identifier* temp = list;
	int count=0;
	while(temp!=NULL){
		count++;
		if(strcmp(temp->name,name)==0){
			return count;
		}
		temp=temp->next;
	}	
	return 0;
}

int countIdentifiers(struct identifier* list){
	struct identifier* temp = list;
	int count=0;
	while(temp!=NULL){
		count++;
		temp=temp->next;
	}	
	return count;
}

struct identifier* duplicateIdentifierList(struct identifier* root){
	#ifdef DEBUG_ON 
	printf("[duplicateIdentifierList] STARTED\n");
	#endif
	
	struct identifier* temp = root;
	struct identifier* newRoot = NULL;
	while(temp!=NULL){
		newRoot = addToIdentifierList(newRoot, temp->name);
		temp = temp->next;
	}
	#ifdef DEBUG_ON 
	printf("[duplicateIdentifierList] COMPLETED\n");
	#endif
	
	return newRoot;
}

void freeIdentifierList(struct identifier* id){
	if(id!=NULL){
		freeIdentifierList(id->next);
		free(id);
	}
}

char* containsIdentifier(struct identifier* list1, struct identifier* list2){
	struct identifier* temp = list1;
	while(temp){
		if(getIdentifierID(list2,temp->name)==0){
			char* err = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			sprintf(err,"Identifier [%s] not declared in the the signature of the module.\n",temp->name);
			return err;			
		}
		temp = temp->next;
	}
	return NULL;
}


void printIdentifier(struct identifier* id){
	if(id!=NULL){
		//printf("[%p] %s %s %s ",cond,cond->LHS,operatorMap(cond->op),cond->RHS);
		printf("Addrs: %p\n",id);
		printf("Name: %s\n",id->name);	
		printf("Next: %p\n\n",id->next);
	}
}

void printIdentifierList(struct identifier* id){
	printf("\n\n---------------IDENTIFIER LIST-----------------\n");
	while(id!=NULL){	
		printIdentifier(id);	
		//printf("%s, ",id->name);
		id = id->next;
	}
	printf("\n\n----------------------------------------------\n");
}


/*-----------------------TRANSITION-----------------------------*/

struct transition* createTransition(struct condition* when, char *sync, struct condition* reset, char* gotoLoc){
	#ifdef DEBUG_ON 
	printf("[createTransition] STARTED\n");
	#endif
	
	struct transition* trans = (struct transition*)malloc(sizeof(struct transition));
	
	strcpy(trans->sync,sync);
	strcpy(trans->gotoLoc,gotoLoc);
	trans->when = when;
	trans->reset = reset;
	trans->next = NULL;
	trans->original = 0;
	
	#ifdef DEBUG_ON 
	printf("[createTransition] CRETED NODE\n");
	#endif
	
	return trans;
}

struct transition* addTransitionToList(struct transition* root, struct transition* trans){
	#ifdef DEBUG_ON 
	printf("[addTransitionToList] STARTED\n");
	#endif
	
	struct transition* temp = root;
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[addTransitionToList] TRANSITION LIST NOT EMPTY\n");
		#endif
		
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
		printf("[addTransitionToList] ADDING TO TRANSITION LIST\n");
		#endif
		
		temp->next = trans;
	} else {
		#ifdef DEBUG_ON 
		printf("[addTransitionToList] TRANSITION LIST EMPTY\n");
		printf("[addTransitionToList] ADDING TO TRANSITION LIST\n");
		#endif
		
		root = trans;
		#ifdef DEBUG_ON 
		printf("[addTransitionToList] ROOT CHANGED\n");
		#endif
		
	}
	#ifdef DEBUG_ON 
	printf("[addTransitionToList] ADDED TRANSITION TO [%s]\n",trans->gotoLoc);
	#endif
	
	return root;
}

struct transition* addToTransitionList(struct transition* root, struct condition* when, char *sync, struct condition* reset, char* gotoLoc){
	#ifdef DEBUG_ON 
	printf("[addToTransitionList] STARTED\n");
	#endif
	
	struct transition* trans = (struct transition*)malloc(sizeof(struct transition));
	struct transition* temp = root;
	
	strcpy(trans->sync,sync);
	strcpy(trans->gotoLoc,gotoLoc);
	trans->when = duplicateConditionList(when);
	trans->reset = duplicateConditionList(reset);
	trans->next = NULL;
	trans->original = 0;
	
	#ifdef DEBUG_ON 
	printf("[addToTransitionList] CRETED NODE\n");
	#endif
	
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[addToTransitionList] TRANSITION LIST NOT EMPTY\n");
		#endif
		
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
		printf("[addToTransitionList] ADDING TO TRANSITION LIST\n");
		#endif
		
		temp->next = trans;
	} else {
		#ifdef DEBUG_ON 
		printf("[addToTransitionList] TRANSITION LIST EMPTY\n");
		printf("[addToTransitionList] ADDING TO TRANSITION LIST\n");
		#endif
		
		root = trans;
		#ifdef DEBUG_ON 
		printf("[addToTransitionList] ROOT CHANGED\n");
		#endif
		
	}
	#ifdef DEBUG_ON 
	printf("[addToTransitionList] ADDED TRANSITION TO [%s]\n",gotoLoc);
	printf("[addToTransitionList] ENDED\n");
	#endif
	
	return root;
}

struct transition* addTransitionToLocationTransitionList(struct location* loc, struct transition* trans){
	if(loc!=NULL){
		#ifdef DEBUG_ON
			printf("[addTransitionToLocationTransitionList] Loc not null, loc->transitions = [%p]\n",loc->transitions);
		#endif
			
		loc->transitions = addTransitionToList(loc->transitions,trans);
		return loc->transitions;
	}
}

struct transition* duplicateTransision(struct transition* temp){
	#ifdef DEBUG_ON 
	printf("[duplicateTransition] STARTED\n");
	#endif
	struct transition* newRoot = NULL;
	
	if(temp!=NULL){
		struct condition* when = NULL;
		struct condition* reset = NULL;
		when = duplicateConditionList(temp->when);
		reset = duplicateConditionList(temp->reset);
		newRoot = addToTransitionList(newRoot, when, temp->sync, reset, temp->gotoLoc);		
	}
	
	#ifdef DEBUG_ON 
	printf("[duplicateTransition] COMPLETED\n");
	#endif
	
	return newRoot;
}

struct transition* duplicateTransitionList(struct transition* root){
	#ifdef DEBUG_ON 
	printf("[duplicateTransitionList] STARTED\n");
	#endif
	
	struct transition* temp = root;
	struct transition* newRoot = NULL;
	while(temp!=NULL){
		struct condition* when = NULL;
		struct condition* reset = NULL;
		when = duplicateConditionList(temp->when);
		reset = duplicateConditionList(temp->reset);
		newRoot = addToTransitionList(newRoot, when, temp->sync, reset, temp->gotoLoc);
		temp = temp->next;
	}
	#ifdef DEBUG_ON 
	printf("[duplicateTransitionList] COMPLETED\n");
	#endif
	
	return newRoot;
}

/*
 * Checks if a location has any transition out of the location with the specified level
 */
int locHasTransAtLevel(struct location* loc, int level){
	#ifdef DEBUG_ON 
	printf("[locHasTransAtLevel] STARTED\n");
	#endif

	if(loc){
		struct transition* trans = loc->transitions;
		char levelStr[] = "level";
		char valStr[MAX_STR_LENGTH];
		sprintf(valStr,"%d",level);

		while(trans){
			if(inConditionList(trans->when,levelStr,valStr)){
				#ifdef DEBUG_ON 
					printf("[locHasTransAtLevel] ENDED\n");
				#endif
				return 1;
			}
			trans = trans->next;
		}
	}
	return 0;

	#ifdef DEBUG_ON 
	printf("[locHasTransAtLevel] ENDED\n");
	#endif
}


struct transitionWrapper* addToPseudoTransitionList(struct transitionWrapper* root, struct location* loc, struct transition* trans){
	#ifdef DEBUG_ON 
	printf("[addToPseudoTransitionList] STARTED\n");
	#endif
	
	struct transitionWrapper* temp = root;
	struct transitionWrapper* transW = (struct transitionWrapper*)malloc(sizeof(struct transitionWrapper));
	
	transW->trans = trans;
	transW->loc = loc;
	transW->next=NULL;
	
	#ifdef DEBUG_ON 
	printf("[addToPseudoTransitionList] CRETED NODE\n");
	#endif
	
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[addToPseudoTransitionList] PSEUDO TRANSITION LIST NOT EMPTY\n");
		#endif
		
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
		printf("[addToPseudoTransitionList] ADDING TO PSEUDO TRANSITION LIST\n");
		#endif
		
		temp->next = transW;
		//transW->id = temp->id + 1;
	} else {
		#ifdef DEBUG_ON 
		printf("[addToPseudoTransitionList] PSEUDO TRANSITION LIST EMPTY\n");
		printf("[addToPseudoTransitionList] ADDING TO PSEUDO TRANSITION LIST\n");
		#endif
		
		root = transW;
		//transW->id=0;
		#ifdef DEBUG_ON 
		printf("[addToPseudoTransitionList] ROOT CHANGED\n");
		#endif
		
	}
	#ifdef DEBUG_ON 
	printf("[addToPseudoTransitionList] ADDED PSEUDO TRANSITION [%s]\n",loc->name);
	#endif
	
	return root;
}


struct transition* createTransitionOnEvent(struct condition* eventCond, char* syncLabel, struct condition* resets, char* gotoLoc, struct identifier* localVars){
	if(eventCond!=NULL && syncLabel!=NULL && gotoLoc!=NULL && localVars!=NULL){
		#ifdef DEBUG_ON
			printf("[createTransitionOnEvent] STARTED [eventCond = %p, syncLabel = %s, gotoLoc = [%s], localVars = [%s]\n",eventCond,syncLabel,gotoLoc,localVars);
		#endif	
		/*struct condition* tempNext = eventCond->next;
		eventCond->next = NULL;
		
		struct condition* whenCond = duplicateConditionList(eventCond);
		eventCond->next = tempNext;
		tempNext = NULL;
		*/
		struct transition* trans = createTransition(/*whenCond*/ eventCond, syncLabel, resets, gotoLoc);
		
		//Create Identity Resets
		
		while(localVars!=NULL){
			//Add Identity Resets in Outgoing Transitions
			if(strcmp(localVars->name,"level")!=0 && strcmp(localVars->name,"time")!=0)
				addIdentityResetToTransition(trans,localVars->name);
			
			localVars = localVars->next;
		}
		trans->original = 0;
		return trans;
	}
	
	if(eventCond == NULL && syncLabel!=NULL && gotoLoc!=NULL && localVars!=NULL){
		#ifdef DEBUG_ON
			printf("[createTransitionOnEvent] STARTED [eventCond = NULL, syncLabel = %s, gotoLoc = [%s], localVars = [%s]\n",syncLabel,gotoLoc,localVars);
		#endif
		
		struct transition* trans = createTransition(NULL, syncLabel, NULL, gotoLoc);
		
		//Create Identity Resets
		
		while(localVars!=NULL){
			//Add Identity Resets in Outgoing Transitions
			if(strcmp(localVars->name,"level")!=0)
				addIdentityResetToTransition(trans,localVars->name);
			
			localVars = localVars->next;
		}
		trans->original = 0;
		return trans;		
	}
	return NULL;
}

/*
 * Duplicate contents of "transList"
 */
struct transitionWrapper* duplicatePseudoTransitionList(struct transitionWrapper* transList){
	if(transList!=NULL){
		#ifdef DEBUG_ON 
			printf("[duplicatePseudoTransitionList] STARTED\n");
		#endif
		struct transitionWrapper* transNew = (struct transitionWrapper*)malloc(sizeof(struct transitionWrapper));
		
		transNew->loc = transList->loc;
		transNew->trans = transList->trans;
		
		transNew->next = duplicatePseudoTransitionList(transList->next);
		
		#ifdef DEBUG_ON 
			if(transNew->next == NULL){
				printf("[duplicatePseudoTransitionList] End of TransitionList\n");
			}
			
			printf("[duplicatePseudoTransitionList] CREATED Duplicate TransitionList\n");
		#endif
		
		return transNew;
	}
	return NULL;
}


void freeIncomingTransitions(struct transitionWrapper* incoming){
	#ifdef DEBUG_ON 
		printf("[freeIncomingTransitions] STARTED\n");
	#endif
	struct transitionWrapper* next = NULL;
	//int flag = 0;
	//next = incoming;
/*	while(next!=NULL){
		printf("I'm ok\n");
		flag = 1;
		next = next->next;
	}
	printf("I reached here now\n");fflush(stdout);fflush(stderr);
	while(flag==1 & incoming!=NULL){
		next = incoming->next;
		free(incoming);
		incoming = next;		
	}
	*/
	if(incoming!=NULL){
		freeIncomingTransitions(incoming->next);
		#ifdef DEBUG_ON
			printf("incoming = %p\n",incoming);
		#endif
			
		free(incoming);
	}
	#ifdef DEBUG_ON 
		printf("[freeIncomingTransitions] ENDED\n");
	#endif
}

void processIncomingTransitions(struct location* root){
	#ifdef DEBUG_ON 
		printf("[processIncomingTransitions] STARTED\n");
	#endif
	
	struct location* locList = root;
	//Free All incoming lists
	while(locList!=NULL){
		#ifdef DEBUG_ON 
		printf("Incoming = %p\n",locList->incoming);
		#endif
		freeIncomingTransitions(locList->incoming);
		locList->incoming = NULL;
		locList = locList->next;
	}
	
	#ifdef DEBUG_ON 
		printf("[processIncomingTransitions] Cleant up all incoming transitions\n");
		printf("[processIncomingTransitions] Setting new transitions in place\n");
	#endif
	
	locList = root;
	while(locList!=NULL){
		// For each location in the Automaton		
		struct transition* transList = locList->transitions;
		while(transList!=NULL){
			/* 
			 * For each outgoing transition (source,dest) add an incoming 
			 * transition (source,dest) to dest
			 */
			struct location* dest = getLocationByName(root, transList->gotoLoc);
			dest->incoming = addToPseudoTransitionList(dest->incoming,locList,transList);
			transList = transList->next;
		}
		locList = locList->next;
	}
	
	#ifdef DEBUG_ON 
	printf("[processIncomingTransitions] ENDED\n");
	#endif
}

void printTransition(struct transition* transition){
	struct condition* reset;
	struct condition* when;
	when = transition->when;
	reset= transition->reset;
	
	printf("\n\t\t\twhen ");
	
	while(when!=NULL){
		printf("%s %s %s",when->LHS,phOperatorMap(when->op),when->RHS);
		
		if(when->next != NULL){
			printf(" & ");
		} else {
			printf("\n\t\t\t");
		}
		when=when->next;
	}
	printf("sync %s do { ",transition->sync);
	//int flag = 0; char timeLabel[5]; sprintf(timeLabel,"time");	
	while(reset!=NULL){
		/*
		if(flag == 0 && strcmp(reset->RHS,timeLabel)==0)
			flag = 1;
		if (flag==1 && strcmp(reset->RHS,timeLabel)==0) 
			printf("Houston we have a problem in the list\n");
		*/
		printf("%s %s %s",reset->LHS,"==",reset->RHS);
		if(reset->next != NULL){
			printf(" & ");
		} else {
			printf(" }\n\t\t\t");
		}
		reset=reset->next;
	}
	printf("goto %s;\n",transition->gotoLoc);
}



/*-----------------------INITIAL-----------------------------*/

struct initial* createInitial(struct identifier* init_locs, struct condition* init_conds){
	#ifdef DEBUG_ON 
	printf("[createInitial] STARTED\n");
	#endif
	
	struct initial* init = (struct initial*)malloc(sizeof(struct initial));
	
	init->init_conds = init_conds;
	init->init_locs = init_locs;
	init->next = NULL;
	
	#ifdef DEBUG_ON 
	printf("[createInitial] CRETED NODE\n");
	#endif
	
	
	return init;
}

struct initial* addInitialToList(struct initial* root, struct initial* init){
	#ifdef DEBUG_ON 
	printf("[addInitialToList] STARTED\n");
	#endif
	
	struct initial* temp = root;
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[addInitialToList] INITIAL LIST NOT EMPTY\n");
		#endif
		
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
		printf("[addInitialToList] ADDING TO INITIAL LIST\n");
		#endif
		
		temp->next = init;
	} else {
		#ifdef DEBUG_ON 
		printf("[addInitialToList] INITIAL LIST EMPTY\n");
		printf("[addInitialToList] ADDING TO INITIAL LIST\n");
		#endif
		
		root = init;
		#ifdef DEBUG_ON 
		printf("[addInitialToList] ROOT CHANGED\n");
		#endif
		
	}
	#ifdef DEBUG_ON 
	printf("[addInitialToList] ADDED INITIAL \n");
	#endif
	
	return root;
}

/*
 * Duplicate contents of "Initial"
 */
struct initial* duplicateInitialList(struct initial* init){
	if(init!=NULL){
		#ifdef DEBUG_ON 
			printf("[duplicateInitialList] STARTED\n");
		#endif
		struct initial* initNew = (struct initial*)malloc(sizeof(struct initial));
		
		initNew->init_locs = duplicateIdentifierList(init->init_locs);
		initNew->init_conds = duplicateConditionList(init->init_conds);
		initNew->next = duplicateInitialList(init->next);
		
		#ifdef DEBUG_ON 
			if(initNew->next == NULL){
				printf("[duplicateInitialList] End of Initial List\n");
			}
			
			printf("[duplicateInitialList] CREATED Duplication Initial List\n");
		#endif
		
		return initNew;
	}
	return NULL;
}

/*
 * Duplicate contents of "Initial"
 */
struct initial* duplicateInitialConditions(struct initial* init){
	if(init!=NULL){
		#ifdef DEBUG_ON 
			printf("[duplicateInitialConditions] STARTED\n");
		#endif
		struct initial* initNew = (struct initial*)malloc(sizeof(struct initial));
		
		initNew->init_locs = duplicateIdentifierList(init->init_locs);
		initNew->init_conds = duplicateConditionList(init->init_conds);
		initNew->next = NULL;
		
		#ifdef DEBUG_ON 
			printf("[duplicateInitialConditions] CREATED Duplication Initial List\n");
		#endif
		return initNew;
	}
	return NULL;
}

/*
 * Return initial condition if location is an initial location
 */
struct initial* checkInitial(struct location* loc, struct initial* init){
	if(loc!=NULL && init!=NULL){
		while(init!=NULL){
			if(strcmp(loc->name, init->init_locs->name)==0){
				return init;
			}
			init = init->next;
		}
	} 
	return NULL;
}

void printInitial(struct initial* init){
	struct identifier* init_locs = NULL;
	struct condition* init_conds = NULL;
	printf("\ninitially: ");
	while(init!=NULL){
		init_locs = init->init_locs;
		init_conds = init->init_conds;
		
		while(init_locs!=NULL){
			printf("%s",init_locs->name);
			if(init_locs->next != NULL){
				printf(" & ");
			} else {
				if(init_conds != NULL)
					printf(" & ");
			}
			init_locs=init_locs->next;
		}
		while(init_conds!=NULL){
			printf("%s %s %s",init_conds->LHS,phOperatorMap(init_conds->op),init_conds->RHS);
			if(init_conds->next != NULL){
				printf(" & ");
			} 
			init_conds=init_conds->next;
		}
		if(init->next!=NULL){
			printf(", ");
		} else printf(";\n");
		init = init->next;
	}
}

void printSpaceExConfigInitial(char* name, struct initial* init,FILE* out){
	struct identifier* init_locs = NULL;
	struct condition* init_conds = NULL;
	fprintf(out,"initially = \"");
	while(init!=NULL){
		init_locs = init->init_locs;
		init_conds = init->init_conds;
		//fprintf(out," ( ");
		while(init_locs!=NULL){
			//fprintf(out,"( loc(%s_bind) == %s",name,init_locs->name);
			fprintf(out," loc(%s_bind) == %s",name,init_locs->name);
			if(init_locs->next != NULL){
				fprintf(out," | ");
			} else {
				//if(init_conds != NULL) fprintf(out," ) & ");
				//else fprintf(out," )");
				if(init_conds != NULL) fprintf(out," & ");
				
			}
			init_locs=init_locs->next;
		}
		while(init_conds!=NULL){
			fprintf(out,"%s %s %s",init_conds->LHS,phOperatorMap(init_conds->op),init_conds->RHS);
			if(init_conds->next != NULL){
				fprintf(out," & ");
			} 
			init_conds=init_conds->next;
		}
		if(init->next!=NULL){
			//fprintf(out," ) | ");
			fprintf(out," | ");
		}// else fprintf(out," ) ");
		init = init->next;
		
	}
	fprintf(out,"\"\n");
}


/*-----------------------LOCATION-----------------------------*/


struct location* createLocation(char *name, struct condition* invariant, struct condition* derivative, struct transition* transitions/*,int req*/){
	
	#ifdef DEBUG_ON 
		printf("[createLocation] STARTED\n");
	#endif
	
	struct location* loc = (struct location*)malloc(sizeof(struct location));
		
	strcpy(loc->name,name);
	loc->originalName = NULL;
	loc->invariant = invariant;
	loc->derivative = derivative;
	loc->transitions = transitions;
	//loc->requires = req;
	loc->next = NULL;
	loc->labels = NULL;
	loc->type = 0;
	#ifdef DEBUG_ON 
		printf("[createLocation] CRETED NODE\n");
	#endif
	
	
	return loc;
}

struct location* addLocationToList(struct location* root, struct location* loc){
	#ifdef DEBUG_ON 
		printf("[addLocationToList] STARTED\n");
	#endif
	
	
	struct location* temp = root;
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
			printf("[addLocationToList] LOCATION LIST NOT EMPTY\n");
		#endif
		
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
			printf("[addLocationToList] ADDING TO LOCATION LIST\n");
		#endif
		
		temp->next = loc;
	} else {
		#ifdef DEBUG_ON 
			printf("[addLocationToList] LOCATION LIST EMPTY\n");
			printf("[addLocationToList] ADDING TO LOCATION LIST\n");
		#endif
		
		root = loc;
		#ifdef DEBUG_ON 
			printf("[addLocationToList] ROOT CHANGED\n");
		#endif
		
	}
	#ifdef DEBUG_ON 
	printf("[addLocationToList] ADDED LOCATION [%s]\n",loc->name);
	#endif
	
	return root;
}

struct location* addToLocationList(struct location* root, char *name, struct condition* invariant, struct condition* derivative, struct transition* transitions, int locType/*,int req*/){
	#ifdef DEBUG_ON 
	printf("[addToLocationList] STARTED\n");
	#endif
	
	struct location* loc = (struct location*)malloc(sizeof(struct location));
	struct location* temp = root;
	
	strcpy(loc->name,name);
	loc->invariant = invariant;
	loc->derivative = derivative;
	loc->transitions = transitions;
	//loc->requires = req;
	loc->next = NULL;
	loc->labels = NULL;
	loc->originalName = NULL;
	loc->type = locType;
	#ifdef DEBUG_ON 
	printf("[addToLocationList] CRETED NODE\n");
	#endif
	
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[addToLocationList] LOCATION LIST NOT EMPTY\n");
		#endif
		
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
		printf("[addToLocationList] ADDING TO LOCATION LIST\n");
		#endif
		
		temp->next = loc;
	} else {
		#ifdef DEBUG_ON 
		printf("[addToLocationList] LOCATION LIST EMPTY\n");
		printf("[addToLocationList] ADDING TO LOCATION LIST\n");
		#endif
		
		root = loc;
		#ifdef DEBUG_ON 
		printf("[addToLocationList] ROOT CHANGED\n");
		#endif
		
	}
	#ifdef DEBUG_ON 
	printf("[addToLocationList] ADDED LOCATION [%s]\n",name);
	#endif
	
	return root;
}


struct locationWrapper* addToPseudoLocationList(struct locationWrapper* root, struct location* loc){
	#ifdef DEBUG_ON 
	printf("[addToPseudoLocationList] STARTED\n");
	#endif
	
	struct locationWrapper* temp = root;
	struct locationWrapper* locW = (struct locationWrapper*)malloc(sizeof(struct locationWrapper));
	locW->loc = loc;
	locW->next=NULL;
	#ifdef DEBUG_ON 
	printf("[addToPseudoLocationList] CRETED NODE\n");
	#endif
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[addToPseudoLocationList] LOCATION LIST NOT EMPTY\n");
		#endif
		
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
		printf("[addToPseudoLocationList] ADDING TO LOCATION LIST\n");
		#endif
		
		temp->next = locW;
		locW->id = temp->id + 1;
	} else {
		#ifdef DEBUG_ON 
		printf("[addToPseudoLocationList] LOCATION LIST EMPTY\n");
		printf("[addToPseudoLocationList] ADDING TO LOCATION LIST\n");
		#endif
		
		root = locW;
		locW->id=0;
		#ifdef DEBUG_ON 
		printf("[addToPseudoLocationList] ROOT CHANGED\n");
		#endif
		
	}
	#ifdef DEBUG_ON 
	printf("[addToPseudoLocationList] ADDED LOCATION [%s]\n",loc->name);
	#endif
	
	return root;
}

void printPseudoLocationList(struct locationWrapper *root){
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[printPseudoLocationList] STARTED\n");
		#endif
		
		struct locationWrapper* locationIterator = root;
		while(locationIterator!=NULL){
			
			if(locationIterator->loc->originalName==NULL){
				printf("Unsplit Location [%s]\n",locationIterator->loc->name);
			} else {
				printf(" Split  Location [%s], originally Location [%s]\n",locationIterator->loc->name,locationIterator->loc->originalName);
			} 
			locationIterator = locationIterator->next;
		}
		
		#ifdef DEBUG_ON 
		printf("[printPseudoLocationList] ENDED\n");
		#endif
		
	}
}


void freePseudoLocationList(struct locationWrapper* root){
	#ifdef DEBUG_ON 
	printf("[freePseudoLocationList] STARTED\n");
	#endif
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[freePseudoLocationList] Freeing Next Root [%p]\n",root->next);
		#endif
		
		freePseudoLocationList(root->next);
		#ifdef DEBUG_ON 
		printf("[freePseudoLocationList] Freeing Current Root [%p]\n",root);
		#endif
		free(root);
	}
	#ifdef DEBUG_ON 
	printf("[freePseudoLocationList] ENDED\n");
	#endif
}
/*
 * Duplicate contents of "loc"
 */
struct location* duplicateLocation(struct location* loc){
	if(loc!=NULL){
		#ifdef DEBUG_ON 
			printf("[duplicateLocation] STARTED\n");
		#endif
		struct location* locNew = (struct location*)malloc(sizeof(struct location));
		
		strcpy(locNew->name,loc->name);
		
		locNew->invariant = duplicateConditionList(loc->invariant);
		locNew->derivative = duplicateConditionList(loc->derivative);
		locNew->transitions = duplicateTransitionList(loc->transitions);
		locNew->labels = duplicatePORVList(loc->labels);
		//locNew->requires = loc->requires;
		
		if(loc->originalName!=NULL){
			locNew->originalName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			strcpy(locNew->originalName,loc->originalName);
		} else {
			locNew->originalName = NULL;
		}
		
		#ifdef DEBUG_ON 
			printf("[duplicateLocation] Duplicating Incoming Transition List\n");
		#endif
		
		locNew->incoming = duplicatePseudoTransitionList(loc->incoming);
		locNew->next = NULL;
		
		#ifdef DEBUG_ON 
			printf("[duplicateLocation] CRETED LOCATION\n");
		#endif
			
		return locNew;
	}
	return NULL;
}

/*
char* cleanLocationName(char *locName){
	int iterator = 0;
	char *cleanName = (char *)malloc(sizeof(char) * 1024);
	while(locName[iterator] != '\0' || locName[iterator] != '_'){
		cleanName[iterator] = locName[iterator];
		iterator++;
	}
	cleanName[iterator]='\0';
	return cleanName;
}
*/

int isMarked(struct locationWrapper* markedList, struct location *loc){
	#ifdef DEBUG_ON
		printf("[isMarked] STARTED\n");
	#endif
	
	if(markedList!=NULL && loc!=NULL){
		struct locationWrapper* iterator = markedList;
		while(iterator!=NULL){
			#ifdef DEBUG_ON
				printf("[isMarked] (Split location [TER CHECK) Iterator Location = [%p] checked with Input Location [%p]\n",iterator->loc,loc);
			#endif
			
			if(iterator->loc == loc){
				#ifdef DEBUG_ON
					printf("[isMarked] Found Location Marked. ENDED\n");
				#endif
				return 1;
			}
			iterator = iterator->next;
		}
		#ifdef DEBUG_ON
			printf("[isMarked] Nothing found. ENDED\n");
		#endif
		
		return 0;
	}
	#ifdef DEBUG_ON
	printf("[isMarked] ENDED\n");
	#endif
	return 0;
}

struct location* getLocationByName(struct location* root, char *locName){
	if(root!=NULL){
		struct location* iterator = root;
		while(iterator!=NULL){
			if(strcmp(iterator->name,locName)==0){
				return iterator;
			}
			iterator = iterator->next;
		}
	} else return NULL;
}

//Need to keep track of which location has already been returned
//Do this in a location wrapper list
struct location* getUnmarkedLocation(struct location* root, char *locName, struct locationWrapper* marked){
	#ifdef DEBUG_ON
	printf("[getUnmarkedLocation] STARTED\n");
	#endif
	//Check for Null Location List
	if(root!=NULL){
		//Get the list of marked/visited locations ===> list
		//The current location being analysed in each iteration will be given by ==> iterator
		struct location* iterator = root;
		while(iterator!=NULL){
			//char *cleanName = cleanLocationName(iterator->name);
			#ifdef DEBUG_ON
				char* strTemp = (char*)malloc(sizeof(char)*(isMarked(marked,iterator)?3:7));
				if(isMarked(marked,iterator)){
					sprintf(strTemp,"IS");
				} else sprintf(strTemp,"IS NOT");
				printf("[getUnmarkedLocation] Checking [%s] Location : [%s] and Location %s Marked\n",locName,(iterator->originalName==NULL)?iterator->name:iterator->originalName,strTemp);
			#endif
			
			//If the location has not yet been split. The locations name is it's original name.
			if(iterator->originalName==NULL && isMarked(marked,iterator)==0){
				#ifdef DEBUG_ON
				printf("[getUnmarkedLocation] The location [%s] is unmarked.\n",iterator->name );
				#endif
				if(strcmp(iterator->name,locName)==0){
					//Mark the location as visited
					//*marked = addToPseudoLocationList(*marked,iterator);
					
					#ifdef DEBUG_ON
					printf("[getUnmarkedLocation] Matched [%s] Unsplit Location : [%s]\n",locName,iterator->name);
					#endif
					//Return the location as the next location to split.
					return iterator;
				}
			} else if(iterator->originalName!=NULL){
				//If the location was previously split, we'll be in this block.
				#ifdef DEBUG_ON
				printf("[getUnmarkedLocation] The location has been previously split.\n");
				#endif
				//The locations original name is given by the "originalName" field.
				if(strcmp(iterator->originalName,locName)==0 && isMarked(marked,iterator)==0){
					#ifdef DEBUG_ON
					printf("[getUnmarkedLocation] The location [%s] is unmarked.\n",iterator->name);
					#endif
					//Location is unmarked, mark the location and return it.
					//*marked = addToPseudoLocationList(*marked,iterator);
					
					#ifdef DEBUG_ON
					printf("[getUnmarkedLocation] FOUND : [%s]\n",iterator->originalName);
					#endif
					
					return iterator;
				}
			}
			
			iterator = iterator->next;
		}
	} 
	#ifdef DEBUG_ON
		printf("[getUnmarkedLocation] ENDED\n");
	#endif
	
	return NULL;
}

void printLocation(struct location* loc){
	
	struct condition* invariant=NULL;
	struct condition* derivative=NULL;
	struct transition* transitions=NULL;
	struct condition* when=NULL;
	struct condition* reset=NULL;
	struct transitionWrapper* incoming = NULL;
	printf("\nloc %s:\t\t",loc->name);
	printf("while ");
	invariant=loc->invariant;
	while(invariant!=NULL){
		if(invariant->op == -1){
			printf("true");
		} else {
			printf("%s %s %s",invariant->LHS,phOperatorMap(invariant->op),invariant->RHS);
		}
		
		if(invariant->next != NULL){
			printf(" & ");
		} else {
			printf("\n\t\t\t");
		}
		invariant=invariant->next;
	}
	printf("\twait {");
	derivative=loc->derivative;
	while(derivative!=NULL){
		printf("%s %s %s",derivative->LHS,phOperatorMap(derivative->op),derivative->RHS);
		//printf("%s == %s",derivative->LHS,derivative->RHS);
		if(derivative->next != NULL){
			printf(" & ");
		} else {
			printf(" };\n\t\t\t");
		}
		derivative=derivative->next;
	}
		
		
	//OUTGOING TRANSITIONS
	transitions = loc->transitions;
	//printf("transitions = %p\n",loc->transitions);
	while(transitions!=NULL){
		printTransition(transitions);
		//printf("\n");
		transitions=transitions->next;
	}
			
	//INCOMING TRANSITIONS
	incoming = loc->incoming;
	//printf("********************** INCOMING TRANSITIONS *********************");
	while(incoming!=NULL){
		transitions = incoming->trans;
		when = transitions->when;
		reset= transitions->reset;
		printf("\n\t\tFROM : %s",incoming->loc->name);
		printf("\n\t\t\twhen ");
		while(when!=NULL){
			printf("%s %s %s",when->LHS,phOperatorMap(when->op),when->RHS);
			if(when->next != NULL){
				printf(" & ");
			} else {
				printf("\n\t\t\t");
			}
			when=when->next;
		}
		printf("sync %s do { ",transitions->sync);
			
		while(reset!=NULL){
			printf("%s %s %s",reset->LHS,"==",reset->RHS);
			if(reset->next != NULL){
				printf(" & ");
			} else {
				printf(" }\n\t\t\t");
			}
			reset=reset->next;
		}
		printf("goto %s;\n",transitions->gotoLoc);
		printf("\n");
		incoming=incoming->next;
	}
	printf("loc [%p] tra [%p] \n",loc,loc->transitions);
}

void printLocationList(struct location* l){
	while(l!=NULL){
		printLocation(l);
		l=l->next;
	}
}

//-----------------------------------------------------------------------------


int isTruePORV(struct location* loc, struct PORV* porv){
	#ifdef DEBUG_ON
		printf("[isTruePORV] STARTED\n");
	#endif
	
	if(loc!=NULL && porv!=NULL){
		struct PORV* iterator = loc->labels;
		while(iterator!=NULL){
			if(iterator->id == porv->id){
				#ifdef DEBUG_ON
					printf("[isTruePORV] ENDED\n");
				#endif
				return 1;
			}
			iterator = iterator->next;
		}
	}
	
	#ifdef DEBUG_ON
		printf("[isTruePORV] ENDED\n");
	#endif
	return 0;
}

int isTrueConjunct(struct location* loc, struct PORV* conjunct){
	#ifdef DEBUG_ON
		printf("[isTrueConjunct] STARTED\n");
	#endif
	
	if(loc!=NULL && conjunct!=NULL){
		struct PORV* porv = conjunct;
		struct PORV* iterator;
		int flag =1;
		
		#ifdef DEBUG_ON
			iterator = loc->labels;
			while(iterator!=NULL){
				printf("%d\t",iterator->id);
				iterator = iterator->next;
			}
		
			printf("To compare with: ");
			iterator = porv;
			while(iterator!=NULL){
				printf("%d\t",iterator->id);
				iterator = iterator->next;
			}
			printf("\n\n");
		#endif
		while(porv!=NULL && flag == 1){
			if(porv->id>0){
				iterator = loc->labels;
				flag = 0;
				while(iterator!=NULL){
					if(iterator->id == porv->id){
						#ifdef DEBUG_ON
							printf("[isTrueConjunct] ENDED\n");
						#endif
						flag = 1;
						break;
					}
					iterator = iterator->next;
				}
			}
			porv = porv->next;
		}
		return flag;
	}
	
	#ifdef DEBUG_ON
		printf("[isTrueConjunct	] ENDED\n");
	#endif
	return 0;
}

int isTruePORVExpr(struct location* loc, struct PORVExpression* PORVExpr){
	#ifdef DEBUG_ON
		printf("[isTruePORVExpr] STARTED\n");
	#endif
	
	if(loc!=NULL && PORVExpr!=NULL){
		struct PORVExpression* conjunctWrapper = PORVExpr;
		
		while(conjunctWrapper!=NULL){
			if(isTrueConjunct(loc,conjunctWrapper->conjunct) == 1)
				return 1;
			
			conjunctWrapper = conjunctWrapper->next;
		}
		
	}
	
	#ifdef DEBUG_ON
		printf("[isTruePORVExpr	] ENDED\n");
	#endif
	return 0;
}

int isTrueExpr(struct location* loc, struct expression* expr){
	#ifdef DEBUG_ON
		printf("[isTrueExpr] STARTED\n");
	#endif
	int eventFlag = 0; int exprFlag = 0;
	if(loc!=NULL && expr!=NULL){
		if(expr->event!=NULL){
			eventFlag = isTruePORV(loc,expr->event->porv);
			//printf("[%s] EVENTFLAG = %d\n",loc->name,eventFlag);
		} else eventFlag = 1;
		
		#ifdef DEBUG_ON
			printf("[%s] EVENTFLAG = %d\n",loc->name,eventFlag);
		#endif
		
		if(expr->dnf!=NULL){
			exprFlag = isTruePORVExpr(loc,expr->dnf);
			//printf("[%s] exprFlag = %d\n",loc->name,exprFlag);
		}
		else exprFlag = 1;
		
		#ifdef DEBUG_ON
			printf("[%s] exprFlag = %d\n",loc->name,exprFlag);
		#endif
			
		if(eventFlag==1 && exprFlag==1){

			#ifdef DEBUG_ON
				printf("[isTrueExpr] ENDED\n");
			#endif
			
			return 1;
		}	
	}
	
	#ifdef DEBUG_ON
		printf("[isTrueExpr] ENDED\n");
	#endif
	
	if(expr->dnf == NULL && expr->event==NULL){
		return 1;
	}
	
	return 0;
}

/*-----------------------AUTOMATON-----------------------------*/

struct automaton* createAutomaton(struct automaton* root, char* name, struct identifier* contr_vars, struct identifier* synclabs, struct location* locations){
	#ifdef DEBUG_ON 
		printf("[addAutomaton] STARTED\n");
	#endif
	
	struct automaton* ha = (struct automaton*)malloc(sizeof(struct automaton));
	struct automaton* temp = root;
	
	strcpy(ha->name,name);
	ha->contr_vars = contr_vars;
	ha->synclabs = synclabs;
	ha->locations = locations;
	processIncomingTransitions(locations);
	#ifdef DEBUG_ON 
		printf("[addAutomaton] CRETED NODE\n");
	#endif
	fflush(stdout);
	fflush(stderr);
	
	root = ha;
	#ifdef DEBUG_ON 
		printf("[addAutomaton] ROOT CHANGED\n");
	
		printf("[addAutomaton] ADDED NODE\n");
	#endif
	
	return root;
}

/*-----------------------PHAVER-----------------------------*/

struct phaver* createPhaverStruct(struct phaver* root, struct condition* params, struct automaton* ha, struct initial* init){
	#ifdef DEBUG_ON 
		printf("[addPhaver] STARTED\n");
	#endif
	struct phaver* pha = (struct phaver*)malloc(sizeof(struct phaver));
	root = pha;
	pha->params = params;
	pha->ha = ha;
	pha->init = init;
	pha->temporal = 0;
	
	#ifdef DEBUG_ON 
		printf("[addPhaver] CRETED NODE\n");
		printf("[addPhaver] ROOT CHANGED\n");
		printf("[addPhaver] ADDED NODE\n");
	#endif
	return pha;
}

/*-------------------------CONFIG---------------------------*/
struct config* createConfig(){
	return (struct config*)malloc(sizeof(struct config));
}

/*-----------------------MISCELLANEOUS-----------------------------*/

int charInList(char *c, char *list, int count){
	int i=0;
	for(i=0;i<count;i++){
		if(*c==list[i])
			return 1;
	}
	return 0;
	
}

void zeroDerivative(char *eqn, char *var){//Equation, varname to look for
	char list[14]={'{','}','(',')','[',']','+','-','*','/',' ','%','&','='};
	
	int eqnLen = strlen(eqn);
	int varLen = strlen(var);
	
	if(var!=NULL && eqn!=NULL){
		int i,j,k;
		for(i=0;i<eqnLen;i++){
			if(eqn[i]==var[0]){
				if((i!=0 && (charInList(&eqn[i-1],list,14)==1))||i==0){
					for(j=0;j<varLen && eqn[i+j]==var[j];j++);
					if(j==varLen){
						if(eqnLen==(i+j)||(eqnLen>(i+j)&&charInList(&eqn[i+j],list,14))) {
							//REPLACE STRING
							k=0;
							for(k=eqnLen;k>(i+varLen-1);k--){
								eqn[k+4]=eqn[k];
							}
							eqn[i]='(';
							eqn[i+1]='0';
							eqn[i+2]='*';
							for(k=0;k<varLen;k++){
								eqn[i+3+k]=var[k];
							}
							eqn[i+varLen+3]=')';
							eqnLen=strlen(eqn);
							i=i+varLen+4;
						}
					}
				}
			}
		}
	}
}

/*-----------------------Event-------- ---------------------*/

struct eventType* createEvent(int type, struct PORV* porv){
	#ifdef DEBUG_ON 
		printf("[createEvent] STARTED\n");
	#endif
	struct eventType* event = (struct eventType*)malloc(sizeof(struct eventType));
	
	event->type = type;
	event->porv = porv;
	event->firstMatch = 0;
	#ifdef DEBUG_ON 
		printf("[createEvent] CRETED NODE\n");
	#endif
		
	return event;
}


/*-----------------------PORV-------- ---------------------*/

struct PORV* createPORV(struct condition* cond,int id){
	#ifdef DEBUG_ON 
		printf("[createPORV] STARTED\n");
	#endif
	struct PORV* porv = (struct PORV*)malloc(sizeof(struct PORV));
	
	porv->porv = cond;
	porv->id = id;
	porv->next = NULL;
	#ifdef DEBUG_ON 
		printf("[createPORV] CRETED NODE\n");
	#endif
	
	return porv;
}

struct PORV* addPORVToList(struct PORV* root, struct PORV* porv){
	#ifdef DEBUG_ON 
		printf("[addPORVToList] STARTED\n");
	#endif
	
	struct PORV* temp = root;
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
			printf("[addPORVToList] PORV LIST NOT EMPTY\n");
		#endif
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
			printf("[addPORVToList] ADDING TO PORV LIST\n");
		#endif
		temp->next = porv;
	} else {
		#ifdef DEBUG_ON 
			printf("[addPORVToList] PORV LIST EMPTY\n");
			printf("[addPORVToList] ADDING TO PORV LIST\n");
		#endif
		root = porv;
		#ifdef DEBUG_ON 
			printf("[addPORVToList] ROOT CHANGED\n");
		#endif
		
	}
	#ifdef DEBUG_ON 
		printf("[addPORVToList] ADDED PORV\n");
	#endif
	return root;
}

struct PORV* duplicatePORVList(struct PORV* root){
	#ifdef DEBUG_ON 
		printf("[duplicatePORVList] STARTED\n");
	#endif
	
	struct PORV* temp = root;
	struct PORV* newRoot = NULL;
	#ifdef DEBUG_ON 
		printf("[root = %p] %p \n",root,root->next);
	#endif
	
	while(temp!=NULL){
		struct PORV* newPORV = (struct PORV*)malloc(sizeof(struct PORV));
		newPORV->id = temp->id;
		newPORV->next = NULL;
		newPORV->porv = duplicateConditionList(temp->porv);
		
		newRoot = addPORVToList (newRoot, newPORV);
		#ifdef DEBUG_ON 
			printf("[duplicatePORVList] DUPLICATING NEXT PORV [%p]\n",temp->next);
			printf("[root = %p]\n",root);
		//if(temp == temp->next) temp->next=NULL;//exit(0);
		#endif
		
		temp = temp->next;
	}
	#ifdef DEBUG_ON 
		printf("[duplicatePORVList] COMPLETED\n");
	#endif
	
	return newRoot;
}
/*-----------------------PORVExpression--------------------*/
struct PORVExpression* createPORVExpression(struct PORV* conjunct){
	#ifdef DEBUG_ON 
	printf("[createPORVExpression] STARTED\n");
	#endif
	struct PORVExpression* porvExpr = (struct PORVExpression*)malloc(sizeof(struct PORVExpression));
	porvExpr->firstMatch=0;
	porvExpr->conjunct = conjunct;
	porvExpr->next = NULL;
	#ifdef DEBUG_ON 
	printf("[createPORVExpression] CRETED NODE\n");
	#endif
	
	return porvExpr;
}

struct PORVExpression* addPORVExpressionToEOfList(struct PORVExpression* root, struct PORVExpression* porvExpr){
	#ifdef DEBUG_ON 
	printf("[addPORVExpressionToList] STARTED\n");
	#endif
	
	struct PORVExpression* temp = root;
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[addPORVExpressionToList] PORV LIST NOT EMPTY\n");
		#endif
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
		printf("[addPORVExpressionToList] ADDING TO PORV LIST\n");
		#endif
		temp->next = porvExpr;
	} else {
		#ifdef DEBUG_ON 
		printf("[addPORVExpressionToList] PORV LIST EMPTY\n");
		printf("[addPORVExpressionToList] ADDING TO PORV LIST\n");
		#endif
		root = porvExpr;
		#ifdef DEBUG_ON 
		printf("[addPORVExpressionToList] ROOT CHANGED\n");
		#endif
		
	}
	#ifdef DEBUG_ON 
	printf("[addPORVExpressionToList] ADDED PORV\n");
	#endif
	return root;
}

/*-----------------------Expression--------------------*/
struct expression* createExpression(struct PORVExpression* porvExpr, struct eventType* event){
	#ifdef DEBUG_ON 
	printf("[createExpression] STARTED\n");
	#endif
	struct expression* expr = (struct expression*)malloc(sizeof(struct expression));
	
	expr->dnf = porvExpr;
	expr->event = event;
	#ifdef DEBUG_ON 
	printf("[createExpression] CRETED NODE\n");
	#endif
	
	return expr;
}

/*-------------------SequenceExpression----------------*/
/*
 s *truct sequenceExpr{	
	 struct expression* expr;	//Sub-sequence Expression
	 struct timeDelay* delay;	//Delay associated with expr match
	 struct condition* assignments;	//List of Assignments when expr matches within delay
	 struct sequenceExpr* next;	//Next sub-expression
 };
 */

struct sequenceExpr* createSequenceExpr(struct expression* expr, struct timeDelay* delay, struct condition* assignments){
	#ifdef DEBUG_ON 
	printf("[createSequenceExpr] STARTED\n");
	#endif
	struct sequenceExpr* seqExpr = (struct sequenceExpr*)malloc(sizeof(struct sequenceExpr));
	
	seqExpr->expr = expr;
	seqExpr->delay = delay;
	seqExpr->assignments = assignments;
	seqExpr->next = NULL;
	#ifdef DEBUG_ON 
	printf("[createSequenceExpr] CRETED NODE\n");
	#endif
	
	return seqExpr;
}


struct sequenceExpr* addSequenceExprToEOfList(struct sequenceExpr* root, struct sequenceExpr* seqExpr){
	#ifdef DEBUG_ON 
	printf("[addSequenceExprToEOfList] STARTED\n");
	
	printf("seqExpr->delay = %p\n",seqExpr->delay);
	
	if(seqExpr->delay!=NULL){
		printf("LOWER = %f, UPPER = %f\n",seqExpr->delay->lower,seqExpr->delay->upper);
	}
	#endif
	struct sequenceExpr* temp = root;
	
	if(root!=NULL){
		#ifdef DEBUG_ON 
		printf("[addSequenceExprToEOfList] PORV LIST NOT EMPTY\n");
		#endif
		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON 
		printf("[addSequenceExprToEOfList] ADDING TO PORV LIST\n");
		#endif
		temp->next = seqExpr;
	} else {
		#ifdef DEBUG_ON 
		printf("[addSequenceExprToEOfList] PORV LIST EMPTY\n");
		printf("[addSequenceExprToEOfList] ADDING TO PORV LIST\n");
		#endif
		root = seqExpr;
		#ifdef DEBUG_ON 
		printf("[addSequenceExprToEOfList] ROOT CHANGED\n");
		#endif
		
	}
	#ifdef DEBUG_ON 
	printf("[addSequenceExprToEOfList] ADDED PORV\n");
	#endif
	return root;
}


/*-------------------TimeDelay----------------*/
// struct timeDelay{
// 	float lower;			//Delay Lower Bound
// 	float upper;			//Delay Upper Bound
// 	int type;			//Eventually (1) or Finite Bounds (0) or Fixed(2)
// };	

struct timeDelay* createTimeDelay(double lower, double upper, int type){
	#ifdef DEBUG_ON 
	printf("[createTimeDelay] STARTED\n");
	#endif
	struct timeDelay* delay = (struct timeDelay*)malloc(sizeof(struct timeDelay));
	
	delay->lower = (float)lower;
	delay->upper = (float)upper;
	delay->type = type;
	#ifdef DEBUG_ON 
	printf("[createTimeDelay] CRETED NODE [%f %f %d]\n",delay->lower,delay->upper,delay->type);
	#endif
	
	return delay;
}


/*-------------------FeatureDefinition----------------*/
/*
 s *truct featureDef{
	 struct identifier* localVars;	//List of Feature Local Variables
	 struct sequenceExpr* seq;	//Sequence Expression - Behaviour to match
	 struct condition* featureAssign;//Feature Computation assignments
 };
 
 struct feature{
	 char name[MAX_STR_LENGTH];			//Feature Name
	 struct identifier* params;	//Feature Parameters
	 struct featureDef* def;		//Feature Definition (Behaviour and Feature Computation)
 };*/

struct featureDef* createFeatureDef(struct identifier* localVars, struct sequenceExpr* seq, struct condition* featureAssign){
	#ifdef DEBUG_ON 
	printf("[createFeatureDef] STARTED\n");
	#endif
	struct featureDef* featureStmt = (struct featureDef*)malloc(sizeof(struct featureDef));
	
	featureStmt->localVars = localVars;
	featureStmt->seq = seq;
	featureStmt->featureAssign = featureAssign;
	#ifdef DEBUG_ON 
	printf("[createFeatureDef] CRETED NODE\n");
	#endif
	
	return featureStmt;
}

/*-------------------Feature----------------*/
/*
 * 
 * struct feature{
 * char name[MAX_STR_LENGTH];			//Feature Name
 * struct identifier* params;	//Feature Parameters
 * struct featureDef* def;		//Feature Definition (Behaviour and Feature Computation)
 };
 */

struct feature* createFeature(char *name, struct identifier* params, struct featureDef* def){
	#ifdef DEBUG_ON 
	printf("[createFeature] STARTED\n");
	#endif
	struct feature* featureStmt = (struct feature*)malloc(sizeof(struct feature));
	
	strcpy(featureStmt->name,name);
	featureStmt->params = params;
	featureStmt->def = def;
	#ifdef DEBUG_ON 
	printf("[createFeature] CRETED NODE\n");
	#endif
	
	return featureStmt;
}

/*--------------------------- Feature Support Methods ------------------------ */

void printPORVDetails(struct feature* featureStmt){
	struct sequenceExpr* seq = featureStmt->def->seq;
	printf("----- ID ----- | ----- PORV -----\n               |                 \n");
	while(seq!=NULL){
		if(seq->expr->event!=NULL){
			if(seq->expr->event->porv!=NULL){
				printf("      %d       | %s %s %s\n",seq->expr->event->porv->id,seq->expr->event->porv->porv->LHS,featureOperatorMap(seq->expr->event->porv->porv->op),seq->expr->event->porv->porv->RHS);
			}
		}
		struct PORVExpression* expr = seq->expr->dnf;
		while(expr!=NULL){
			struct PORV* conjunct = expr->conjunct;
			while(conjunct!=NULL){
				printf("      %d       | %s %s %s\n",conjunct->id,conjunct->porv->LHS,featureOperatorMap(conjunct->porv->op),conjunct->porv->RHS);
				conjunct=conjunct->next;
			}
			expr = expr->next;
		}
		seq = seq->next;
	}
	
}

char* featureOperatorMap(int op){	
	char *p=(char *)malloc(sizeof(char)*3);
	switch(op){
		case 0: sprintf(p,"==");break;
		case 1: sprintf(p,"<=");break;
		case 2: sprintf(p,"=");break;
		case 3: sprintf(p,">=");break;
		case 4: sprintf(p,"<");break;
		case 5: sprintf(p,">");break;
		default:sprintf(p,"==");break;
		
	}
	//printf("%s",p);
	return p;
}

void printFeatureCondition(struct condition* cond){
	if(cond!=NULL){
		printf(" %s %s %s ",cond->LHS,featureOperatorMap(cond->op),cond->RHS);	
	}
	/*if(cond->next!=NULL){
		printf("CAUGHT IT\n");exit(0);
	}*/
}

void printAssignmentList(struct condition* assignments){
	if(assignments!=NULL){
		while(assignments->next!=NULL){
			printFeatureCondition(assignments);
			printf(", ");
			assignments = assignments->next;
		}
		printFeatureCondition(assignments);
	}
}


void printPORV(struct PORV* porv){
	if(porv!=NULL){
		//printf("%d ",porv->id);
		//printFeatureCondition(porv->porv);
		printf(" %s %s %s ",porv->porv->LHS,featureOperatorMap(porv->porv->op),porv->porv->RHS);	
	}
}


void printPORVConjunct(struct PORV* conjunct){
	if(conjunct!=NULL){
		while(conjunct->next!=NULL){
			printPORV(conjunct);
			printf("&&");
			conjunct = conjunct->next;
		}
		printPORV(conjunct);
	}
}

void printEvent(struct eventType* event){
	if(event!=NULL){
		printf(" @");
		switch(event->type){
			case 1: printf("+");break;
			case 2: printf("-");break;
			default:break;
		}
		printf("(");
		printPORV(event->porv);
		printf(") ");
	}
}

void printPORVExpression(struct PORVExpression* porvExpr){
	if(porvExpr!=NULL){
		while(porvExpr->next!=NULL){
			printPORVConjunct(porvExpr->conjunct);
			printf("||");
			porvExpr = porvExpr->next;
		}
		printPORVConjunct(porvExpr->conjunct);
	}	
}

void printExpression(struct expression* expr){
	if(expr!=NULL){
		if(expr->dnf!=NULL){
			printf("(");
			printPORVExpression(expr->dnf);
			printf(")");
			if(expr->event!=NULL){
				printf("&&");
			}
		}
		printEvent(expr->event);
	}
}

void printTimeDelay(struct timeDelay* delay){
	#ifdef DEBUG_ON
		printf("[printTimeDelay] START\n");//delay=NULL;printf("delay = [%s]",delay);
	#endif
	//printf("delay = [%f:%f]",delay->lower,delay->upper);
	if(delay!=NULL){
		printf(" ##");//printf("%d",delay->type);printf("HELLO\n");
		switch(delay->type){
			case 0: printf("[%f:%f] ",delay->lower, delay->upper);
			break;
			case 1: printf("[%f:$] ",delay->lower);
			break;
			case 2: printf("%f ",delay->lower);
			break;
			default:printf("\n--------------UNKNOWN DELAY TYPE-------------\n");
			break;
		}
	}
	
	#ifdef DEBUG_ON
	printf("[printTimeDelay] END\n");
	#endif
}

void printSequenceExpr(struct sequenceExpr* seq){
	printf("\t");
	if(seq!=NULL){
		while(seq!=NULL){
			//Print Temporal Seperation
			printTimeDelay(seq->delay);
			
			//Print Sub-sequence Expression
			printExpression(seq->expr);
			
			if(seq->assignments!=NULL){
				printf(", ");
				printAssignmentList(seq->assignments);
			}
			
			seq = seq->next;
		}
	} else {
		printf("\n--------------NO SEQUENCE EXPRESSION-------------\n");
	}
}

void printFeatureDefinition(struct featureDef* def){
	printf("begin\n");
	
	//Print Local Variables
	if(def->localVars!=NULL){
		printf("\tvar ");
		struct identifier* var = def->localVars;
		while(var->next!=NULL){
			printf("%s , ",var->name);
			var = var->next;
		}
		printf("%s ;\n",var->name);
	}
	
	//Print Sequence Expression
	if(def->seq!=NULL){
		printSequenceExpr(def->seq);
		
	} else {
		printf("\n--------------NO SEQUENCE EXPRESSION-------------\n");
	}
	
	printf(" |-> ");
	
	//Printf Feature Computation
	printAssignmentList(def->featureAssign);
	
	printf(";\n");
	
	printf("end\n");
}

void printFeature(struct feature* featureStmt){
	printf("\n\n------------------------------FEATURE--------------------------------\n\n");
	if(featureStmt != NULL){
		if(featureStmt->name!=NULL){
			printf("feature %s ",featureStmt->name);
			
			// Print Parameters
			if(featureStmt->params!=NULL){
				printf("( ");
				struct identifier* param=featureStmt->params;
				while(param->next!=NULL){
					printf("%s , ",param->name);
					param = param->next;
				}
				printf("%s ) ;",param->name);
			}
			printf("\n");
			//Print Feature Body
			if(featureStmt->def!=NULL){
				printFeatureDefinition(featureStmt->def);
				
			} else {
				printf("\n--------------NO FEATURE BODY-------------\n");
			}
			
			
		} else {
			printf("Feature Name is NULL\n");
		}
	}
	printf("\n\n---------------------------------------------------------------------\n\n");
}

int requestFeatureParamChoice(int count){
	int id=0;
	
	do{
		if(id!=0){
			printf("Incorrect parameter ID entered.\n\n");
		}
		//printf("\n\n--------------------------------------------------------------------\n");
		printf("Enter the ID of the parameter you wish to edit.\n");
		printf("If you are satisfied with the value list enter 0.\n");
		printf("Your choice: ");
		scanf("%d",&id);
	} while(!(id>=0 & id<=count));
	printf("\n--------------------------------------------------------------------------\n");
	return id;
}

int printFeatureParamList(struct identifier* params){
	if(params){
		int id = 1;
		while(params){
			printf("%d. %s\n",id++,params->name);
			params = params->next;
		}
		id--;
		return id;
	}
	return 0;
}

void updateFeatureParamList(double** paramValues, int id){
	if(paramValues[id-1]!=NULL){
		printf("Present Parameter Value: ");
	} else {
		paramValues[id-1] = (double*)malloc(sizeof(double));
	}
	
	printf("Suggested Value: ");
	scanf("%lf",paramValues[id-1]);
	
	if(runFile) fprintf(runFile,"%lf\n",*paramValues[id-1]);
	printf("Updated Parameter Value: %lf\n",*paramValues[id-1]); 
}

void getParamValueFromIO(struct feature* featureStmt){
	if(featureStmt){
		printFeature(featureStmt);
		
		if(featureStmt->params){
			printf("Update feature parameters:\n");
			int count = countIdentifiers(featureStmt->params);
			int param_id = 0;
			int i=0; int flag = 0;
			double* paramValues[count];
			bzero(paramValues,sizeof(double*)*count);
			struct identifier* params = NULL;
			do{	
				params = featureStmt->params;
				for(i=0;i<count;i++){
					if(paramValues[i]!=NULL){
						if(flag==0){
							printf("Parameters set:\n");
							flag = 1;
						}
						printf("%s = %lf\n",params->name,*paramValues[i]);
					}
					params = params->next;
				}
			
				param_id = requestFeatureParamChoice( printFeatureParamList(featureStmt->params) );
				if(runFile) fprintf(runFile,"%d\n",param_id);
				if(param_id == 0){
					for(i=0;i<count;i++){
						if(paramValues[i]==NULL){
							flag = 0;
						}
					}
					if(flag == 1)
						break;
					else {
						printf("Not all parameter values are set. Please ensure that all parameters are initialized.\n");
					}
				} else {
					updateFeatureParamList(paramValues,param_id);
				}
			} while(1);
			updateFeatureParams(featureStmt,paramValues,count);
			printFeature(featureStmt);
		} else {
			printf("No feature parameters. Proceeding.\n");
		}
	}
}

char* replaceStrWithStr(
    char const * const original, 
    char const * const pattern, 
    char const * const replacement
) {
	size_t const replen = strlen(replacement);
	size_t const patlen = strlen(pattern);
	size_t const orilen = strlen(original);

	size_t patcnt = 0;
	const char * oriptr;
	const char * patloc;

	// find how many times the pattern occurs in the original string
	for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen){
		patcnt++;
	}

	// allocate memory for the new string
	size_t const retlen = orilen + patcnt * (replen - patlen);
	if(retlen>MAX_STR_LENGTH){
		printf("ERROR: Parameter cannot be replaced in the feature definition\n");
		exit(0);
	}
	
	char * const returned = (char *) malloc( sizeof(char) * (retlen + 1) );
	
	// copy the original string, 
	// replacing all the instances of the pattern
	char* retptr = returned;
	for (oriptr = original; patloc = strstr(oriptr, pattern); oriptr = patloc + patlen)
	{
		size_t const skplen = patloc - oriptr;
		// copy the section until the occurence of the pattern
		strncpy(retptr, oriptr, skplen);
		retptr += skplen;
		// copy the replacement 
		strncpy(retptr, replacement, replen);
		retptr += replen;
	}
	// copy the rest of the string.
	strcpy(retptr, oriptr);
	
	return returned;
}

void updateStringParam(char* original, char* substr, double value){
	if(original && substr){
		char replacementString[MAX_STR_LENGTH];
		sprintf(replacementString,"%lf",value);
		
		char* updatedString = replaceStrWithStr(original,substr,replacementString);
		strcpy(original,updatedString);
		//free(updatedString);
	}
}

void updatePORVParam(struct PORV* porvStruct, char* param, double value){
	if(porvStruct){
		while(porvStruct){
			struct condition* cond = porvStruct->porv;
			updateStringParam(cond->LHS,param,value);
			updateStringParam(cond->RHS,param,value);
			porvStruct = porvStruct->next;
		}
	}
}

void updateFeatureParams(struct feature* featureStmt, double** values, int count){
	if(count>0 && featureStmt && values){
		//Assuming values are all NON-NULL
		struct sequenceExpr* seq = featureStmt->def->seq;
		while(seq){
			int i=0;
			struct identifier* params = featureStmt->params;
			while(params){
				char* paramName = params->name;
				
				//Update Event
				updateStringParam(seq->expr->event->porv->porv->LHS,paramName,*values[i]);
				updateStringParam(seq->expr->event->porv->porv->RHS,paramName,*values[i]);
				
				//Update DNF
				struct PORVExpression* dnf = seq->expr->dnf;
				while(dnf){
					updatePORVParam(dnf->conjunct,paramName,*values[i]);
					dnf=dnf->next;
				}
				i++;
				params = params->next;
			}
			
			seq = seq->next;
		}
	}
}

char* phOperatorMap(int op){	
	char *p=(char *)malloc(sizeof(char)*3);
	switch(op){
		case 0: sprintf(p,"==");break;
		case 1: sprintf(p,"<=");break;
		case 2: sprintf(p,":=");break;
		case 3: sprintf(p,">=");break;
		case 4: sprintf(p,"<");break;
		case 5: sprintf(p,">");break;
		default:sprintf(p,"==");break;
		
	}
	//printf("%s",p);
	return p;
}




void displayAutomaton(struct phaver* pha/*, struct feature* sysFeature*/){
	struct condition* params=pha->params;
	struct automaton* ha=pha->ha;
	struct identifier* contr_vars=ha->contr_vars;
	struct identifier* synclabs=ha->synclabs;
	struct identifier* var_inputs=ha->var_inputs;
	struct location* loc=ha->locations;
	struct condition* invariant=NULL;
	struct condition* derivative=NULL;
	struct transition* transitions=NULL;
	struct condition* when=NULL;
	struct condition* reset=NULL;
	struct transitionWrapper* incoming = NULL;
	struct initial* init = pha->init;
	struct identifier* init_locs=pha->init->init_locs;
	struct condition* init_conds=pha->init->init_conds;
	
	printf("//--------------PARAMETERS------------------\n");
	while(params!=NULL){
		printf("%s %s %s;\n",params->LHS,":=",params->RHS);
		params=params->next;
	}
	
	printf("//--------------AUTOMATON-------------------\n");
	printf("automaton %s\n",ha->name);
	printf("contr_var: ");
	while(contr_vars!=NULL){
		if(contr_vars->next == NULL){
			printf("%s;\n",contr_vars->name);
		} else printf("%s, ",contr_vars->name);
		contr_vars=contr_vars->next;
	}
	printf("synclabs: ");
	while(synclabs!=NULL){
		if(synclabs->next == NULL){
			printf("%s;\n",synclabs->name);
		} else printf("%s,",synclabs->name);
		synclabs=synclabs->next;
	}
	if(var_inputs!=NULL) printf("var_inputs: ");
	while(var_inputs!=NULL){
		if(var_inputs->next == NULL){
			printf("%s;\n",var_inputs->name);
		} else printf("%s, ",var_inputs->name);
		var_inputs=var_inputs->next;
	}
	printf("//--------------LOCATIONS-------------------\n");
	while(loc!=NULL){
		//printf("//-----------------THIS LOCATION %s VAR_INPUTS--------------\n",loc->requires==0?"DOES NOT REQUIRE":"REQUIRES");
		printf("\nloc %s:\t\t",loc->name);
		printf("while ");
		invariant=loc->invariant;
		while(invariant!=NULL){
			if(invariant->op == -1){
				printf("true");
			} else {
				printf("%s %s %s",invariant->LHS,phOperatorMap(invariant->op),invariant->RHS);
			}
			//printf("%s %s %s",invariant->LHS,phOperatorMap(invariant->op),invariant->RHS);
			if(invariant->next != NULL){
				printf(" & ");
			} else {
				printf("\n\t\t\t");
			}
			invariant=invariant->next;
		}
		printf("\twait {");
			derivative=loc->derivative;
			while(derivative!=NULL){
				printf("%s %s %s",derivative->LHS,phOperatorMap(derivative->op),derivative->RHS);
				//printf("%s == %s",derivative->LHS,derivative->RHS);
				if(derivative->next != NULL){
					printf(" & ");
				} else {
					printf(" };\n\t\t\t");
				}
				derivative=derivative->next;
			}
			
			
			//OUTGOING TRANSITIONS
			transitions = loc->transitions;
			while(transitions!=NULL){
				when = transitions->when;
				reset= transitions->reset;
				
				printf("\n\t\t\twhen ");
				
				while(when!=NULL){
					if(strcasecmp(when->LHS,"true")==0)
						printf("true");
					else 
						printf("%s %s %s",when->LHS,phOperatorMap(when->op),when->RHS);
					
					if(when->next != NULL){
						printf(" & ");
					} else {
						printf("\n\t\t\t");
					}
					when=when->next;
				}
				printf("sync %s do { ",transitions->sync);
				//int flag = 0; char timeLabel[5]; sprintf(timeLabel,"time");	
				while(reset!=NULL){
					/*
					if(flag == 0 && strcmp(reset->RHS,timeLabel)==0)
						flag = 1;
					if (flag==1 && strcmp(reset->RHS,timeLabel)==0) 
						printf("Houston we have a problem in the list\n");
					*/
					printf("%s %s %s",reset->LHS,"==",reset->RHS);
					if(reset->next != NULL){
						printf(" & ");
					} else {
						printf(" }\n\t\t\t");
					}
					reset=reset->next;
				}
				printf("goto %s;\n",transitions->gotoLoc);
				//printf("\n");
				transitions=transitions->next;
			}
			/*
			//INCOMING TRANSITIONS
			incoming = loc->incoming;
			//printf("********************** INCOMING TRANSITIONS *********************");
			while(incoming!=NULL){
				transitions = incoming->trans;
				when = transitions->when;
				reset= transitions->reset;
				printf("\n\t\tFROM : %s",incoming->loc->name);
				printf("\n\t\t\twhen ");
				while(when!=NULL){
					printf("%s %s %s",when->LHS,phOperatorMap(when->op),when->RHS);
					if(when->next != NULL){
						printf(" & ");
					} else {
						printf("\n\t\t\t");
					}
					when=when->next;
				}
				printf("sync %s do { ",transitions->sync);
					
					while(reset!=NULL){
						printf("%s %s %s",reset->LHS,"==",reset->RHS);
						if(reset->next != NULL){
							printf(" & ");
						} else {
							printf(" }\n\t\t\t");
						}
						reset=reset->next;
					}
					printf("goto %s;\n",transitions->gotoLoc);
					printf("\n");
					incoming=incoming->next;
			}*/
			//printf("*****************************************************************\n");
			//printf("\tPORV INVARIANTS"); printPORVConjunct(loc->labels);printf("\n");
			/*
			if(sysFeature!=NULL){
				char* T = "TRUE";
				char* F = "FALSE";
				
				struct sequenceExpr* seq = sysFeature->def->seq;
				printf("----- ID ----- | ----- PORV -----\n               |                 \n");
				
				while(seq!=NULL){
					if(seq->expr->event!=NULL){
						if(seq->expr->event->porv!=NULL){
							printf("      %d       | %s %s %s | %s\n",seq->expr->event->porv->id,seq->expr->event->porv->porv->LHS,featureOperatorMap(seq->expr->event->porv->porv->op),seq->expr->event->porv->porv->RHS,isTruePORV(loc,seq->expr->event->porv)?T:F);
						}
					}
					struct PORVExpression* expr = seq->expr->dnf;
					while(expr!=NULL){
						struct PORV* conjunct = expr->conjunct;
						while(conjunct!=NULL){
							printf("      %d       | %s %s %s | %s\n",conjunct->id,conjunct->porv->LHS,featureOperatorMap(conjunct->porv->op),conjunct->porv->RHS,isTruePORV(loc,conjunct)?T:F);
							conjunct=conjunct->next;
						}
						expr = expr->next;
					}
					seq = seq->next;
				}
			}*/
			/*
			if(sysFeature!=NULL){
				char* T = "TRUE";
				char* F = "FALSE";
				
				struct sequenceExpr* seq = sysFeature->def->seq;
				//printf("----- ID ----- | ----- PORV -----\n               |                 \n");
				
				while(seq!=NULL){
					int flag = 0;
					if(seq->expr->event!=NULL){
						if(seq->expr->event->porv!=NULL){
							flag = isTruePORV(loc,seq->expr->event->porv) ? 1 : 0 ;
							printf("\t\t");printEvent(seq->expr->event);
						}
					}
					struct PORVExpression* expr = seq->expr->dnf;
					while(expr!=NULL){
						printf("\n\t\t\t");printPORVConjunct(expr->conjunct);
						printf("%s\n",isTrueConjunct(loc,expr->conjunct) && flag == 1?T:F);
						expr = expr->next;
					}
					seq = seq->next;
				}
			}*/
			/*if(sysFeature!=NULL){
				char* T = "TRUE";
				char* F = "FALSE";
				
				struct sequenceExpr* seq = sysFeature->def->seq;
				//printf("----- ID ----- | ----- PORV -----\n               |                 \n");
				
				while(seq!=NULL){
					printf("\t\t\t");printExpression(seq->expr);
					printf("\n\t\t\t\t\t\t");
					printf("%s\n",isTrueExpr(loc,seq->expr)==1?T:F);					
					seq = seq->next;
				}
			}
			*/
			loc=loc->next;
			
	}
	//struct identifier* init_locs=pha->init_locs;
	//struct condition* init_conds=pha->init_conds;
	printf("\ninitially: ");
	while(init!=NULL){
		init_locs = init->init_locs;
		init_conds = init->init_conds;
		
		while(init_locs!=NULL){
			printf("%s",init_locs->name);
			if(init_locs->next != NULL){
				printf(" & ");
			} else {
				if(init_conds != NULL)
					printf(" & ");
			}
			init_locs=init_locs->next;
		}
		while(init_conds!=NULL){
			printf("%s %s %s",init_conds->LHS,phOperatorMap(init_conds->op),init_conds->RHS);
			if(init_conds->next != NULL){
				printf(" & ");
			} 
			init_conds=init_conds->next;
		}
		if(init->next!=NULL){
			printf(", ");
		} else printf(";\n");
		init = init->next;
	}
	
	printf("\nend\n");
}


void writeAutomaton(struct phaver* pha, FILE *out){
	struct condition* params=pha->params;
	struct automaton* ha=pha->ha;
	struct identifier* contr_vars=ha->contr_vars;
	struct identifier* synclabs=ha->synclabs;
	struct identifier* var_inputs=ha->var_inputs;
	struct location* loc=ha->locations;
	struct condition* invariant=NULL;
	struct condition* derivative=NULL;
	struct transition* transitions=NULL;
	struct condition* when=NULL;
	struct condition* reset=NULL;
	struct transitionWrapper* incoming = NULL;
	struct initial* init = pha->init;
	struct identifier* init_locs=pha->init->init_locs;
	struct condition* init_conds=pha->init->init_conds;
	
	fprintf(out,"//--------------PARAMETERS------------------\n");
	while(params!=NULL){
		fprintf(out,"%s %s %s;\n",params->LHS,":=",params->RHS);
		params=params->next;
	}
	
	fprintf(out,"//--------------AUTOMATON-------------------\n");
	fprintf(out,"automaton %s\n",ha->name);
	fprintf(out,"contr_var: "); //fprintf(out,"state_var: ");
	while(contr_vars!=NULL){
		if(contr_vars->next == NULL){
			fprintf(out,"%s;\n",contr_vars->name);
		} else fprintf(out,"%s, ",contr_vars->name);
		contr_vars=contr_vars->next;
	}
	fprintf(out,"synclabs: ");
	while(synclabs!=NULL){
		if(synclabs->next == NULL){
			fprintf(out,"%s;\n",synclabs->name);
		} else fprintf(out,"%s,",synclabs->name);
		synclabs=synclabs->next;
	}
	if(var_inputs!=NULL) fprintf(out,"var_inputs: ");
	while(var_inputs!=NULL){
		if(var_inputs->next == NULL){
			fprintf(out,"%s;\n",var_inputs->name);
		} else fprintf(out,"%s, ",var_inputs->name);
		var_inputs=var_inputs->next;
	}
	fprintf(out,"//--------------LOCATIONS-------------------\n");
	while(loc!=NULL){
		//fprintf(out,"//-----------------THIS LOCATION %s VAR_INPUTS--------------\n",loc->requires==0?"DOES NOT REQUIRE":"REQUIRES");
		fprintf(out,"\nloc %s:\t\t",loc->name);
		fprintf(out,"while ");
		invariant=loc->invariant;
		while(invariant!=NULL){
			if(invariant->op == -1){
				fprintf(out,"true");
			} else {
				fprintf(out,"%s %s %s",invariant->LHS,phOperatorMap(invariant->op),invariant->RHS);
			}
			
			if(invariant->next != NULL){
				fprintf(out," & ");
			} else {
				fprintf(out,"\n\t\t\t");
			}
			invariant=invariant->next;
		}
		fprintf(out,"\twait {");
		derivative=loc->derivative;
		while(derivative!=NULL){
			fprintf(out,"%s %s %s",derivative->LHS,phOperatorMap(derivative->op),derivative->RHS);
			//fprintf(out,"%s == %s",derivative->LHS,derivative->RHS);
			if(derivative->next != NULL){
				fprintf(out," & ");
			} else {
				fprintf(out," };\n\t\t\t");
			}
			derivative=derivative->next;
		}


		//OUTGOING TRANSITIONS
		transitions = loc->transitions;
		while(transitions!=NULL){
			when = transitions->when;
			reset= transitions->reset;
			
			fprintf(out,"\n\t\t\twhen ");
			
			while(when!=NULL){
				fprintf(out,"%s %s %s",when->LHS,phOperatorMap(when->op),when->RHS);
				
				if(when->next != NULL){
					fprintf(out," & ");
				} else {
					fprintf(out,"\n\t\t\t");
				}
				when=when->next;
			}
			fprintf(out,"sync %s do { ",transitions->sync);
				//int flag = 0; char timeLabel[5]; sprintf(timeLabel,"time");	
			while(reset!=NULL){
			/*
			if(flag == 0 && strcmp(reset->RHS,timeLabel)==0)
			flag = 1;
			if (flag==1 && strcmp(reset->RHS,timeLabel)==0) 
			printf("Houston we have a problem in the list\n");
			*/
				fprintf(out,"%s %s %s",reset->LHS,"==",reset->RHS);
				if(reset->next != NULL){
					fprintf(out," & ");
				} else {
					fprintf(out," }\n\t\t\t");
				}
				reset=reset->next;
			}
			fprintf(out,"goto %s;\n",transitions->gotoLoc);
			//printf("\n");
			transitions=transitions->next;
		}
		loc=loc->next;
	}
	//struct identifier* init_locs=pha->init_locs;
	//struct condition* init_conds=pha->init_conds;
	fprintf(out,"\ninitially: ");
	while(init!=NULL){
		init_locs = init->init_locs;
		init_conds = init->init_conds;
		
		while(init_locs!=NULL){
			fprintf(out,"%s",init_locs->name);
			if(init_locs->next != NULL){
				fprintf(out," & ");
			} else {
				if(init_conds != NULL)
					fprintf(out," & ");
			}
			init_locs=init_locs->next;
		}
		while(init_conds!=NULL){
			fprintf(out,"%s %s %s",init_conds->LHS,phOperatorMap(init_conds->op),init_conds->RHS);
			if(init_conds->next != NULL){
				fprintf(out," & ");
			} 
			init_conds=init_conds->next;
		}
		if(init->next!=NULL){
			fprintf(out,", ");
		} else fprintf(out,";\n");
		init = init->next;
	}
	
	fprintf(out,"\nend\n");
}


 int writeSpaceEx(struct phaver* HA, char* fileName){
 	int finalID = 0;
	std::map<char*, int> locNametoIDMap;
 	struct condition* params=HA->params;
 	struct condition* params2=HA->params;
 	struct condition* params3=HA->params;
 	struct automaton* ha=HA->ha;
 	struct identifier* contr_vars=ha->contr_vars;
 	struct identifier* contr_vars2=ha->contr_vars;
 	struct identifier* contr_vars3=ha->contr_vars;
 	struct identifier* synclabs=ha->synclabs;
 	struct identifier* synclabs2=ha->synclabs;
 	struct identifier* synclabs3=ha->synclabs;
 	struct identifier* var_inputs=ha->var_inputs;
 	struct location* loc = ha->locations;
 	struct location* loc2 = ha->locations;
 	struct condition* invariant=NULL;
 	struct condition* derivative=NULL;
 	struct transition* transitions=NULL;
 	struct condition* when=NULL;
 	struct condition* resets=NULL;
 	struct transitionWrapper* incoming = NULL;
 	struct initial* init = HA->init;
 	struct identifier* init_locs=HA->init->init_locs;
 	struct condition* init_conds=HA->init->init_conds;
 	struct condition* init_conds2=HA->init->init_conds;
 	struct condition* init_conds3;	
 	struct locationSpaceEx* locSp = (struct locationSpaceEx*)malloc(sizeof(struct locationSpaceEx));
 	struct transitionSpaceEx* transSp = (struct transitionSpaceEx*)malloc(sizeof(struct transitionSpaceEx));
 	struct transitionSpaceEx *transSpPtr, *transtemp;
 	struct locationSpaceEx *locSpPtr, *loctemp;
 	char targetName[MAX_STR_LENGTH];
 	char sourceName[MAX_STR_LENGTH];
 	FILE *fptr;
 	char st[MAX_STR_LENGTH],PHAVerFile[MAX_STR_LENGTH]; //filename[MAX_STR_LENGTH],
 	//strcpy(PHAVerFile, argv[1]);
 	//strncpy(st, PHAVerFile, strlen(PHAVerFile)-4);
 	//printf("St : %s\n", PHAVerFile);
 	//sprintf(filename, "spaceexModel.xml");
 	//sprintf(filename,"%s_%s.xml",argv[1],argv[2]);
 	
 	//printf("filename : %s\n",filename);
 	fptr = fopen(fileName, "w");
 
 	int i=0, l=0;
 	fprintf(fptr, "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n<sspaceex xmlns=\"http://www-verimag.imag.fr/xml-namespaces/sspaceex\" version=\"0.2\" math=\"SpaceEx\">\n\t");
 	//there will only be one component
 	fprintf(fptr, "<component id=\"%s_template\">\n", ha->name);
 
 	while(contr_vars!=NULL)
 	{
 		fprintf(fptr, "\t\t<param name=\"%s\" type=\"real\" dynamics=\"any\" local=\"false\" d1=\"1\" d2=\"1\" />\n",contr_vars->name);
 		#ifdef DEBUG_ON 
 			printf("\t\t<param name=\"%s\" type=\"real\" dynamics=\"any\" local=\"false\" d1=\"1\" d2=\"1\" />\n",contr_vars->name);
 		#endif
 		
 		contr_vars = contr_vars->next;
 	}
 	
 	while(params!=NULL)
 	{
 		fprintf(fptr, "\t\t<param name=\"%s\" type=\"real\" dynamics=\"const\" local=\"false\" d1=\"1\" d2=\"1\" />\n",params->LHS);
 		#ifdef DEBUG_ON 
 			printf("\t\t<param name=\"%s\" type=\"real\" dynamics=\"const\" local=\"false\" d1=\"1\" d2=\"1\" />\n",params->LHS);
 		#endif
 		params = params->next;
 	}
 	
 	while(synclabs!=NULL)
 	{
 		fprintf(fptr, "\t\t<param name=\"%s\" type=\"label\" local=\"false\" />", synclabs->name);
 		#ifdef DEBUG_ON 
 			printf("\t\t<param name=\"%s\" type=\"label\" local=\"false\" />", synclabs->name);
 		#endif
 		synclabs = synclabs->next;
 
 	}
 	//PHAVer locations to SpaceEx locations
 	#ifdef DEBUG_ON 
 		printf("\nhere\n");
 	#endif
 	fprintf(fptr, "\n");
 	locSp->id = 0;
 	transSp->sourceID = 0;
 	transSp->targetID = 0;
 	while(l<5)
 	{
 		locSp->invariants[l] = NULL;
 		locSp->flows[l] = NULL;
 		bzero(transSp->label,sizeof(char)*MAX_STR_LENGTH);
 		transSp->guard[l] = NULL;
 		transSp->assignments[l] = NULL;
 		l++;
 	}
 	locSp->next = NULL;
 	transSp->next = NULL;
 	while(loc!=NULL)
 	{
 
 		i++;
 		
 		while(locSp->next!=NULL)
 		{
 			locSp = locSp->next;
 		}
 		loctemp = (struct locationSpaceEx*)malloc(sizeof(struct locationSpaceEx));
 
 		loctemp->id = i;
 		strcpy(loctemp->name, loc->name);
 
 		fprintf(fptr, "\t\t<location id=\"%d\" name=\"%s\">\n", loctemp->id, loctemp->name);
 		
 		if(strcasecmp(loctemp->name,"final")==0){
			finalID = loctemp->id;
		}
		
 		#ifdef DEBUG_ON 
 		printf("\t\t<location id=\"%d\" name=\"%s\">\n", loctemp->id, loctemp->name);
 		#endif
 		
 		locNametoIDMap[loctemp->name] = loctemp->id;
 		int j=0;
 		invariant = loc->invariant;
 		while(invariant!=NULL)
 		{
 			loctemp->invariants[j] = invariant;
 			invariant = invariant->next;
 			j++;
 		}
 		int k=0;
 		fprintf(fptr, "\t\t\t<invariant>");
 		
 		#ifdef DEBUG_ON 
 		printf("\t\t\t<invariant>");
 		#endif
 		
 		while(loctemp->invariants!=NULL)
 		{
 			if(k>=j) break;
 			/*if(strcmp(SpaceExOpMap(loctemp->invariants[k]->op),"==")==0)
 			{
 				fprintf(fptr, "true");
 				printf("true");
 				loctemp->invariants[k]->op = -99;
 				
 			}*/
 			if(loctemp->invariants[k]->op==-1){
 				fprintf(fptr, "true");
 				
 				#ifdef DEBUG_ON 
 				printf("true");
 				#endif
 				
 				
 			} else {
 				fprintf(fptr, "%s%s%s", loctemp->invariants[k]->LHS, SpaceExOpMap(loctemp->invariants[k]->op), loctemp->invariants[k]->RHS);
 				
 				#ifdef DEBUG_ON 
 				printf("%s%s%s", loctemp->invariants[k]->LHS, SpaceExOpMap(loctemp->invariants[k]->op), loctemp->invariants[k]->RHS);
 				#endif
 				
 				
 			}
 			
 			if(loctemp->invariants[k]->next == NULL)
 			{
 				fprintf(fptr, "</invariant>\n");
 				#ifdef DEBUG_ON 
 				printf("</invariant>\n");
 				#endif
 			}
 			else 
 			{
 				fprintf(fptr, " &amp; ");
 				#ifdef DEBUG_ON 
 				printf(" &amp; ");
 				#endif
 			}
 			k++;
 		}
 
 		int l=0;
 
 		derivative = loc->derivative;
 		while(derivative!=NULL)
 		{
 			loctemp->flows[l] = derivative;
 			derivative = derivative->next;
 			l++;
 		}
 		k=0;
 		fprintf(fptr, "\t\t\t<flow>");
 		
 		#ifdef DEBUG_ON 
 		printf("\t\t\t<flow>");
 		#endif
 		
 		while(loctemp->flows!=NULL)
 		{
 			if(k>=l) break;
 			fprintf(fptr, "%s%s%s", loctemp->flows[k]->LHS, SpaceExOpMap(loctemp->flows[k]->op), loctemp->flows[k]->RHS);
 			
 			#ifdef DEBUG_ON 
 			printf("%s%s%s", loctemp->flows[k]->LHS, SpaceExOpMap(loctemp->flows[k]->op), loctemp->flows[k]->RHS);
 			#endif
 			
 			if(loctemp->flows[k]->next != NULL)
 			{
 				fprintf(fptr, " &amp; ");
 				
 				#ifdef DEBUG_ON 
 				printf(" &amp; ");
 				#endif
 			}
 			else 
 			{
 				fprintf(fptr, "</flow>\n");
 				
 				#ifdef DEBUG_ON 
 				printf("</flow>\n");
 				#endif
 			}
 			k++;
 
 		}
 	
 		loctemp->next = locSp->next;
 		locSp->next = loctemp;
 		fprintf(fptr, "\t\t</location>\n");
 		
 		#ifdef DEBUG_ON 
 		printf("\t\t</location>\n");
 		#endif
 		
 		fflush(fptr);
 		loc = loc->next;
 	} 
 	
 	#ifdef DEBUG_ON 
 	printf("\n\nPrinting Location Transition Map\n\n");
 	#endif
 	
 	while(loc2!=NULL)
 	{
 		int p=0,q=0,r=0,l,m;
 		transitions = loc2->transitions;
 
 		while(transitions!=NULL)
 		{
 			l=0;
 			m=0;
 			
 			#ifdef DEBUG_ON 
 			printf("Transition Not Null\n");
 			#endif
 			
 			transSpPtr = transSp;
 			while(transSpPtr->next!=NULL)
 			{
 				transSpPtr = transSpPtr->next;
 			}
 			
 			transtemp = (struct transitionSpaceEx*)malloc(sizeof(struct transitionSpaceEx));
 			when = transitions->when;
 			
 
 			while(when!=NULL)
 			{
 				transtemp->guard[p] = when;
 				when = when->next;
 				p++;
 				l=p;
 			}
 			p=0;
 			resets = transitions->reset;
 			while(resets!=NULL)
 			{
 				transtemp->assignments[q] = resets;
 				resets = resets->next;
 				q++;
 				m=q;
 			}
 			q=0;
 			strcpy(transtemp->label,transitions->sync);
 			std::map<char* ,int>::iterator it1 = locNametoIDMap.begin();
 			std::map<char* ,int>::iterator it2 = locNametoIDMap.begin();
 			while(it1 != locNametoIDMap.end())
 			{
 				if(strcmp(it1->first, loc2->name)==0)
 				{
 					transtemp->sourceID = it1->second;
 					break;
 				}
 				it1++;
 			}
 			while(it2 != locNametoIDMap.end())
 			{
 				if(strcmp(it2->first, transitions->gotoLoc) ==0)
 				{	
 					transtemp->targetID = it2->second;
 					break;
 				}
 				it2++;
 			}
 
 			
 			fprintf(fptr, "\t\t<transition source=\"%d\" target=\"%d\">\n", transtemp->sourceID,transtemp->targetID);
 			fprintf(fptr, "\t\t\t<label>%s</label>\n", transtemp->label);
 			fprintf(fptr, "\t\t\t<guard>");
 			
 			#ifdef DEBUG_ON 
 				printf("\t\t<transition source=\"%d\" target=\"%d\">\n", transtemp->sourceID,transtemp->targetID);
 				printf("\t\t\t<label>%s</label>\n", transtemp->label);
 				printf("\t\t\t<guard>");
 			#endif
 			
 			r=0;
 			while(transtemp->guard!=NULL)
 			{
 				if(r>=l) break;
 				if(strcasecmp(transtemp->guard[r]->LHS,"true")==0)
					fprintf(fptr, "true");
				else 
					fprintf(fptr, "%s%s%s", transtemp->guard[r]->LHS, SpaceExOpMap(transtemp->guard[r]->op), transtemp->guard[r]->RHS);
 				
 				#ifdef DEBUG_ON 
 				printf("%s%s[%s]\n", transtemp->guard[r]->LHS, SpaceExOpMap(transtemp->guard[r]->op), transtemp->guard[r]->RHS);
 				#endif
 				
 				if(transtemp->guard[r]->next != NULL)
 				{
 					fprintf(fptr, " &amp; ");
 					#ifdef DEBUG_ON 
 					printf(" &amp; ");
 					#endif
 				}
 				else 
 				{
 					fprintf(fptr, "</guard>\n");
 					
 					#ifdef DEBUG_ON 
 					printf("</guard>\n");
 					#endif
 				}
 				r++;
 			}
 			r=0;
 			fprintf(fptr, "\t\t\t<assignment>");
 			
 			#ifdef DEBUG_ON 
 			printf("\t\t\t<assignment>");
 			#endif
 			
 			while(transtemp->assignments!=NULL)
 			{
 				char oper[5];
 				if(r>=m) break;
 				if(strcmp(SpaceExOpMap(transtemp->assignments[r]->op), "=") == 0)
 					strcpy(oper, "==");
 				else
 					strcpy(oper, SpaceExOpMap(transtemp->assignments[r]->op));
 				fprintf(fptr, "%s%s%s", transtemp->assignments[r]->LHS, oper, transtemp->assignments[r]->RHS);
 				
 				#ifdef DEBUG_ON 
 				printf("%s%s%s", transtemp->assignments[r]->LHS, oper, transtemp->assignments[r]->RHS);
 				#endif
 				if(transtemp->assignments[r]->next != NULL)
 				{
 					fprintf(fptr, " &amp; ");
 					#ifdef DEBUG_ON 
 					printf(" &amp; ");
 					#endif
 				}
 				else 
 				{
 					fprintf(fptr, "</assignment>");
 					
 					#ifdef DEBUG_ON 
 					printf("</assignment>");
 					#endif
 					
 				}
 				r++;
 			}
 
 			transtemp->next = transSpPtr->next;
 			transSpPtr->next = transtemp;
 			fprintf(fptr, "\n\t\t</transition>\n");
 			#ifdef DEBUG_ON 
 			printf("\n\t\t</transition>\n");
 			#endif
 			transitions = transitions->next;
 		}
 
 		loc2 = loc2->next;
 	}
 
 	fprintf(fptr, "\t</component>\n");
 	fprintf(fptr, "\t<component id=\"%s\">\n", ha->name);
 	
 	#ifdef DEBUG_ON 
 	printf("\t</component>\n");
 	printf("\t<component id=\"%s\">\n", ha->name);
 	#endif
 	
 	while(contr_vars3!=NULL)
 	{
 		fprintf(fptr, "\t\t<param name=\"%s\" type=\"real\" local=\"false\" dynamics=\"any\" controlled=\"true\" d1=\"1\" d2=\"1\" />\n",contr_vars3->name);
 		
 		#ifdef DEBUG_ON 
 		printf("\t\t<param name=\"%s\" type=\"real\" local=\"false\" dynamics=\"any\" controlled=\"true\" d1=\"1\" d2=\"1\" />\n",contr_vars3->name);
 		#endif
 		
 		contr_vars3 = contr_vars3->next;
 	}
 	
 	while(params3!=NULL)
 	{
 		init_conds3=HA->init->init_conds;
 		while(init_conds3!=NULL)
 		{
 			if(strcmp(init_conds3->RHS,params3->LHS)==0)
 			{
 				fprintf(fptr, "\t\t<param name=\"%s\" type=\"real\" local=\"false\" dynamics=\"const\" controlled=\"true\" d1=\"1\" d2=\"1\" />\n", init_conds3->RHS);
 				
 				#ifdef DEBUG_ON 
 				printf("\t\t<param name=\"%s\" type=\"real\" local=\"false\" dynamics=\"const\" controlled=\"true\" d1=\"1\" d2=\"1\" />\n", init_conds3->RHS);
 				#endif
 				
 				break;
 			}
 			init_conds3=init_conds3->next;
 		}
 		params3=params3->next;	
 	}
 	while(synclabs3!=NULL)
 	{
 		fprintf(fptr, "\t\t<param name=\"%s\" type=\"label\" local=\"false\"/>\n", synclabs3->name);
 		
 		#ifdef DEBUG_ON 
 		printf("\t\t<param name=\"%s\" type=\"label\" local=\"false\"/>\n", synclabs3->name);
 		#endif
 		
 		synclabs3 = synclabs3->next;	
 	}
 	fprintf(fptr, "\t\t<bind component=\"%s_template\" as=\"%s_bind\">\n", ha->name, ha->name);
 	
 	#ifdef DEBUG_ON 
 	printf("\t\t<bind component=\"%s_template\" as=\"%s_bind\">\n", ha->name, ha->name);
 	printf("\nPARAMS \n");
 	#endif
 	
 	while(params2!=NULL)
 	{
 		fprintf(fptr, "\t\t\t<map key=\"%s\">%s</map>\n", params2->LHS, params2->RHS);
 		
 		#ifdef DEBUG_ON 
 		printf("\t\t\t<map key=\"%s\">%s</map>\n", params2->LHS, params2->RHS);
 		#endif
 		
 		params2 = params2->next;
 	}
 	
 	#ifdef DEBUG_ON 
 	printf("\nINITIAL CONDS\n");
 	#endif
 	
 	while(contr_vars2!=NULL)
 	{
 		fprintf(fptr, "\t\t\t<map key=\"%s\">%s</map>\n", contr_vars2->name, contr_vars2->name);
 		
 		#ifdef DEBUG_ON 
 		printf("\t\t\t<map key=\"%s\">%s</map>\n", contr_vars2->name, contr_vars2->name);
 		#endif
 		
 		contr_vars2 = contr_vars2->next;
 	}
 	
 	#ifdef DEBUG_ON 
 	printf("\nSYNC LABS \n");
 	#endif
 	
 	while(synclabs2!=NULL)
 	{
 		fprintf(fptr, "\t\t\t<map key=\"%s\">%s</map>\n", synclabs2->name, synclabs2->name);
 		
 		#ifdef DEBUG_ON 
 		printf("\t\t\t<map key=\"%s\">%s</map>\n", synclabs2->name, synclabs2->name);
 		#endif
 		
 		synclabs2 = synclabs2->next;	
 	}
 	fflush(fptr);
 	fprintf(fptr, "\t\t</bind>\n");
 	fprintf(fptr, "\t</component>\n");
 	fprintf(fptr, "</sspaceex>");
 	
 	#ifdef DEBUG_ON 
 	printf("\t\t</bind>\n");
 	printf("\t</component>\n");
 	printf("</sspaceex>");
 	#endif
 	
 	fflush(fptr);
 	
 	#ifdef DEBUG_ON 
 	printf("\nAFTER FLUSHING TO FILE");
 	#endif
 	
 	fflush(stdout);
 	
 	#ifdef DEBUG_ON 
 	printf("\nAFTER FLUSHING STDOUT");
 	#endif
 	
 	fclose(fptr);
 	
 	#ifdef DEBUG_ON 
 	printf("\n--------------SpaceEx XML output is now available--------------\n");  
 	#endif
 	
 	return finalID;
 }


void writeSpaceExConfig(struct phaver* HA,char *fileName, double samplingTime, double timeHorizon, int scenario, char* featureName){
	#ifdef DEBUG_ON 
		printf("[writeSpaceExConfig] STARTED\n");
	#endif
	
	FILE *out = fopen(fileName,"w");
	fprintf(out,"system = %s\n",HA->ha->name);
	//fprintf(out,"initially = \"");
	printSpaceExConfigInitial(HA->ha->name,HA->init,out);
	if(scenario == 0){
		fprintf(out,"scenario = supp\n");
	} else {
		fprintf(out,"scenario = stc\n");
	}
	fprintf(out,"directions = oct\n");
	fprintf(out,"set-aggregation = \"chull\"\n");
	fprintf(out,"sampling-time = %*.*lf\n",DBL_DIG,DBL_MANT_DIG,samplingTime);
	//fprintf(out,"sampling-time = %*.*lf\n",(double)samplingTime);
	fprintf(out,"flowpipe-tolerance = %*.*lf\n",DBL_DIG,DBL_MANT_DIG,samplingTime);//0.001\n");
	fprintf(out,"time-horizon = %lf\n",timeHorizon);
	fprintf(out,"clustering = 100\n");
	fprintf(out,"iter-max = -1\n");
	if(featureName!=NULL){
		fprintf(out,"output-variables = \"%s\"\n",featureName);
	} else {
		fprintf(out,"output-variables = \"");
		struct identifier* var = HA->ha->contr_vars;
		while(var && var->next){
			fprintf(out,"%s,",var->name);
			var = var->next;
		}
		fprintf(out,"%s\"\n",var->name);
	}
	fprintf(out,"output-format = INTV\n");
	fprintf(out,"verbosity = m\n");
	fprintf(out,"output-error = 0\n");
	fprintf(out,"rel-err = 1.0E-12\n");
	fprintf(out,"abs-err = 1.0E-15\n");
	
	fclose(out);
	
	#ifdef DEBUG_ON 
		printf("[writeSpaceExConfig] ENDED\n");
	#endif
}

int readModelList(struct identifier** list,char* libPath){
	if(strlen(trim(libPath))>0){
		int modelCount = 0;
		*list = NULL;
		char fileName[MAX_STR_LENGTH];
		sprintf(fileName,"%s/modelList.txt",libPath);
		
		FILE *modelList = fopen(fileName,"r");
		
		char model[MAX_STR_LENGTH];
		while(fscanf(modelList,"%s",model)!=EOF){
			*list = addToIdentifierList(*list,model);
			modelCount++;
		}
		
		fclose(modelList);
		return modelCount;
	}
	
}

int displayModelList(struct identifier* list){
	struct identifier* model = list;
	int count = 0;
	
	printf("\n\n------------------------------Model Library-------------------------------\n");
	
	while(model!=NULL){
		printf("[%d] %s\n",++count,model->name);
		model = model->next;		
	}
	   printf("--------------------------------------------------------------------------\n");
	return count;
}

int requestModelChoice(int count){
	int id;
	
	do{
		printf("Enter the ID of the model you wish to generate: ");
		scanf("%d",&id);
	} while(!(id>=0 & id<=count));
	
	if(id == 0) exit(0);
	if(runFile) fprintf(runFile,"%d\n",id);
	return id;	
}

int displayModelParameters(struct phaver* HA){
	struct condition* param = HA->params;
	int count=0;
	printf("\n\n------------------------------Parameter List------------------------------");
	while(param!=NULL){
		printf("\n");
		printf("[%d] : ",++count);
		printCondition(param);
		param = param->next;
	}
	printf("\n--------------------------------------------------------------------------\n");
	return count;
}

int requestParamChoice(int count){
	int id;
	
	do{
		//printf("\n\n--------------------------------------------------------------------\n");
		printf("Enter the ID of the parameter you wish to edit.\n");
		printf("If you are satisfied with the value list enter 0.\n");
		printf("Your choice: ");
		scanf("%d",&id);
	} while(!(id>=0 & id<=count));
	printf("\n--------------------------------------------------------------------------\n");
	return id;	
}

int editParameter(int id, struct condition* params){
	struct condition* param = params;
	int count=1;
	while(count!=id & param!=NULL){
		param=param->next;
		count++;
	}
	
	if(param == NULL){ return -1 ;}
	
	printf("Present Parameter Value: "); printCondition(param);
	
	printf("Suggested Value: ");
	scanf("%s",param->RHS);
	if(runFile) fprintf(runFile,"%s\n",param->RHS);
	printf("Updated Parameter Value: "); printCondition(param);
	
	return id;
}

char* getModelName(int id, struct identifier* list){
	if(list!=NULL){
		struct identifier* model = list;
		int count=1;
		while(count!=id & model!=NULL){
			model = model->next;
			count++;
		}
	
		if(model == NULL){ return NULL ;}
		
		return model->name;
	}
	return NULL;
}

void readFeatureList(struct identifier** list,char* modelName, char* libPath){
	#ifdef DEBUG_ON 
		printf("[readFeatureList] STARTED\n");
	#endif
	if(libPath!=NULL){
		*list = NULL;
		char fileName[MAX_STR_LENGTH];
		sprintf(fileName,"%s/features/%s/featureList.txt",libPath,modelName);
		FILE *featureList = fopen(fileName,"r");
		
		if(!featureList){
			printf("\nWe couldn't find the feature folder for model "%s". Make sure the feature spec is present at this location.\n",modelName);
			exit(0);
		}
		
		char feature[MAX_STR_LENGTH];
		while(fscanf(featureList,"%s",feature)!=EOF){
			*list = addToIdentifierList(*list,feature);
		}
		
		fclose(featureList);    
	}
	#ifdef DEBUG_ON 
		printf("[readFeatureList] ENDED\n");
	#endif
}

int displayFeaturelList(struct identifier* list){
	#ifdef DEBUG_ON 
		printf("[displayFeaturelList] STARTED\n");
	#endif
	
	struct identifier* feature = list;
	int count = 0;
	printf("\n\n------------------------------Feature Library-----------------------------\n");
	while(feature!=NULL){
		printf("[%d] %s\n",++count,feature->name);
		feature = feature->next;		
	}
	    printf("--------------------------------------------------------------------------\n");
	#ifdef DEBUG_ON 
		printf("[displayFeaturelList] ENDED\n");
	#endif
	
	return count;
}

int requestFeatureChoice(int count){
	#ifdef DEBUG_ON 
		printf("[requestFeatureChoice] STARTED\n");
	#endif
	
	int id;
	
	do{
		printf("Enter the ID of the feature you wish to compute: ");
		scanf("%d",&id);
		if(runFile) fprintf(runFile,"%d\n",id);
	} while(!(id>=0 & id<=count));
	printf("\n--------------------------------------------------------------------------\n");
	if(id == 0){
		#ifdef DEBUG_ON 
			printf("[requestFeatureChoice] ENDED by EXITING PROGRAM\n");
		#endif
		exit(0);
	}
	
	#ifdef DEBUG_ON 
		printf("[requestFeatureChoice] ENDED\n");
	#endif
	
	return id;	
}


char* getFeatureName(int id, struct identifier* list){
	#ifdef DEBUG_ON 
		printf("[getFeatureName] STARTED\n");
	#endif
	
	struct identifier* feature = list;
	int count=1;
	while(count!=id & feature!=NULL){
		feature = feature->next;
		count++;
	}
	
	if(feature == NULL){ return NULL ;}
	
	#ifdef DEBUG_ON 
		printf("[getFeatureName] ENDED\n");
	#endif
	
	return feature->name;
}

char* strToUpper(char* str){
	int len = strlen(str);
	char* newStr = (char*)malloc(sizeof(char)*(len+1));
	
	for(int i=0;i<len;i++){
		newStr[i] = toupper(str[i]);
	}
	newStr[len] = '\0';
	return newStr;
}

struct value* addToValues(struct value* root, double encL, double encR, double timeL, double timeR){
	#ifdef DEBUG_ON
	printf("[addToValues] STARTED\n");
	#endif

	struct value* val = (struct value*)malloc(sizeof(struct value));
	struct value* temp = root;

	val->encL = encL;
	val->encR = encR;
	val->timeL = timeL;
	val->timeR = timeR;
	val->next = NULL;

	#ifdef DEBUG_ON
	printf("[addToValuesstructs] CRETED NODE\n");
	#endif


	if(root!=NULL){
		#ifdef DEBUG_ON
		printf("[addToValues] VALUE LIST NOT EMPTY\n");
		#endif

		while(temp->next!=NULL){
			temp=temp->next;
		}
		#ifdef DEBUG_ON
		printf("[addToValues] VALUE TO CONDITION LIST\n");
		#endif

		temp->next = val;

	} else {
		#ifdef DEBUG_ON
		printf("[addToValues] VALUE LIST EMPTY\n");
		printf("[addToValues] ADDING TO VALUE LIST\n");
		#endif

		root = val;

		#ifdef DEBUG_ON
		printf("[addToValues] ROOT CHANGED\n");
		printf("[addToValues] ROOT = %p, NEXT = %p\n",root,root->next);
		#endif

	}
	

	return root;
}

void freeValueList(struct value* root){
	if(root!=NULL){
		freeValueList(root->next);
		free(root);
	}
}

void freeVar(struct varValue* root){
	if(root!=NULL){
		freeValueList(root->values);
		free(root);
	}
}

int getSpaceExScenario(){
	printf("\nChoose a SpaceEx Scenario: [2 is default]\n\t1. LGG\n\t2. STC\nYour choice: ");
	int scenario = 2;
	scanf("%d",&scenario);
	if(runFile) fprintf(runFile,"%d\n",scenario);
	if(scenario==1)
		return 0;
	return 1;
}

char* trim(char *str) {
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

struct interval* createInterval(){
	return createIntervalInit(0.0,0.0,0);
}

struct interval* createIntervalInit(double left, double right, int id){
	struct interval* range = (struct interval*)malloc(sizeof(struct interval));
	range->left = left;
	range->right = right;
	range->id = id;
	return range;
}

int fileCopy(const char* source, const char* dest){
	if(source!=NULL && dest!=NULL){
		pid_t cpPID=fork();
		if(cpPID == 0){
			execlp("cp", "cp", source, dest,(char*)NULL);
			printf("ERROR: FAILED TO COPY %s  to %s\n",source,dest);
			exit(127);
			
		} else {
			waitpid(cpPID,0,0); // wait for child to exit 
			return access( dest, F_OK );
		}
	}
}

//---------------------------SMT EXTENSIONS------------------------------------
char* constructGoal(char* featureName, struct interval* range,int id,int type){
	char* goal = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
	bzero(goal,sizeof(char)*MAX_STR_LENGTH);
	if(type == 0){
		sprintf(goal,"@%d (and (%s >= %lf) (%s <= %lf));",id,featureName,range->left,featureName,range->right);
		printf("\n%s\n",goal);
	} else if(type > 0) {
		sprintf(goal,"@%d (and (%s >= %lf));",id,featureName,range->left);
		printf("\n%s\n",goal);
	} else if(type < 0){
		sprintf(goal,"@%d (and true (%s <= %lf));",id,featureName,range->right);
		printf("\n%s\n",goal);
	}
	return goal;
}

struct interval* satCheck(char *featureName, int depth, float precision, char* goal, char* workPath,char* dReachPath){
	// Prepare Goal - Passed as Input
	
	// Prepare DRH with Goal
	char templateFileName[MAX_STR_LENGTH];
	char automatonFileName[MAX_STR_LENGTH];
	sprintf(templateFileName,"%s/drhTemplate.drh",workPath);
	sprintf(automatonFileName,"%s/aut.drh",workPath);
	
	pid_t cpPID=fork();
	if(cpPID == 0){		
		execlp("cp", "cp", templateFileName, automatonFileName,(char*)NULL);
		printf("ERROR: FAILED TO COPY DRH TEMPLATE: work/drhTemplate.drh\n");
		exit(127);
		
	} else {
		waitpid(cpPID,0,0); // wait for child to exit 
		
		FILE* drh = fopen(automatonFileName,"a");
		fprintf(drh,"\n\n");
		fprintf(drh,"goal: \n%s",goal);
		fflush(drh);
		fclose(drh);
		
		// Run dReach on the DRH file
		char drhOutputFileName[MAX_STR_LENGTH];
		sprintf(drhOutputFileName,"%s/drh.out",workPath);
		
		pid_t dreachPID=fork();
		if(dreachPID == 0){
			char* d = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			char* p = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			sprintf(d,"%d",depth);
			sprintf(p,"%f",precision);
			
			//Redirecting Output
			//char *satOut = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			//sprintf(satOut,"drh.out");
			
			FILE* drhOut = fopen(drhOutputFileName,"w");
			fflush(stdout);
			int save_out = dup(fileno(stdout));
			dup2(fileno(drhOut), STDOUT_FILENO);
			fclose(drhOut);
			
			execlp(dReachPath,dReachPath,"-k", d,automatonFileName,"--precision",p,"--visualize",(char*)NULL);
			dup2(save_out, STDOUT_FILENO);
			close(save_out);
			printf("\nERROR: FAILED TO EXECUTE dREACH\n");fflush(stdout);
			exit(127);
			
		} else {
			//printf("\nDo you want to force quit this search?");
			char cinput[MAX_STR_LENGTH]; //scanf("%s",cinput);
			sprintf(cinput,"n");
			if(cinput[0]=='y' || cinput[0] =='Y'){
				kill(dreachPID,SIGTERM);
			}
			waitpid(dreachPID,0,0); // wait for child to exit 
			
			drhOutin = fopen(drhOutputFileName,"r");
			
			drhOutparse();
			
			printf("kvalue = %d\n",kValue);
			printf("lvalue = %d\n",lValue);
			/*FILE* jsonOut = fopen("jsonLog.out", "w");
			fflush(jsonOut);
			int stdfd = dup2(fileno(jsonOut), STDOUT_FILENO);
			int errfd = dup2(fileno(jsonOut), STDERR_FILENO);
			fclose(jsonOut);*/
			struct interval* range = extractJSONFeatureInterval(featureName,lValue,kValue,workPath);
			if(range==NULL){
				printf("\n Range is Null\n");
			} else {
				#ifdef DEBUG_ON
					printf("[extremeTraceSearch] Non-Null Range [%f,%f]\n",range->left,range->right);
				#endif
				char* oldFileName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
				char* newFileName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
				
				sprintf(oldFileName,"%s/aut_%d_%d.smt2.json",workPath,kValue,lValue);
				sprintf(newFileName,"%s/trace_%d.json",workPath,++traceCount);
				
				range->id = traceCount;
				
				if(fixNullsInJSON(oldFileName,newFileName)==0){
					printf("Problems were encountered while fixing JSON Nulls for trace file [%s]\nAttempting a file re-name to [%s].\n",oldFileName,newFileName);
					if(fileCopy(oldFileName,newFileName)!=0){//rename(oldFileName,newFileName)!=0){
						printf("Error Renaming File [%s] to [%s]\n",oldFileName,newFileName);
						exit(0);
					}
				}
				sprintf(oldFileName,"%s/aut.drh",workPath);
				sprintf(newFileName,"%s/aut_%d.drh",workPath,traceCount);
				
				if(fileCopy(oldFileName,newFileName)!=0){//rename(oldFileName,newFileName)!=0){
					printf("Error Renaming File [%s] to [%s]\n",oldFileName,newFileName);
					exit(0);
				}
				
				printf("Range = [%lf,%lf]\n",range->left, range->right);
			}
			
			/*dup2(stdfd, STDOUT_FILENO);
			dup2(errfd, STDERR_FILENO);
			close(stdfd);
			close(errfd);*/
			//fprintf(drh,"\n\n");
			return range;
			// Return Answer: SAT - with feature range , or UNSAT - with feature range.
		}
	}
	return NULL;
	
}

struct interval* extractJSONFeatureInterval(char *featureName, int l, int k, char* workPath){
	char* varName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
	sprintf(varName,"%s_%d_0",featureName,k);
	
	struct interval* range = NULL;
	
	char* fileName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
	sprintf(fileName,"%s/aut_%d_%d.smt2.json",workPath,k,l);
	//------------------JSON EXTRACT--------------------------
	JsonParser *jsonParser  =  NULL;
	GError *error  =  NULL;
	jsonParser = json_parser_new ();


	json_parser_load_from_file (jsonParser,
	                            fileName,
	                            &error);

	if (error) {
	    g_error_free(error);
	} else {
	    JsonNode *root;
	    root = json_parser_get_root(jsonParser);

	    if(root!=NULL){
	    	#ifdef DEBUG_ON
	    	printf("Root NOT NULL\n");
	    	#endif
	    	JsonObject *object = NULL;
	    	object = json_node_get_object(root);

	    	const gchar* memberName;
	    	JsonNode *memberValue;

	    	JsonObjectIter iter;
	    	json_object_iter_init(&iter,object);

	    	while(json_object_iter_next(&iter, &memberName, &memberValue)){
	    		//There should be a single object at the root named "traces"

	    		#ifdef DEBUG_ON
	    		printf("\nRoot Object Name = %s\n",memberName);
	    		#endif
	    		
	    		if(strcmp((const char*)memberName,"traces")!=0){
	    			#ifdef DEBUG_ON
	    			printf("Skipping this field of the object");
	    			#endif
	    			
	    			continue;
	    		}

	    		JsonArray *stepArray = NULL;
	    		stepArray = json_node_get_array(memberValue);
				
				#ifdef DEBUG_ON
	    		printf("\nListing SMT Unrolled Steps:\n");
				#endif
				
	    		int steps = json_array_get_length(stepArray);
	    		int stepIterator = 0;
	    		JsonNode *stepMember = NULL;

	    		if(steps==0){
	    			#ifdef DEBUG_ON
	    			printf("\nNo Step Array Members\n");
	    			#endif
	    			
	    			return 0;
	    		}

	    		while(stepIterator < steps){
	    			//For each step
	    			stepMember = json_array_get_element(stepArray,stepIterator);
	    			if(stepMember==NULL){
	    				#ifdef DEBUG_ON
	    				printf("\nError: Expected a Step Array Member at this position\n");
	    				#endif
	    				
	    				return 0;
	    			}
	    			
	    			#ifdef DEBUG_ON
	    			printf("\n\t Step %d\n",stepIterator);
	    			#endif
	    			
	    			JsonArray *step =json_node_get_array(stepMember);

	    			#ifdef DEBUG_ON
	    			printf("\n\t Variable Valuations\n");
	    			#endif
	    			
	    			int varCount = json_array_get_length(step);
	    			int varIterator = 0;
	    			JsonNode *var = NULL;

	    			#ifdef DEBUG_ON
	    			printf("\n\t Numebr of Variables: %d\n",varCount);
	    			#endif
	    			
	    			struct varValue* varVal = NULL;
	    			while(varIterator<varCount){
	    				var = json_array_get_element(step,varIterator);

	    				if(var==NULL){
	    					#ifdef DEBUG_ON
	    					printf("\nError: Expected a Variable Valuation Array Member at this position\n");
	    					#endif
	    					
	    					return 0;
	    				}
	    				#ifdef DEBUG_ON
		    			printf("\n\t Variable %d\n",varIterator);
		    			#endif

	    				JsonObject *varObject = json_node_get_object(var);

	    				JsonObjectIter varObjectIter;
	    				json_object_iter_init(&varObjectIter,varObject);

	    				const gchar *varMemberName;
	    				JsonNode *varMemberNode;
	    				//printf("After Initialization\n");fflush(stdout);

	    				varVal = (struct varValue*)malloc(sizeof(struct varValue));
	    				varVal->values = NULL;
	    				while(json_object_iter_next(&varObjectIter,&varMemberName,&varMemberNode)){
	    					//printf("%s : ",varMemberName);

	    					if(strcmp((const char*)varMemberName,"mode")==0){
	    						//int value;
	    						//value = json_node_get_int(varMemberNode);
	    						//printf("%d",value);
	    						varVal->mode = json_node_get_int(varMemberNode);

	    					}

	    					if(strcmp((const char*)varMemberName,"step")==0){
	    						varVal->step = json_node_get_int(varMemberNode);
	    					}

	    					if(strcmp((const char*)varMemberName,"key")==0){
	    						const gchar *key = json_node_get_string(varMemberNode);
	    						//if(strncmp(key,"vc",2)!=0){
	    						//	printf("\"%s\"\n",key);
	    						//	break;
	    						//}
	    						//printf("\"%s\"\n",key);
	    						strcpy(varVal->key,(const char*)key);
	    					}

	    					if(strcmp((const char*)varMemberName,"values")==0){
	    						JsonArray *values = json_node_get_array(varMemberNode);
	    						int valueCount = json_array_get_length(values);
	    						int valueIterator = 0;
	    						JsonNode *valueNode = NULL;
	    						JsonObject *valueObject = NULL;

	    						while(valueIterator < valueCount){
	    							valueNode = json_array_get_element(values,valueIterator);
	    							valueObject = json_node_get_object(valueNode);
	    							JsonObjectIter valueObjectIter;
	    							json_object_iter_init(&valueObjectIter,valueObject);
	    							const gchar* valueMemberName;
	    							JsonNode *valueMemberNode;
	    							double encL,encR,timeL,timeR;
	    							while(json_object_iter_next(&valueObjectIter,&valueMemberName,&valueMemberNode)){
	    								//printf("%s : ",valueMemberName);
	    								JsonArray *range = json_node_get_array(valueMemberNode);
	    								//int rangeIterator = 0;
	    								JsonNode *rangeValueL = json_array_get_element(range,0);
	    								JsonNode *rangeValueR = json_array_get_element(range,1);

	    								//printf("%lf , %lf\n",json_node_get_double(rangeValueL),json_node_get_double(rangeValueR));
	    								if(strcmp((const char*)valueMemberName,"enclosure")==0){
	    									encL = json_node_get_double(rangeValueL);
	    									encR = json_node_get_double(rangeValueR);
	    								} else {
	    									timeL = json_node_get_double(rangeValueL);
	    									timeR = json_node_get_double(rangeValueR);
	    								}

	    							}
	    							varVal->values = addToValues(varVal->values,encL,encR,timeL,timeR);
	    							/*if(varVal->values == NULL){
	    								printf("ITS NULL\n");
	    								exit(0);
	    							} else {
	    								struct value* v = varVal->values;
	    								printf("\n\n");
	    								while(v){

	    									printf("enc:[%lf,%lf] : time:[%lf,%lf]\n",v->encL,v->encR,v->timeL,v->timeR);
	    									v = v->next;
	    								}
	    							}*/
	    							valueIterator++;
	    						}
	    						/*if(varVal->values == NULL){
	    							printf("ITS NULL\n");
	    							exit(0);
	    						} else {
	    							struct value* v = varVal->values;
	    							printf("\n\n");
	    							while(v){

	    								printf("enc:[%lf,%lf] : time:[%lf,%lf]\n",v->encL,v->encR,v->timeL,v->timeR);
	    								v = v->next;
	    							}
	    						}*/
	    						//printf("\nInner Block varValues = %p",varVal->values);
	    					}
	    					//printf("\nOuter Block varValues = %p",varVal->values);
	    					/*if(varVal->values == NULL){
	    						printf("\nITS NULL\n");
	    						exit(0);
	    					} else {
	    						struct value* v = varVal->values;
	    						printf("\n\n");
	    						while(v){

	    							printf("enc:[%lf,%lf] : time:[%lf,%lf]\n",v->encL,v->encR,v->timeL,v->timeR);
	    							v = v->next;
	    						}
	    					}
	    					printf("\n");*/
	    					fflush(stdout);
	    				}
	    				//printf("\nReally Outer Block varValues = %p",varVal->values);
	    				
	    				#ifdef DEBUG_ON
	    				printf("varVal->key : %s\n",varVal->key);
	    				printf("COMPARE: %d\n", strcmp(varVal->key,varName));
	    				#endif
	    				
	    				if(strcmp(varVal->key,varName)==0){
	    					#ifdef DEBUG_ON
	    					printf("PRINTING\n");
	    					#endif
	    					
	    					range = createInterval();
	    					if(varVal->values == NULL){
	    						#ifdef DEBUG_ON
	    						printf("ITS NULL\n");
	    						#endif
	    						exit(0);
	    					}
	    					struct value* v = varVal->values;
	    					while(v){
								#ifdef DEBUG_ON
	    						printf("enc:[%lf,%lf] : time:[%lf,%lf]\n",v->encL,v->encR,v->timeL,v->timeR);
	    						#endif
	    						
	    						range->left = v->encL;
	    						range->right = v->encR;
	    						v = v->next;
	    					}
	    				}
	    				free(varVal);
	    				varIterator++;
	    			}
	    			stepIterator++;
	    		}
	    	}
	    	//printf("JSON: %s\n",json_node_get_string(root));
	    	//printf("JSON: %s\n",json_node_type_name(root));
	    	//printf("JSON: %s\n",json_node_get_value_type(root));
	    }
	}
	
	free(varName);
	free(fileName);
	
	return range;
	
}


int isnumber(char* tempString){
	if(tempString){
		int num;
		num = atoi( tempString );
		 
		if (num == 0 && tempString[0] != '0')
		   return 0;
		else
		   return 1;
	}
}

struct keyItem* parseKey(char *src){
	char abc[MAX_STR_LENGTH];
	//sscanf(src,"%[a-zA-Z0-9]_%d_%d",abc,&i,&j);
	//printf("STR = %s\ni = %d\nj = %d\n",abc,i,j);
	
	//sscanf(src,"%[a-zA-Z0-9]",abc);
	//printf("STR = %s\n",abc);
	
	char *tempPtr = src;
	
	int keyStepID[2] = {-1,-1};
	
	char* temp_keyStr = (char*)(malloc(sizeof(char)*MAX_STR_LENGTH));bzero(temp_keyStr,sizeof(char)*MAX_STR_LENGTH);
	char* key_str = (char*)(malloc(sizeof(char)*MAX_STR_LENGTH));bzero(key_str,sizeof(char)*MAX_STR_LENGTH);
	char* key_str_1 = (char*)(malloc(sizeof(char)*MAX_STR_LENGTH));bzero(key_str_1,sizeof(char)*MAX_STR_LENGTH);
	char* key_str_2 = (char*)(malloc(sizeof(char)*MAX_STR_LENGTH));bzero(key_str_2,sizeof(char)*MAX_STR_LENGTH);
	
	//printf("ORIGINAL = %s\n",tempPtr);
	sscanf(tempPtr,"%[a-zA-Z0-9]",abc);
	//printf("STR = %s\n",abc);
	
	int numberRead = 0;
	
	int charsRead = 0;
	int charsReadAfterFirstNumber = 0;

	while(!(tempPtr[0]=='\0')){
		sscanf(tempPtr,"%[a-zA-Z0-9]",abc);
		//printf("[%s] - [%s] isNumeber?%s\n",tempPtr,abc,isnumber(abc)?"YES":"NO");
		//printf("before=[%s]\n",key_str);
		
		charsRead += strlen(abc);
		
		if(isnumber(abc)){
			if(keyStepID[0]!=-1 && keyStepID[1]!=-1){ //More than two numbers read
				tempPtr+=strlen(abc);
				
				if(tempPtr[0]=='_'){
					tempPtr++;charsRead++;
					//printf("[%s]\n",abc);
					key_str = strncpy(key_str,src,charsRead-strlen(abc));
					keyStepID[0] = atoi(abc);
					charsReadAfterFirstNumber=strlen(abc);
				} else {
					//printf("[%s]\n",abc);
					key_str = strncpy(key_str,src,charsRead-charsReadAfterFirstNumber-1-strlen(abc));
					keyStepID[0] = keyStepID[1];
					keyStepID[1] = atoi(abc);
				}
				
			} else if(keyStepID[0]!=-1){	//Second
				tempPtr+=strlen(abc);
				if(tempPtr[0]=='_'){
					tempPtr++;charsRead++;
					//printf("[%s]\n",abc);
					key_str = strncpy(key_str,src,charsRead-strlen(abc));
					keyStepID[0] = atoi(abc);
					charsReadAfterFirstNumber=strlen(abc);
				} else {
					charsReadAfterFirstNumber=+strlen(abc);
					keyStepID[1] = atoi(abc);
				}
			} else {	//First
				keyStepID[0] = atoi(abc);
				numberRead = 1;
				
				tempPtr+=strlen(abc);
				charsReadAfterFirstNumber=strlen(abc);
				
				if(tempPtr[0]=='_'){
					tempPtr++;charsReadAfterFirstNumber++;
					charsRead++;
				}
			}	
			
		} else {
			keyStepID[0] = -1; keyStepID[1] = -1;
			key_str = strncpy(key_str,src,charsRead);
			
			tempPtr+=strlen(abc);
			if(tempPtr[0]=='_'){
				key_str = strcat(key_str,"_");
				charsRead++;
				tempPtr++;
			}
		}
		bzero(abc,sizeof(char)*110);
		
		//printf("after=[%s] [%d,%d]\n",key_str,keyStepID[0],keyStepID[1]);
	}
	
	//printf("STR = %s\ni = %d\nj = %d\n",key_str,keyStepID[0],keyStepID[1]);
	struct keyItem* KI= (struct keyItem*)malloc(sizeof(struct keyItem));
	KI->step = keyStepID[0];
	KI->varID = keyStepID[1];
	KI->keyPrefix = (char*)malloc(MAX_STR_LENGTH*sizeof(char));
	bzero(KI->keyPrefix,sizeof(char)*MAX_STR_LENGTH); 
	KI->keyPrefix = strcpy(KI->keyPrefix,key_str);
	return KI;
}

int fixNullsInJSON(char* oldFileName, char* newFileName){
	//char* fileName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
	//sprintf(fileName,"%s/aut_%d_%d.smt2.json",workPath,k,l);
	
	//char* clean_fileName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
	//sprintf(clean_fileName,"%s/trace_%d_%d.json",workPath,k,l);
	
	FILE* JSON_out = fopen(newFileName,"w");
	
	//------------------JSON EXTRACT--------------------------
	JsonParser *jsonParser  =  NULL;
	GError *error  =  NULL;
	jsonParser = json_parser_new ();


	json_parser_load_from_file (jsonParser,
	                            oldFileName,
	                            &error);

	if (error) {
	    g_error_free(error);
	} else {
	    JsonNode *root;
	    root = json_parser_get_root(jsonParser);
		
	    if(root!=NULL){
	    	#ifdef DEBUG_ON
				printf("Root NOT NULL\n");
	    	#endif
	    	
	    	//[JSON] START ROOT NODE 
	    	fprintf(JSON_out,"{\n");
	    	
	    	JsonObject *object = NULL;
	    	object = json_node_get_object(root);

	    	const gchar* memberName;
	    	JsonNode *memberValue;

	    	JsonObjectIter iter;
	    	json_object_iter_init(&iter,object);

	    	while(json_object_iter_next(&iter, &memberName, &memberValue)){
	    		//There should be a single object at the root named "traces"

	    		#ifdef DEBUG_ON
	    		printf("\nRoot Object Name = %s\n",memberName);
	    		#endif
	    		
	    		if(strcmp((const char*)memberName,"traces")!=0){
	    			#ifdef DEBUG_ON
	    			printf("Skipping this field of the object");
	    			#endif
	    			
	    			continue;
	    		}
	    		//[JSON] \t "traces"
				fprintf(JSON_out,"\t\"traces\": [");
				
	    		JsonArray *stepArray = NULL;
	    		stepArray = json_node_get_array(memberValue);
				
				#ifdef DEBUG_ON
	    		printf("\nListing SMT Unrolled Steps:\n");
				#endif
				
	    		int steps = json_array_get_length(stepArray);
	    		int stepIterator = 0;
	    		int expectedStep = 0;
	    		JsonNode *stepMember = NULL;

	    		if(steps==0){
	    			#ifdef DEBUG_ON
	    			printf("\nNo Step Array Members\n");
	    			#endif
	    			
	    			fprintf(JSON_out,"\t]\n}");
	    			fclose(JSON_out);
	    			
	    			return 1;
	    		}
				
	    		while(stepIterator < steps){
	    			//For each step
	    			stepMember = json_array_get_element(stepArray,stepIterator);
	    			if(stepMember==NULL){
	    				#ifdef DEBUG_ON
	    				printf("\nError: Expected a Step Array Member at this position\n");
	    				#endif
	    				
						stepIterator++;
	    				continue;
	    			}
	    			#ifdef DEBUG_ON
	    			printf("\n\t Step %d\n",stepIterator);
	    			#endif
	    			
	    			JsonArray *step =json_node_get_array(stepMember);

	    			#ifdef DEBUG_ON
	    			printf("\n\t Variable Valuations\n");
	    			#endif
	    			
	    			int varCount = json_array_get_length(step);
	    			int varIterator = 0;
	    			JsonNode *var = NULL;

	    			#ifdef DEBUG_ON
	    			printf("\n\t Number of Variables: %d\n",varCount);
	    			#endif
	    			
	    			struct varValue* varVal = NULL;
	    			if(varCount==0){
						stepIterator++;
						continue;
					}
					
					//[JSON] START STEP
	    			fprintf(JSON_out,"\t\t[\n");
	    			
					while(varIterator<varCount){
	    				var = json_array_get_element(step,varIterator);
						
	    				if(var==NULL){
	    					#ifdef DEBUG_ON
	    					printf("\nError: Expected a Variable Valuation Array Member at this position\n");
	    					#endif
	    					fprintf(JSON_out,"\t\t]\n\t]\n}");
	    					return 0;
	    				}
	    				
	    				//[JSON] START Variable
	    				fprintf(JSON_out,"\t\t\t{\n");
	    				
	    				#ifdef DEBUG_ON
		    			printf("\n\t Variable %d\n",varIterator);
		    			#endif

	    				JsonObject *varObject = json_node_get_object(var);

	    				JsonObjectIter varObjectIter;
	    				json_object_iter_init(&varObjectIter,varObject);

	    				const gchar *varMemberName;
	    				JsonNode *varMemberNode;
						
						int objectCount = 0;
						
						while(json_object_iter_next(&varObjectIter,&varMemberName,&varMemberNode)){
							objectCount++;
						}
						varObject = json_node_get_object(var);
						json_object_iter_init(&varObjectIter,varObject);
	    				//printf("After Initialization\n");fflush(stdout);

	    				varVal = (struct varValue*)malloc(sizeof(struct varValue));
	    				varVal->values = NULL;
	    				
						int objectIterator = 0;
						
	    				//For each variable in the current step
	    				while(json_object_iter_next(&varObjectIter,&varMemberName,&varMemberNode)){
	    					//printf("%s : ",varMemberName);
							objectIterator++;
							
	    					if(strcmp((const char*)varMemberName,"mode")==0){
	    						varVal->mode = json_node_get_int(varMemberNode);
								fprintf(JSON_out,"\t\t\t\t\"mode\": %d",varVal->mode);
								
	    					}

	    					if(strcmp((const char*)varMemberName,"step")==0){
	    						varVal->step = json_node_get_int(varMemberNode);
								fprintf(JSON_out,"\t\t\t\t\"step\": %d",expectedStep);
	    					}

	    					if(strcmp((const char*)varMemberName,"key")==0){
	    						const gchar *key = json_node_get_string(varMemberNode);
	    						
	    						strcpy(varVal->key,(const char*)key);
								struct keyItem* KI = parseKey(varVal->key);
								
	    						if(KI->step > expectedStep){
									fprintf(JSON_out,"\t\t\t\t\"key\": \"%s%d_%d\"",KI->keyPrefix,expectedStep,KI->varID);
								} else {
									fprintf(JSON_out,"\t\t\t\t\"key\": \"%s\"",varVal->key);
								}
								
	    					}

	    					if(strcmp((const char*)varMemberName,"values")==0){
								//[JSON] "values" START
								fprintf(JSON_out,"\t\t\t\t\"values\": [\n");
								
	    						JsonArray *values = json_node_get_array(varMemberNode);
	    						int valueCount = json_array_get_length(values);
	    						int valueIterator = 0;
	    						JsonNode *valueNode = NULL;
	    						JsonObject *valueObject = NULL;

	    						while(valueIterator < valueCount){
									//[JSON] "value" START
									fprintf(JSON_out,"\t\t\t\t\t{\n");
								
	    							valueNode = json_array_get_element(values,valueIterator);
	    							valueObject = json_node_get_object(valueNode);
	    							JsonObjectIter valueObjectIter;
	    							json_object_iter_init(&valueObjectIter,valueObject);
	    							const gchar* valueMemberName;
	    							JsonNode *valueMemberNode;
	    							double encL,encR,timeL,timeR;
	    							while(json_object_iter_next(&valueObjectIter,&valueMemberName,&valueMemberNode)){
	    								//printf("%s : ",valueMemberName);
	    								JsonArray *range = json_node_get_array(valueMemberNode);
	    								//int rangeIterator = 0;
	    								JsonNode *rangeValueL = json_array_get_element(range,0);
	    								JsonNode *rangeValueR = json_array_get_element(range,1);

	    								//printf("%lf , %lf\n",json_node_get_double(rangeValueL),json_node_get_double(rangeValueR));
	    								if(strcmp((const char*)valueMemberName,"enclosure")==0){
	    									encL = json_node_get_double(rangeValueL);
	    									encR = json_node_get_double(rangeValueR);
											//[JSON] "enclosure"
											fprintf(JSON_out,"\t\t\t\t\t\t\"enclosure\": [\n");
											fprintf(JSON_out,"\t\t\t\t\t\t\t%lf,\n",encL);
											fprintf(JSON_out,"\t\t\t\t\t\t\t%lf\n",encR);
											fprintf(JSON_out,"\t\t\t\t\t\t],\n");
	    								} else {
	    									timeL = json_node_get_double(rangeValueL);
	    									timeR = json_node_get_double(rangeValueR);
											//[JSON] "time"
											fprintf(JSON_out,"\t\t\t\t\t\t\"time\": [\n");
											fprintf(JSON_out,"\t\t\t\t\t\t\t%lf,\n",timeL);
											fprintf(JSON_out,"\t\t\t\t\t\t\t%lf\n",timeR);
											fprintf(JSON_out,"\t\t\t\t\t\t]\n");
	    								}

	    							}
	    							
	    							varVal->values = addToValues(varVal->values,encL,encR,timeL,timeR);
	    							valueIterator++;
									
									if(valueIterator==valueCount){
										//[JSON] "value" END
										fprintf(JSON_out,"\t\t\t\t\t}\n");
									} else {
										//[JSON] "value" END
										fprintf(JSON_out,"\t\t\t\t\t},\n");
									}
	    						}
	    						//[JSON] "values" START
								fprintf(JSON_out,"\t\t\t\t]");
							}
							if(objectIterator == objectCount){
								fprintf(JSON_out,"\n");
							} else {
								fprintf(JSON_out,",\n");
							}
	    					fflush(stdout);
	    				}
	    				
	    				free(varVal);
	    				varIterator++;
						
						if(varIterator==varCount){
							//[JSON] "values" START
							fprintf(JSON_out,"\t\t\t}\n");
						} else {
							//[JSON] "values" START
							fprintf(JSON_out,"\t\t\t},\n");
						}
	    			}
	    			
	    			stepIterator++;
					expectedStep++;
					if(stepIterator==steps){
						//[JSON] "STEP" END
						fprintf(JSON_out,"\t\t]\n");
					} else {
						//[JSON] "STEP" END
						fprintf(JSON_out,"\t\t],\n");
					}
					
	    		}
	    		//[JSON] "traces" END
				fprintf(JSON_out,"\t]\n");
	    	}
	    	fprintf(JSON_out,"}");
	    	//printf("JSON: %s\n",json_node_get_string(root));
	    	//printf("JSON: %s\n",json_node_type_name(root));
	    	//printf("JSON: %s\n",json_node_get_value_type(root));
	    }
	}
	
}

/*
 * Searching using SMT - dReach
 */
struct interval* extremeTraceSearch(char* featureName, struct interval* range, double e, int depth, int precision, int dir, int id, char* workPath,char* dReachPath){
	#ifdef DEBUG_ON
		printf("[extremeTraceSearch] STARTED\n");
	#endif
	if(featureName!=NULL && range!=NULL){
		double left = range->left;
		double right = range->right;
		double mid = left;
		
		//If Precision bound has been met
		if(fabs(left - right)<= 2*e){
			return range;
		}
		
		#ifdef DEBUG_ON
			printf("[extremeTraceSearch] Remembering Given Range\n");
		#endif
		struct interval* givenRange = createIntervalInit(range->left,range->right,id);
		
		
		#ifdef DEBUG_ON
			printf("[extremeTraceSearch] Computing Mid-Point\n");
		#endif
		//Compute the mid-point
		mid = (left + right)/2;
		
		struct interval* goalInterval = createInterval();
		struct interval* satResult = NULL;
		
		if(dir<0){	//LEFT Search - Left Interval
			goalInterval->left = left;
			goalInterval->right = mid;
		} else {	//RIGHT Search - Right Interval
			goalInterval->left = mid;
			goalInterval->right = right;
		}
		
		#ifdef DEBUG_ON
			printf("[extremeTraceSearch] Searching %s in [%f:%f]\n",(dir<0)?"left":"right",goalInterval->left,goalInterval->right);
		#endif
		
		
		#ifdef DEBUG_ON
			printf("[extremeTraceSearch] Calling SatCheck (1)\n");
		#endif
			satResult = satCheck(featureName,depth,precision,constructGoal(featureName, goalInterval,id,0), workPath, dReachPath);
		#ifdef DEBUG_ON
			printf("[extremeTraceSearch] Back from SatCheck (1)\n");
		#endif
		
		if(satResult!=NULL){ 
		//--------------------------- If SATISFIABLE then---------------------------------
			if(dir<0){
				givenRange->right = (satResult->left - e);
			} else {
				givenRange->left = (satResult->right + e);
			}
			givenRange->id = satResult->id;
			struct interval* temp = extremeTraceSearch(featureName, givenRange, e, depth, precision, dir, id, workPath, dReachPath);
			if(temp) return temp;
			else return satResult;

		} else {
		//------------------------- If NOT SATISFIABLE then------------------------------
			#ifdef DEBUG_ON
				printf("[extremeTraceSearch] Null Range \n");
			#endif
			if(dir<0){
				//LEFT Search - Right Interval
				goalInterval->left = mid;
				goalInterval->right = right;
			} else {
				//RIGHT Search - Left Interval
				goalInterval->left = left;
				goalInterval->right = mid;
			}
			
			#ifdef DEBUG_ON
				printf("[extremeTraceSearch] Calling SatCheck(2)\n");
			#endif
			
				satResult = satCheck(featureName,depth,precision,constructGoal(featureName, goalInterval,id,0), workPath, dReachPath);
			
			#ifdef DEBUG_ON
				printf("[extremeTraceSearch] Back from SatCheck(2)\n");
			#endif
			
			if(satResult != NULL) {
				if(dir<0){
					givenRange->left = mid;
					givenRange->right = (satResult->left - e);
				} else {
					givenRange->left = (satResult->right +e);
					givenRange->right = mid;
				}
				givenRange->id = satResult->id;
				struct interval *temp = extremeTraceSearch(featureName, givenRange, e, depth, precision, dir, id, workPath, dReachPath);
				if(temp) return temp;
				else return satResult;
			} else 
				return NULL;
		}
	}
	#ifdef DEBUG_ON
		printf("[extremeTraceSearch] ENDED with NULL\n");
	#endif
	return NULL;
}


float timedDiff(struct timeval t0, struct timeval t1){
	return (t1.tv_sec - t0.tv_sec)*1000.0f + (t1.tv_usec - t0.tv_usec)/1000.0f;
}


/*
 * Checks whether the directory exists, if it does not, return 0 else 1
 */
int checkDirExists(char* dir){
	struct stat st = {0};
	if(stat(dir,&st)==-1){
		return 0;
	} else return 1;
}

int checkDirReadWritePermission(char* dir){
	// Check write access
	int returnvalW = 0;
	returnvalW = access(dir, W_OK);
	
	int errW = errno;
	if(errW<0){
		errW = -errW;
	}
	
	int returnvalR = 0;
	int errR = 0;
	
	if(returnvalW!=0){
		returnvalR = access(dir, R_OK);
		errR = errno;
	}
	
	if(errR>0){
		errR = -errR;
	}
	
	int returnVal = returnvalW==0?0:(returnvalR==0?1:errR);
	return returnVal;
	// errno == ENOENT
	// No such file or directory
	
	// errno == EACCES
	// Write Permission denied
	
}

//------------------------------------ FEATURE_COMPUTE_METHODS -------------------------------
static int count = 0;

/*
 * For variable x, location l
 * 	Add x'==0 as an activity for location l
 */
void addLocalVariableActivityToLocation(struct location* loc, char* id, int temporal){
	if(loc!=NULL && id!=NULL){
		#ifdef DEBUG_ON 
		printf("[addLocalVariableActivityToLocation] STARTED Loc = [%s], id = [%s], temporal = [%d]\n",loc->name,id,temporal);
		#endif
		
		char *LHS = (char*)malloc(sizeof(char)*(sizeof(id)+2));
		char *RHS = (char*)malloc(sizeof(char)*(sizeof(char)*2));
		sprintf(LHS,"%s'",id);
		sprintf(RHS,"%d",temporal);
		loc->derivative = addToConditionList(loc->derivative,LHS,RHS,0);
		
		#ifdef DEBUG_ON 
		printf("[addLocalVariableActivityToLocation] ENDED\n");
		#endif
	}
}


/*
 * For the given variable "id" add an Identity reset
 * to the transition trans i.e. id' == id so that when
 * the transition is active the variables valuation
 * is retained accross the locations. 
 */
void addIdentityResetToTransition(struct transition* trans, char* id){
	if(trans!=NULL && id!=NULL){
		#ifdef DEBUG_ON 
		printf("[addIdentityResetToTransition] STARTED Transition TO: [%s] Variable ID = [%s]\n",trans->gotoLoc,id);
		#endif
		
		char *temp = (char*)malloc(sizeof(char)*(sizeof(id)+2));
		sprintf(temp,"%s'",id);
		
		char *temp2 = (char*)malloc(sizeof(char)*sizeof(id)+1);
		strcpy(temp2,id);
		
		trans->reset = addToConditionList(trans->reset,temp,temp2,0);
		
		#ifdef DEBUG_ON 
		struct condition* test=trans->reset;
		while(test!=NULL){
			printCondition(test);
			test = test->next;
		}
		printf("\n");
		
		printf("[addIdentityResetToTransition] ENDED\n");
		#endif
	}
}

/*
 * Add local variables of Feature f to the Hybrid Automaton Model
 */
struct phaver* addFeatureLocalVariables(struct phaver* pha, struct feature* f, double maxConst){
	#ifdef DEBUG_ON 
	printf("[addFeatureLocalVariables] STARTED\n");
	#endif
	
	struct identifier* localVars = f->def->localVars;
	maxConst = maxConst*10;
	char number[MAX_STR_LENGTH];
	sprintf(number,"%.8f",maxConst);
	while(localVars!=NULL){
		//Add Local Variable to Control Variable List
		pha->ha->contr_vars = addToIdentifierList(pha->ha->contr_vars, localVars->name);
		
		//Add Initial Value for localvariable
		pha->init->init_conds = addToConditionList(pha->init->init_conds,localVars->name,number,0); 
		
		localVars = localVars->next;
	}
	
	localVars = f->def->localVars;
	struct location* loc = pha->ha->locations;
	while(loc!=NULL){
		while(localVars!=NULL){
			//	Add Local Variable Activity (assuming no temporal 
			//	variable is in the list)
			addLocalVariableActivityToLocation(loc,localVars->name,0);
			
			//Add Identity Resets in Outgoing Transitions
			struct transition* trans = loc->transitions;
			while(trans!=NULL){
				addIdentityResetToTransition(trans,localVars->name);
				trans=trans->next;
			}
			localVars = localVars->next;
		}
		loc = loc->next;
	}
	#ifdef DEBUG_ON 
	printf("[addFeatureLocalVariables] ENDED\n");
	#endif
	
}

void addLocalVariables(struct phaver* pha, struct identifier* localVar, double maxConst, int temporal){
	if(pha && localVar){
		while(localVar){
			addLocalVariable(pha,localVar,maxConst,temporal);
			localVar = localVar->next;
		}
	}
}

/*
 * Add the single local Variable localVar to the Hybrid Automaton
 * Model.
 */
void addLocalVariable(struct phaver* pha, struct identifier* localVar, double maxConst, int temporal){
	if(localVar!=NULL){
		#ifdef DEBUG_ON 
		printf("[addLocalVariable] STARTED LocalVar being Added : [%s]\n",localVar->name);
		#endif
		//maxConst = maxConst*10;
		maxConst = (temporal == 0)? maxConst : 0;
		char number[MAX_STR_LENGTH];
		sprintf(number,"%10f",maxConst);
		char* localVarName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
		strcpy(localVarName,localVar->name);
		//Add Local Variable to Control Variable List
		pha->ha->contr_vars = addToIdentifierList(pha->ha->contr_vars, localVarName);
		
		//Add Initial Value for localvariable
		struct initial* initIterator = pha->init;
		while(initIterator!=NULL){
			if(initIterator->init_conds==NULL){
				printf("ERROR: No Initial Conditions Specified\n");
				exit(0);
			}
			addToConditionList(initIterator->init_conds,localVarName,number,0); 
			initIterator = initIterator->next;
		}
		
		//pha->init->init_conds = addToConditionList(pha->init->init_conds,localVar->name,number,0); 
		
		struct location* loc = pha->ha->locations;
		
		
		/*while(loc!=NULL){
		 *			printf("NEXT: loc [%s] \n",loc->name);
		 *			struct transition* trans = loc->transitions;
		 *			while(trans!=NULL){
		 *				struct condition* transCond = trans->reset;
		 *				printCondition(transCond);
		 *				printf("\n");
		 *				trans = trans->next;
	}
	printf("\n");
	loc = loc->next;
	}*/
		while(loc!=NULL){
			#ifdef DEBUG_ON 
			printf("[addLocalVariable] Location Being Processed: [%s]\n",loc->name);
			#endif
			//Add Local Variable Activity
			addLocalVariableActivityToLocation(loc,localVarName,temporal);
			
			//Add Identity Resets in Outgoing Transitions
			struct transition* trans = loc->transitions;
			while(trans!=NULL){
				//For each transition. Add an identity reset
				addIdentityResetToTransition(trans,localVarName);
				trans=trans->next;
				
			}
			loc = loc->next;
		}
		
		/*
		 *		loc = pha->ha->locations;
		 *		while(loc!=NULL){
		 *			printf("NEXT: ");
		 *			struct transition* trans = loc->transitions;
		 *			while(trans!=NULL){
		 *				printf("[%p] ",trans);
		 *				trans = trans->next;
	}
	printf("\n");
	loc = loc->next;
	}*/
		
		#ifdef DEBUG_ON 
		printf("[addLocalVariable] ENDED\n");
		#endif
		
	}
}

/*
 * Split location loc on PORV porv into l<id> and lx<id> in which 
 * porv is TRUE and FALSE respectively.
 */
struct location *twoWaySplit(struct location *loc, struct PORV* porv, char* sync, struct identifier* vars){
	#ifdef DEBUG_ON 
	printf("[twoWaySplit] STARTED\n");
	#endif
	
	
	//Create a copy of the location
	struct location* negLoc = duplicateLocation(loc);
	#ifdef DEBUG_ON 
	printf("[twoWaySplit] DUPLICATED LOCATION\n");
	#endif
	
	//Change name of the location with PORV true to "name"+id
	sprintf(loc->name,"%s_%d",loc->name,porv->id);
	
	//Change name of the location with PORV false to "name"+"x"+id
	sprintf(negLoc->name,"%s_x%d",negLoc->name,porv->id);
	
	#ifdef DEBUG_ON 
	printf("[twoWaySplit] Altered Location Names\n");
	#endif
	/*
	 * For each incoming transition (u,v)
	 * 	Create a transition to v.PORVID and v.xPORVID
	 */
	#ifdef DEBUG_ON 
	printf("[twoWaySplit] Updating source locations of incoming transitions\n");
	#endif
	
	//For each transition into the location v i.e. (u,v)
	struct transitionWrapper* incoming = loc->incoming;
	while(incoming!=NULL){
		//Update the GOTO name in the transition from (u,old_v) to (u,v)
		strcpy(incoming->trans->gotoLoc,loc->name);
		
		//Add to the transition list of the incoming node
		addToTransitionList(incoming->loc->transitions,incoming->trans->when,incoming->trans->sync,incoming->trans->reset,negLoc->name);
		
		incoming = incoming->next;
	}
	#ifdef DEBUG_ON 
	printf("[twoWaySplit] Incoming location transitions updated, Adding Invariant to loc\n");
	#endif
	
	/*
	 * Adding PORV Invariants
	 */
	#ifdef DEBUG_ON 
	printf("BEFORE %p -> %p\n",porv->porv, porv->porv->next);
	printf("loc->invariant = %p\n",loc->invariant);
	#endif
	
	struct condition* porvCopy = duplicateConditionList(porv->porv);//createCondition(porv->porv->LHS,porv->porv->RHS,porv->porv->op);
	addConditionToList(loc->invariant,porvCopy);
	#ifdef DEBUG_ON 
	printf("AFTER %p -> %p\n",porv->porv, porv->porv->next);
	printf("[twoWaySplit] Invariants added to loc\n");
	
	if(porv->porv->next == porv->porv){
		printf("CAUGHT IT\n");exit(0);
	}
	#endif
	
	struct condition* negPorv = duplicateConditionList(porv->porv);
	
	#ifdef DEBUG_ON 
	printf("[twoWaySplit] Duplicating splitting PORV for negation\n");
	#endif
	
	
	switch(negPorv->op){
		case 1: negPorv->op = 3; break;
		case 3: negPorv->op = 1; break;
		default: printf("ERROR SPLITTING\n");exit(0);
	}
	
	addConditionToList(negLoc->invariant,negPorv);
	
	#ifdef DEBUG_ON 
	printf("[twoWaySplit] Added Negated Invariant\n");
	#endif
	
	/*
	 * Add continuum transitions
	 */
	struct condition* resets = createIdentityResets(vars);
	#ifdef DEBUG_ON 
	printf("[twoWaySplit] Added Resets\n");
	#endif
	
	loc->transitions = addToTransitionList(loc->transitions,negPorv,sync,resets,negLoc->name);
	negLoc->transitions = addToTransitionList(negLoc->transitions,porv->porv,sync,resets,loc->name);
	
	#ifdef DEBUG_ON 
	printf("[twoWaySplit] Updated Transitions\n");
	#endif
	
	//Updating PORV truths for the locations : To later answer the question - does the location satisfy a given PORV
	struct PORV* porvNew = createPORV(porv->porv,porv->id);
	loc->labels = addPORVToList(loc->labels,porvNew);
	
	//Fixing Pointers
	negLoc->next = loc->next;
	loc->next = negLoc;
	
	#ifdef DEBUG_ON 
	printf("[twoWaySplit] Fixed Pointers\n");
	printf("[twoWaySplit] ENDED\n");
	#endif
	
	return loc;
}

/*
 * porv of the form expr == expr
 * Split location loc on PORV porv into l<id>, ll<id> and lg<id> in which 
 * the invariants are respectively, expr == expr, expr <= expr and 
 * expr >=expr
 */
struct location *threeWaySplit(struct location *loc, struct PORV* porv, char* sync, struct identifier* vars){
	//Create Two additional Locations
	struct location* leqLoc = duplicateLocation(loc);
	struct location* geqLoc = duplicateLocation(loc);
	
	//Change the location names
	sprintf(loc->name,"%s_%d",loc->name,porv->id);
	sprintf(leqLoc->name,"%s_l%d",leqLoc->name,porv->id);
	sprintf(geqLoc->name,"%s_g%d",geqLoc->name,porv->id);
	
	/*
	 * For each incoming transition (u,v)
	 * 	Create a transition to v.PORVID and v.xPORVID
	 */
	
	struct transitionWrapper* incoming = loc->incoming;
	while(incoming!=NULL){
		strcpy(incoming->trans->gotoLoc,loc->name);
		addToTransitionList(incoming->loc->transitions,incoming->trans->when,incoming->trans->sync,incoming->trans->reset,leqLoc->name);
		addToTransitionList(incoming->loc->transitions,incoming->trans->when,incoming->trans->sync,incoming->trans->reset,geqLoc->name);
		incoming = incoming->next;
	}
	
	/*
	 * Adding PORV Invariants
	 */
	struct condition* porvCopy = duplicateConditionList(porv->porv);//createCondition(porv->porv->LHS,porv->porv->RHS,porv->porv->op);
	addConditionToList(loc->invariant,porvCopy);
	struct condition* leqPorv = duplicateConditionList(porv->porv);
	struct condition* geqPorv = duplicateConditionList(porv->porv);
	
	leqPorv->op = 1;
	geqPorv->op = 3;
	
	addConditionToList(leqLoc->invariant,leqPorv);
	addConditionToList(geqLoc->invariant,geqPorv);
	
	/*
	 * Add continuum transitions
	 */
	struct condition* resets = createIdentityResets(vars);
	
	loc->transitions = addToTransitionList(loc->transitions,leqPorv,sync,resets,leqLoc->name);
	loc->transitions = addToTransitionList(loc->transitions,geqPorv,sync,resets,geqLoc->name);
	
	leqLoc->transitions = addToTransitionList(leqLoc->transitions,porv->porv,sync,resets,loc->name);
	leqLoc->transitions = addToTransitionList(leqLoc->transitions,geqPorv,sync,resets,geqLoc->name);
	
	geqLoc->transitions = addToTransitionList(geqLoc->transitions,leqPorv,sync,resets,leqLoc->name);
	geqLoc->transitions = addToTransitionList(geqLoc->transitions,porv->porv,sync,resets,loc->name);
	
	//Updating PORV truths for the locations : To later answer the question - does the location satisfy a given PORV
	struct PORV* porvNew = createPORV(porv->porv,porv->id);
	loc->labels = addPORVToList(loc->labels,porvNew);
	
	//Fixing Pointers
	geqLoc->next = loc->next;
	leqLoc->next = geqLoc;	
	loc->next = leqLoc;
	
	return loc;
}

/*
 * Specific Location Splitting:
 * For a PORV of the form I1 <= I2, two way splitting into L1 with I1<=I2 and L2 with I1>=I2
 * For a PORV of the form I1 == I2 three way splitting into L1, L2, and L3 with I1<=I2, I1==I2 and I1>=I2
 * 
 * Returns number of locations split into.
 */
int splitLocation(struct location *loc, struct PORV* porv, char* sync, struct identifier* vars, struct initial* init){
	if(loc!=NULL && porv!=NULL){
		struct initial* initLoc = checkInitial(loc,init);
		if(initLoc != NULL){
			if(porv->porv->op == 0){
				struct initial* initDupL = duplicateInitialConditions(initLoc);
				struct initial* initDupG = duplicateInitialConditions(initLoc);
				
				sprintf(initLoc->init_locs->name,"%s_%d",initLoc->init_locs->name,porv->id);
				sprintf(initDupL->init_locs->name,"%s_l%d",initDupL->init_locs->name,porv->id);
				sprintf(initDupG->init_locs->name,"%s_g%d",initDupG->init_locs->name,porv->id);
				
				initDupG->next = initLoc->next;
				initDupL->next = initDupG;
				initLoc->next = initDupL;
			} else {
				struct initial* initDup = duplicateInitialConditions(initLoc);
				
				sprintf(initLoc->init_locs->name,"%s_%d",initLoc->init_locs->name,porv->id);
				sprintf(initDup->init_locs->name,"%s_x%d",initDup->init_locs->name,porv->id);
				
				initDup->next = initLoc->next;
				initLoc->next = initDup;
			}
		}
		
		/* 
		 * If this is the first split of the location, remember the locations
		 * original Name.
		 */
		if(loc->originalName==NULL){
			loc->originalName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			strcpy(loc->originalName,loc->name);
		}
		
		switch(porv->porv->op){
			case 0: threeWaySplit(loc,porv,sync,vars); break;
			case 1: twoWaySplit(loc,porv,sync,vars); break;
			case 3: twoWaySplit(loc,porv,sync,vars); break;
			default: printf("ERROR SPLITTING\n");exit(0);
		}
		return (porv->porv->op==0)?3:2;
	}
	return 0;
	
}

/*
 * Splitting is either general (for all locations)
 * Or specific for a location
 */
void pSplitTransform(struct automaton* H, struct PORV* porv, char* sync, struct identifier* vars, struct initial* init){
	struct location* locations = H->locations;
	
	struct condition* invariants = NULL;
	struct condition* activity = NULL;
	struct transition* trans = NULL;
	
	struct location* Q = NULL;
	
	while(locations!=NULL){
		Q = locations;
		//Move to the next location, to prevent re-splitting of 
		//an already split location
		locations = locations->next;
		
		//If a location being split is an initial location, then
		//	Delete the location from the initial location list
		//	Add new split locations to the initial location list
		
		
		splitLocation(Q,porv,sync,vars,init);
		processIncomingTransitions(H->locations);
		
		//printf("locations = %p\n",locations);
		
		while(Q!=locations){
			#ifdef DEBUG_ON 
			printf("BEGIN\n");
			#endif
			Q = Q->next;
			
			#ifdef DEBUG_ON 
			printf("Q = %p\n",Q);
			printf("locations = %p\n",locations);
			#endif
			
			if(Q == locations){
				#ifdef DEBUG_ON 
				printf("Im here\n");
				#endif
				
				break;
			}
			#ifdef DEBUG_ON 
			printf("Im here again\n");
			#endif
		}
		#ifdef DEBUG_ON 
		printf("im out\n");
		#endif
		
		if(Q!=locations){
			printf("[pSplitTransform] : ERROR IN SPLITTING\n");
		}
		#ifdef DEBUG_ON 
		printf("I'm really out\n");
		#endif	
	}
}

/*
 * Add $time as a local variable with time'==1 in
 * all locations.
 * 
 * Called when $time is used or when the assertion
 * is temporal in nature. 
 * 
 * $time is added only once, but may be called 
 * multiple times.
 */
void tuneForTemporalProperties(struct phaver* HA){
	
	if(HA->temporal == 0){
		#ifdef DEBUG_ON
		printf("[tuneForTemporalProperties] STARTED\n");
		printf("[tuneForTemporalProperties] Adding $time to the HA\n");
		
		count++;
		if(count>1)
			printf("WHOA!!! I should not be here in any temporal dimension.\n");
		#endif	
		
		char temporal[MAX_STR_LENGTH];
		sprintf(temporal,"time");
		struct identifier* time = createIdentifier(temporal);
		
		sprintf(temporal,"localTime");
		struct identifier* localTime = createIdentifier(temporal);
		
		#ifdef DEBUG_ON
		printf("[tuneForTemporalProperties] Adding TIME as a control variable\n");
		#endif	
		
		addLocalVariable(HA,time,0,1);
		addLocalVariable(HA,localTime,0,1);
		
		#ifdef DEBUG_ON
		printf("[tuneForTemporalProperties] Added TIME\n");
		#endif	
		//A time property has been added
		HA->temporal = 1;
		
		#ifdef DEBUG_ON
		printf("[tuneForTemporalProperties] STARTED\n");
		printf("[tuneForTemporalProperties] Adding $time to the HA\n");
		#endif
		
	}
}

char* getLocationContext(struct PORV* porvConjunct){
	if(porvConjunct!=NULL){
		struct PORV* iterator = porvConjunct;
		char *context = NULL;
		
		//Check for a Location context
		while(iterator!=NULL){
			if(iterator->id == 0){
				//Location context found
				context = iterator->porv->RHS;
				break;
			}
			iterator = iterator->next;
		}
		return context;
	}
	return NULL;
}


struct locationWrapper* splitOnLocationContext(struct phaver* HA, char* context, struct locationWrapper* markedList, struct PORV* porv, char* sync){
	#ifdef DEBUG_ON 
	printf("[splitOnLocationContext] Printing context\n"); fflush(stdout);
	printf("[splitOnLocationContext] Context = %s\n",context); fflush(stdout);
	printf("[splitOnLocationContext] Printing markedlist\n"); fflush(stdout);
	printf("[splitOnLocationContext] markedList = %p\n",markedList);
	#endif
	
	struct location* loc = getUnmarkedLocation(HA->ha->locations, context, markedList);
	
	#ifdef DEBUG_ON 
	printPseudoLocationList(markedList);
	#endif
	
	if(loc != NULL){
		//Atleast one location found
		while(loc!=NULL){
			int splitCount = splitLocation(loc,porv,sync,HA->ha->contr_vars,HA->init);
			processIncomingTransitions(HA->ha->locations);
			
			#ifdef DEBUG_ON 
			printf("[splitOnLocationContext] Marking the original location that was split, now [%s]\n",loc->name);
			#endif
			
			markedList = addToPseudoLocationList(markedList,loc);
			struct location* tempLoc = loc->next;
			while(splitCount>1 && tempLoc!=NULL){
				markedList = addToPseudoLocationList(markedList,tempLoc);
				
				#ifdef DEBUG_ON 
				printf("[splitOnLocationContext] Marking the newly added location after the split, [%s]\n",tempLoc->name);
				printPseudoLocationList(markedList);
				#endif
				
				tempLoc = tempLoc->next;
				splitCount--;
			}
			
			//Get Next Location For Splitting
			loc = getUnmarkedLocation(HA->ha->locations,context, markedList);
			
			#ifdef DEBUG_ON 
			printPseudoLocationList(markedList);
			#endif
		}
		
	} else {
		printf("Feature Error : Could not find the Location: %s\n",context);
	}
	#ifdef DEBUG_ON 
	printf("[splitOnLocationContext] ========== ENDED PORV Split =========\n");
	#endif
	
	return markedList;
}

/*
 * Find location context of the PORV Conjunct
 */
struct phaver* splitPORVConjunct(struct phaver* HA, struct PORV* porvConjunct, char *sync){
	if(porvConjunct!=NULL){
		struct PORV* iterator = porvConjunct;
		struct locationWrapper* markedList = NULL;
		char *context = NULL;
		
		//Check for a Location context
		context = getLocationContext(porvConjunct);
		
		if(context==NULL){
			// No Location Context
			// Split All locations
			iterator = porvConjunct;
			while(iterator!=NULL){
				//For Each PORV, Split All Locations
				pSplitTransform(HA->ha,iterator,sync,HA->ha->contr_vars,HA->init);
				#ifdef DEBUG_ON 
				printf("iterator->next = %p\n",iterator);
				#endif
				
				iterator = iterator->next;
				
			}
			//exit(0);
		} else {
			//Location Context Exists
			// For each PORV
			//	For each location in context
			//		Split all such locations
			iterator = porvConjunct;
			while(iterator!=NULL){
				if(iterator->id > 0){//Split on PORVs only. Their IDs are positive.
					//Get the next location within context, that has not yet been split.
					#ifdef DEBUG_ON 
					printf("[splitPORVConjunct] ========== Next PORV Split =========\n");
					#endif
					markedList = splitOnLocationContext(HA, context, markedList, iterator, sync);
					/*struct location* loc = getUnmarkedLocation(HA->ha->locations, context, markedList);
					 *					
					 *					#ifdef DEBUG_ON 
					 *						printPseudoLocationList(markedList);
					 *					#endif
					 *						
					 *					if(loc != NULL){
					 *						//Atleast one location found
					 *						while(loc!=NULL){
					 *							int splitCount = splitLocation(loc,iterator,sync,HA->ha->contr_vars,HA->init);
					 *							processIncomingTransitions(HA->ha->locations);
					 *							#ifdef DEBUG_ON 
					 *								printf("[splitPORVConjunct] Marking the original location that was split, now [%s]\n",loc->name);
					 *							#endif
					 *							markedList = addToPseudoLocationList(markedList,loc);
					 *							struct location* tempLoc = loc->next;
					 *							while(splitCount>1 && tempLoc!=NULL){
					 *								markedList = addToPseudoLocationList(markedList,tempLoc);
					 *								#ifdef DEBUG_ON 
					 *									printf("[splitPORVConjunct] Marking the newly added location after the split, [%s]\n",tempLoc->name);
					 *									printPseudoLocationList(markedList);
					 *								#endif
					 *								
					 *								tempLoc = tempLoc->next;
					 *								splitCount--;
				}
				
				//Get Next Location For Splitting
				loc = getUnmarkedLocation(HA->ha->locations,context, markedList);
				
				#ifdef DEBUG_ON 
				printPseudoLocationList(markedList);
				#endif
				}
				
				} else {
					printf("Feature Error : Could not find the Location: %s\n",context);
				}
				#ifdef DEBUG_ON 
				printf("[splitPORVConjunct] ========== ENDED PORV Split =========\n");
				#endif
				*/
				}
				freePseudoLocationList(markedList);
				markedList = NULL;
				iterator=iterator->next;
			}
		}
	}
	#ifdef DEBUG_ON 
	printf("[splitPORVConjunct] ENDED\n");
	#endif
	return HA;
}


struct phaver* splitOnEvent(struct phaver* HA, struct sequenceExpr* seq, char *sync){
	//NULL POINTER CHECK
	#ifdef DEBUG_ON 
	printf("[splitOnEvent] STARTED\n");
	#endif
	
	if(HA!=NULL && seq!=NULL && sync!=NULL){
		/*
		 *			struct sequenceExpr{	
		 *				struct expression* expr;	//Sub-sequence Expression
		 *				struct timeDelay* delay;	//Delay associated with expr match
		 *				struct condition* assignments;	//List of Assignments when expr matches within delay
		 *				struct sequenceExpr* next;	//Next sub-expression
	};
	struct PORV{				//List of PORV's conjuncted together
	struct condition* porv;
	int id;
	struct PORV* next;
	};
	
	struct eventType{
	int type;			//Event Type - posedge(1), negedge(2), either(0)
	struct PORV* porv;		//Event Condition
	};
	
	struct PORVExpression{
	struct PORV* conjunct;		//List of PORVs conjuncted together
	struct PORVExpression* next;	//Next PORVConjunct in the Disjunction
	};
	
	struct expression{
	struct PORVExpression* dnf;	//Disjunction of Conjunction of PORVs
	struct eventType* event;	//Event
	};
	*/
		
		struct eventType* event = seq->expr->event;			//Event (Only One allowed per sub expression
		struct PORVExpression* disjunctIterator = seq->expr->dnf;	//Disjunction of PORV conjuncts (Includes possible location Contexts)
		struct locationWrapper* markedList = NULL;
		
		struct identifier* contextList = NULL;
		//Collect location contexts to split
		while(disjunctIterator!=NULL){
			//For each conjunct in the disjunctive normal form, check for a location context
			#ifdef DEBUG_ON 
			printf("[splitOnEvent] Searching for Context.\n");
			#endif
			char* context = getLocationContext(disjunctIterator->conjunct);
			
			if(context!=NULL){
				#ifdef DEBUG_ON 
				printf("[splitOnEvent] Context Found. Adding to List.\n");
				#endif
				
				contextList = addToIdentifierList(contextList,context);
				
				#ifdef DEBUG_ON 
				printf("[splitOnEvent] Context Added.\n");
				#endif
			} else {
				//There Exists atleast one Conjunct with no location context. All Locations should be split
				freeIdentifierList(contextList);
				contextList = NULL;
				break;
			}
			
			#ifdef DEBUG_ON 
			if(context==NULL)
				printf("[splitOnEvent] No Context Found.\n");
			#endif
			//Goto the next disjunction of PORVs
			disjunctIterator = disjunctIterator->next;
		} 
		
		if(contextList==NULL){
			// Split all locations on the event
			#ifdef DEBUG_ON 
			printf("[splitOnEvent] Split all locations on the event.\n");
			#endif
			
			pSplitTransform(HA->ha,event->porv,sync,HA->ha->contr_vars,HA->init);
			
			#ifdef DEBUG_ON 
			printf("[splitOnEvent] Splitting Complete.\n");
			#endif
			
		} else {
			// Split locations within the context list
			#ifdef DEBUG_ON 
			printf("[splitOnEvent] Split on context.\n");
			#endif
			
			struct identifier* idIterator = contextList;
			if(event!=NULL){
				while(idIterator!=NULL){
					//Get next location in context and split it on the event
					//printf("HERE Event is %d\n",event==NULL?0:1);
					markedList = splitOnLocationContext(HA, idIterator->name, markedList, event->porv, sync);
					
					//printf("HERE\n");
					freePseudoLocationList(markedList);
					markedList = NULL;
					idIterator = idIterator->next;
				}
			}
		} 
		
	}
	
	#ifdef DEBUG_ON 
	printf("[splitOnEvent] ENDED\n");
	#endif
	return HA;
}

/* 
 * struct condition* generateTemporalGuards(struct sequenceExpr* seq)
 * Creates a temporal guard. Given [lower:upper], generates lower<=time, time <= upper
 */
struct condition* generateTemporalGuards(struct sequenceExpr* seq){
	if(seq!=NULL){
		if(seq->delay!=NULL){
			struct condition* temporalGuards = NULL;
			char* lower = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			char* upper = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			switch(seq->delay->type){
				case 0: //Finite lower and upper bounds;
					sprintf(lower,"%f",seq->delay->lower);
					sprintf(upper,"%f",seq->delay->upper);
					temporalGuards = addToConditionList(temporalGuards,lower,"localTime",1);
					temporalGuards = addToConditionList(temporalGuards,"localTime",upper,1);
					break;
					
				case 1: //Unconstrained upper bounds : upper == 0
					sprintf(lower,"%f",seq->delay->lower);
					temporalGuards = addToConditionList(temporalGuards,lower,"localTime",1);
					break;
					
				case 2: //Exact Finite Time
					sprintf(lower,"%f",seq->delay->lower);
					temporalGuards = addToConditionList(temporalGuards,lower,"localTime",0);
					break;
					
				default: printf("[generateTemporalGuards] ERROR\n");exit(0);
			}
			return temporalGuards;
		} 
		return NULL;
	}
	return NULL;
}

/*
 * struct condition* generateGuard(struct PORV* porv){
 *	if(porv!=NULL){
 *		struct condition* guard = NULL;
 *		guard = createCondition(porv->porv->LHS,porv->porv->RHS,porv->porv->op);
 *		return guard;
 *	}
 *	return NULL;
 * }
 */

struct condition* generateEventGuard(eventType* event, condition* derivative){
	#ifdef DEBUG_ON 
	printf("[generateEventGuard] STARTED\n");
	#endif
	
	if(event!=NULL){
		struct condition* guard = NULL;
		guard = createCondition(event->porv->porv->LHS,event->porv->porv->RHS,0);
		if(derivative!=NULL){
			struct condition* iterator = derivative;
			while(derivative){
				#ifdef DEBUG_ON 
				printf("[generateEventGuard] Event on Variable [%s]\n",derivative->LHS);
				#endif
				
				if(strncmp(event->porv->porv->LHS,derivative->LHS,strlen(derivative->LHS)-1)==0){
					int op = 0;
					//1 posedge
					//2 negedge
					//0 either
					//printf("EVENT TYPE: %d\n", event->type);
					
					switch(event->type){
						case 1:	if(event->porv->porv->op == 0 || \
							event->porv->porv->op == 3 || \
							event->porv->porv->op == 5){
								op = 5;
							} else 	op = 4;
							break;
							
						case 2: if(event->porv->porv->op == 0 || \
							event->porv->porv->op == 3 || \
							event->porv->porv->op == 5){
								op = 4;
							} else 	op = 5;
							break;
							
						default: op = 0;
					}
					//printf("OP = %d\n");
					if(op>0)
						guard = addToConditionList(guard,derivative->RHS,"0",op);
					
					break;
				}
				derivative = derivative->next;
			}
		}
		#ifdef DEBUG_ON 
		printf("[generateEventGuard] STARTED\n");
		#endif
		return guard;
	}
	#ifdef DEBUG_ON 
	printf("[generateEventGuard] STARTED\n");
	#endif
	return NULL;
}

struct condition* generateConjunctGuards(struct PORV* porv){
	if(porv!=NULL){
		struct condition* guard = NULL;
		while(porv!=NULL){
			if(porv->id>0)
				guard = addConditionToList(guard,createCondition(porv->porv->LHS,porv->porv->RHS,porv->porv->op));
			porv = porv->next;
		}
		return guard;
	}
	return NULL;
}

int countSequenceExpressions(struct sequenceExpr* seq_expr){
	int expr_count = 0;
	
	while(seq_expr!=NULL){
		expr_count++;
		seq_expr = seq_expr->next;
	}
	return expr_count;
}

void markDelayForFinal(struct condition* activity){
	while(activity){
		if(strcasecmp(activity->LHS,"time'")==0){
			sprintf(activity->RHS,"1");
		}
		activity = activity->next;
	}
}


void markOriginalTransitions(struct location* locIterator){
	struct transition* transIterator = NULL;
	
	while(locIterator!=NULL){
		transIterator = locIterator->transitions;
		while(transIterator!=NULL){
			transIterator->original = 1;
			transIterator = transIterator->next;
		}
		locIterator = locIterator->next;
	}
}

struct condition* generateAssignmentResets(struct condition* assignments){
	#ifdef DEBUG_ON 
	printf("[generateAssignmentResets] STARTED\n");
	#endif
	
	if(assignments!=NULL){
		struct condition* root = NULL;
		struct condition* iterator = assignments;
		
		while(iterator!=NULL){
			char* LHS = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			char* RHS = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			
			#ifdef DEBUG_ON 
			printCondition(iterator);printf("\n");
			#endif
			
			sprintf(LHS,"%s'",iterator->LHS);
			
			if(strcmp(iterator->RHS,"$time")==0){
				sprintf(RHS,"time");
			} else strcpy(RHS,iterator->RHS);
			
			#ifdef DEBUG_ON
			printf("CONDITION ADDED IS : %s == %s\n",LHS,RHS);
			#endif
			
			root = addToConditionList(root,LHS,RHS,0);
			
			iterator = iterator->next;
		}
		#ifdef DEBUG_ON 
		printf("[generateAssignmentResets] ENDED\n");
		#endif
		
		return root;
	} 
	#ifdef DEBUG_ON 
	printf("[generateAssignmentResets] ENDED\n");
	#endif
	return NULL;
	
}

int countConditions(struct condition* root){
	#ifdef DEBUG_ON 
	printf("[countConditions] STARTED\n");
	#endif
	
	struct condition* iterator = root;
	int count = 0;
	while(iterator!=NULL){
		count++;
		iterator = iterator->next;
	}
	
	#ifdef DEBUG_ON 
	printf("[countConditions] ENDED\n");
	#endif
	
	return count;
}

struct condition* createZeroDerivative(struct identifier* vars){
	#ifdef DEBUG_ON 
	printf("[createZeroDerivative] STARTED\n");
	#endif
	
	if(vars!=NULL){
		#ifdef DEBUG_ON
		printf("[createZeroDerivative] vars!=NULL\n");
		#endif
		
		struct identifier* iterator = vars;
		struct condition* root = NULL;
		char* LHS = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
		char* RHS = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
		
		while(iterator!=NULL){
			sprintf(LHS,"%s'",iterator->name);
			sprintf(RHS,"0");
			root = addToConditionList(root,LHS,RHS,0);
			
			iterator = iterator->next;
		}
		free(LHS);
		free(RHS);
		
		#ifdef DEBUG_ON 
		printf("[createZeroDerivative] ENDED\n");
		#endif
		
		return root;
	}
	#ifdef DEBUG_ON 
	printf("[createZeroDerivative] vars NULL ENDED\n");
	#endif
	return NULL;
}

void orderedResets(int ExprID, int dnfID, struct automaton* H, struct location* loc, struct transition* trans, struct condition* resets, struct identifier* vars){
	#ifdef DEBUG_ON 
	printf("[orderedResets] BEGIN\n");
	#endif
	
	if(loc!=NULL && trans!=NULL){
		int resetCount = resets?countConditions(resets):0;
		struct condition* iterator = resets;
		int id=1;
		
		#ifdef DEBUG_ON
		printf("[orderedResets] Condition List:\n");
		
		/*struct condition* tempC = resets ;
		 *			while(tempC!=NULL){
		 *				printCondition(tempC);printf(" , ");
		 *				tempC=tempC->next;
	}*/
		printConditionList(resets);
		printf("\n");
		#endif
		
		if(resetCount>1){
			#ifdef DEBUG_ON 
			printf("[orderedResets] Ordering Required - More than 1 assignment present\n");
			#endif
			char* name = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			char* gotoName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			
			while(iterator!=NULL){
				sprintf(name,"%sS%dD%dR%d",loc->name, ExprID, dnfID, id-1);
				sprintf(gotoName,"%sS%dD%dR%d",loc->name, ExprID, dnfID, id);
				struct condition* whileInvariant = duplicateConditionList(loc->invariant);
				struct condition* derivative = createZeroDerivative(vars);
				
				/*
				 *				printf("[orderedResets] Created Derivative list:\n");
				 *				struct condition* tempC = derivative ;
				 *				while(tempC!=NULL){
				 *					printCondition(tempC);printf(" && ");
				 *					tempC=tempC->next;
			}
			*/
				
				struct condition* reset = createCondition(iterator->LHS,iterator->RHS,0);
				struct condition* resetList = NULL;
				if(id==resetCount){
					struct condition* condIterator = trans->reset;
					/*if(strcmp(condIterator->LHS,reset->LHS)==0){
					 *						trans->reset= condIterator->next;
				} else {
					while(condIterator->next!=NULL){
						if(strcmp(condIterator->next->LHS,reset->LHS)==0){
							condIterator->next = condIterator->next->next;
							break;
				}
				condIterator = condIterator->next;
				}
				}*/
					while(condIterator!=NULL){
						if(strcmp(condIterator->LHS,reset->LHS)==0){
							strcpy(condIterator->RHS,reset->RHS);
							break;
						}
						condIterator = condIterator->next;
					}
					
				} else {
					resetList = createIdentityResets(H->contr_vars);
					struct condition* condIterator = resetList;
					/*if(strcmp(condIterator->LHS,reset->LHS)==0){
					 *	strcpy(condIterator->RHS,reset->RHS);
					 *	//resetList = condIterator->next;
				} else {
					while(condIterator->next!=NULL){
						if(strcmp(condIterator->next->LHS,reset->LHS)==0){
							strcpy(condIterator->next->RHS,reset->RHS);
							//condIterator->next = condIterator->next->next;
							break;
				}
				condIterator = condIterator->next;
				}
				}*/
					
					while(condIterator!=NULL){
						if(strcmp(condIterator->LHS,reset->LHS)==0){
							strcpy(condIterator->RHS,reset->RHS);
							break;
						}
						condIterator = condIterator->next;
					}
					
					
				} 
				
				#ifdef DEBUG_ON
				printf("RESET IS : ");printCondition(reset);printf("\n");
				#endif
				
				if(id==1){
					//Starting Location to next location
					loc->transitions = addToTransitionList(loc->transitions,duplicateConditionList(trans->when),trans->sync,resetList,gotoName);
				} else if(id<resetCount){
					//Intermediate locations
					struct transition* dummyTrans = createTransition(duplicateConditionList(trans->when),trans->sync,resetList,gotoName);
					H->locations = addToLocationList(H->locations,name,whileInvariant,derivative,dummyTrans,1);
				} else {//Last Location
					//trans->reset = addConditionToList(trans->reset,reset);
					H->locations = addToLocationList(H->locations,name,whileInvariant,derivative,trans,1);
				}
				id++;
				iterator = iterator->next;
			}
		} else {
			#ifdef DEBUG_ON 
			printf("[orderedResets] Ordering Not Required.\n");
			#endif
			
			if(resetCount==1){
				struct condition* reset = createCondition(iterator->LHS,iterator->RHS,0);
				
				struct condition* condIterator = trans->reset;
				
				while(condIterator!=NULL){
					if(strcmp(condIterator->LHS,reset->LHS)==0){
						strcpy(condIterator->RHS,reset->RHS);
						break;
					}
					condIterator = condIterator->next;
				}
				
				
				#ifdef DEBUG_ON
				printf("RESET IS : ");printCondition(reset);printf("\n");
				#endif	
				
			}
			
			//trans->reset = addConditionToList(trans->reset,reset);
			loc->transitions = addTransitionToList(loc->transitions,trans);
		} 
		
	}
	#ifdef DEBUG_ON 
	printf("[orderedResets] ENDED\n");
	#endif
}

void resetToZero(struct condition* assignmentResets,char* varName){
	//printf("********************************************* HERE ***************************************************\n");
	if(varName!=NULL){
		/*char* resetName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
		 *		sprintf(resetName,"%s'",varName);
		 */
		while(assignmentResets){
			if(strncasecmp(assignmentResets->LHS,varName,strlen(varName))==0){
				sprintf(assignmentResets->RHS,"0");
			}
			//printf("********************************************* [%s] ***************************************************\n",assignmentResets->LHS);
			assignmentResets = assignmentResets->next;
		}
	}
}

//Determines if the event is on the discrete state variable
int isStateEvent(struct eventType* event){
	#ifdef DEBUG_ON 
	printf("[isStateEvent] STARTED\n");
	#endif
	
	if(event && event->porv){
		if(strcasecmp(event->porv->porv->LHS,"state")==0){
			#ifdef DEBUG_ON 
			printf("[isStateEvent] ENDED: Is a state based event\n");
			#endif
			return 1;
		} else {
			#ifdef DEBUG_ON 
			printf("[isStateEvent] ENDED: NOT a state based event\n");
			#endif
			return 0;
		}
	}
	
	#ifdef DEBUG_ON 
	printf("[isStateEvent] ENDED - Something is NULL\n");
	#endif
}


/* 
 * This function computes the product automaton that is key to Feature Analysis
 */
struct phaver* levelSequence(struct phaver* H, struct feature* F, char* syncLabel){
	#ifdef DEBUG_ON 
	printf("[levelSequence] BEGIN\n");
	#endif
	
	if(H!=NULL && F!=NULL){
		//Initial all states start with level == 0 
		int id = 0;
		int nextId = 0;	
		char* strID = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
		
		struct sequenceExpr* seq_expr = F->def->seq;//seq_expr <--- feature sequence expression
		
		int expr_count = countSequenceExpressions(seq_expr);//expr_count <--- Number of subsequences in the sequence expression
		
		#ifdef DEBUG_ON 
		printf("[levelSequence] Sequence Expression Count = [%d]\n",expr_count);
		//Now we mark transitions that are of the original automaton i.e. level 0
		printf("[levelSequence] Marking Transitions of level 0 as ORIGINAL\n");
		#endif
		
		struct location* locIterator = H->ha->locations;
		struct transition* transIterator = NULL;
		
		markOriginalTransitions(H->ha->locations);
		
		//Synchronization label for level switching is syncLabel
		
		//Level Sequencing starts here.
		
		#ifdef DEBUG_ON 
		printf("[levelSequence] Sequencing Begins\n");
		#endif
		
		//ADDING FINAL LOCATION
		char* name = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
		sprintf(name,"FINAL");
		struct condition* whileInvariant = createCondition(NULL,NULL,-1);
		struct condition* derivative = createZeroDerivative(H->ha->contr_vars);
		markDelayForFinal(derivative);
		H->ha->locations = addToLocationList(H->ha->locations,name,whileInvariant,derivative,NULL,1);
		
		whileInvariant = NULL;
		derivative = NULL;
		
		//Get the list of Sequence Expressions - seq_expr
		seq_expr = F->def->seq;
		
		/*
		 * FOR EACH SUB-SEQUENCE EXPRESSION : 
		 * 	1. temporalGuards : common guards for temporal constraints
		 * 	2. Check if there is an event - create common eventGuards and set event type (eType)
		 * 	3. temporalResets : Generate temporal resets.
		 * 	4. levelGuards and levelResets : Generate common level guards and resets.
		 * 	5. Iterate through all locations and modify transitions associated with the location.
		 */
		while(seq_expr!=NULL){
			
			//id = Position of the subexpression
			nextId = ((id+1 == expr_count)? 0 : id+1);
			
			// Generating Temporal Guards
			struct condition* temporalGuards = generateTemporalGuards(seq_expr);
			
			// Generate Event Guards
			int eType = -1;
			int stateEvent = 0; // Either these is no event associated with the subexpression or the event isn't over the discrete state variable.
			char* eventContext = NULL;
			struct condition* eventGuards = NULL;
			
			struct PORVExpression* firstMatchExprs = NULL;
			struct PORVExpression* nonFirstMatchExprs = NULL;
			struct PORVExpression* dnf = seq_expr->expr->dnf;
			
			while(dnf){
				struct PORVExpression* conjunctTerm = createPORVExpression(duplicatePORVList(dnf->conjunct));
				if(dnf->firstMatch){
					firstMatchExprs = addPORVExpressionToEOfList(firstMatchExprs,conjunctTerm);
				} else {
					nonFirstMatchExprs = addPORVExpressionToEOfList(nonFirstMatchExprs,conjunctTerm);
				}
				dnf=dnf->next;
			}
			
			if(seq_expr->expr->event!=NULL){
				/* 
				 * Check Event Type
				 * Check on which variable the PORV is defined (This assumes each PORV has one variable on the LHS)
				 *
				 * If it is a state event, then all transitions 
				 * at this level to the next level entering 
				 * or leaving the state have to be treated 
				 * appropriately.
				 */
				stateEvent = isStateEvent(seq_expr->expr->event);
				eventContext = stateEvent?seq_expr->expr->event->porv->porv->RHS:NULL;
				
				//If it's a non-state event then prepare an event guard
				//printf("\nHERE\n");
				//printf("\n[%p]\n",seq_expr->expr->event);
				
				eventGuards = stateEvent?NULL:generateEventGuard(seq_expr->expr->event,NULL);
				
				eType = seq_expr->expr->event->type;
				
			}
			
			// Level Guards
			sprintf(strID,"%d",id);
			struct condition* levelGuard = createCondition("level",strID,0);
			
			// Generate Conjunct Guards for PORVs in the conjunct
			struct condition* conjunctGuards = NULL;
			
			// Generate Resets
			sprintf(strID,"%d",nextId);
			struct condition* levelResets = createCondition("level'", strID, 2);
			
			struct condition* temporalResets = NULL;
			
			if(H->temporal==1){
				temporalResets = addToConditionList(temporalResets,"time'","time",2);
				sprintf(strID,"%d",0);
				temporalResets = addToConditionList(temporalResets,"localTime'",strID,2);
			}
			
			struct condition* assignmentResets = NULL;//seq_expr->assignments;
			
			struct condition* featureAssignments = generateAssignmentResets(F->def->featureAssign);
			
			/* 
			 * For each location Q either the locations satisfies 
			 * the sub-expression or not. 
			 * 
			 * If the location satisfies the subexpression, transitions
			 * are added at this location to the next level.
			 * 
			 * For locations satisfying and having non-first match constraints
			 * transitions are added to remain within the level.
			 * 
			 * For location p, if there is an event applicable on q, then
			 * there are the following three possibilities:
			 * @+(q) : Transitions from p to q are the ones on which 
			 *         the level changes. Other transitions are deleted. 
			 * @-(q) : Transitions from q to p are the ones on which
			 *         the level changes. Other transitions from q are deleted.
			 * @(q)  : Transition from p to q, and from q to p are ones
			 *         on which level changes. Others are deleted.
			 */
			locIterator = H->ha->locations;
			while(locIterator!=NULL){
				#ifdef DEBUG_ON
				printf("\n");
				printf("\n[levelSequence] LEVEL [%d] : Location: %s :  Location Truth = %d\n", id, locIterator->name, isTrueExpr(locIterator,seq_expr->expr));
				#endif
				
				//For non-pause locations, skip all pause locations
				if(locIterator->type==1){ locIterator=locIterator->next; continue;}
				eventGuards = stateEvent?NULL:generateEventGuard(seq_expr->expr->event,locIterator->derivative);
				//if(isTrueExpr(locIterator,seq_expr->expr)==1){//REMOVED AS PART OF NO SPLIT change
				//The Location Satisfies the expression
				//ENHANCEMENT: FOR WHICH CONJUNCTS IS THE EXPRESSION TRUE
				
				#ifdef DEBUG_ON 
				printf("[levelSequence] [EXPR-MATCH]Location [%s] at Level [%d]\n",locIterator->name,id);
				#endif
				
				struct PORVExpression* dnf = seq_expr->expr->dnf;				
				int dnfID = 0;
				while(dnf!=NULL){
					/*
					 * For each conjunct create one transition:
					 * 1. Get the locationContext.
					 * 2. The conjunct is applicable at this location if
					 * 		A. There is no location context.
					 * 		B. There is a location context, and 
					 * 		   the location context is of this location.
					 */
					dnfID++;
					struct transition* trans = NULL;
					
					#ifdef DEBUG_ON 
					printf("[levelSequence] [EXPR-MATCH]Generating Transition for \n");
					printPORVExpression(dnf);printf("-\n");
					#endif
					
					//Get Location Context
					char* context = getLocationContext(dnf->conjunct);
					#ifdef DEBUG_ON
					if(context!=NULL){ 
						printf("Context = [%s]\n",context);
						printf("locIterator = %p\n",locIterator);
						printf("original Name");fflush(stdout);printf(" = %d\n", locIterator->originalName);
						if(locIterator->originalName!=NULL){
							printf("Original Name is not null = %p\n",locIterator->originalName);
							printf("locIterator = %p\n",locIterator);
							printf("original Name");fflush(stdout);printf(" = %s\n", locIterator->originalName);
							printf("strcmp([%s],[%s])=%d\n",context,locIterator->originalName,strcmp(context,locIterator->originalName));
						}
					}
					
					printf("%d\n",(context!=NULL && (locIterator->originalName!=NULL?(strcmp(context,locIterator->originalName)==0):(strcmp(context,locIterator->name)==0))));
					printf("NAME = %s\n",locIterator->name);
					#endif
					
					
					if((context == NULL) || (context!=NULL && strcmp(context,locIterator->name)==0)){ //(locIterator->originalName!=NULL?(strcmp(context,locIterator->originalName)==0):(strcmp(context,locIterator->name)==0)))){
						//If there is a location context for the formula, then consider the context
						#ifdef DEBUG_ON 
						printf("[levelSequence] No Location Context OR Context Match \n");
						printf("[levelSequence] Creating Transition\n");
						fflush(stdout);
						#endif
						
						/*
						 * Generate Guards:
						 * 	1. Common Temporal Guards
						 * 	2. Common Event Guards
						 * 	3. Guards for the conjunct
						 * 	4. Common Level Guards 
						 */
						struct condition* when = duplicateConditionList(temporalGuards);
						
						//Add Event Guard
						if(!stateEvent){
							when = addConditionToList(when,duplicateConditionList(eventGuards));
						}
						
						//Add Conjunct Guard
						if(dnf->conjunct!=NULL)
							conjunctGuards = generateConjunctGuards(dnf->conjunct);
						when = addConditionToList(when,conjunctGuards);
						
						//Add Level Guard
						when = addConditionToList(when,levelGuard);
						
						/*
						 * If there is a state Q based event, then on all transitions 
						 * from P to Q, the event is applicable, if it is a posedge
						 * otherwise if it is a negedge it is applicable if P==Q.
						 * Event Type - posedge(1), negedge(2), either(0)
						 */
						if(stateEvent){
							//A Posedge Transition entering location Q = "eventContext" from the current location
							if(eType == 1 || eType == 0){
								struct transition* transIterator = locIterator->transitions;
								while(transIterator){
									//If not an original transition skip
									if(!transIterator->original){transIterator = transIterator->next; continue;}
									
									//If transition is not to location Q skip it
									if(strcmp(transIterator->gotoLoc,eventContext)!=0){transIterator = transIterator->next; continue;}			
									else{
										//If it is to Q then make a copy and add on all the other feature luggage onto it
										struct transition* posTrans = duplicateTransision(transIterator);
										posTrans->next = NULL;
										posTrans->original = 0;
										posTrans->when = addConditionToList(posTrans->when,duplicateConditionList(when));
										removeConditionFromList(&posTrans->reset,"level");
										posTrans->reset = addConditionToList(posTrans->reset,duplicateConditionList(levelResets));
										
										//Order Resets for assignments
										assignmentResets = generateAssignmentResets(seq_expr->assignments);
										assignmentResets = reverseConditionList(assignmentResets);
										
										struct condition* allResets = duplicateConditionList(temporalResets);
										posTrans->reset = addConditionToList(posTrans->reset,allResets);
										
										if(id==expr_count-1){
											//Feature Computation
											assignmentResets = addConditionToList(assignmentResets,featureAssignments);
											char varName[MAX_STR_LENGTH] = "time";
											resetToZero(posTrans->reset,varName);
											
											strcpy(posTrans->gotoLoc,name);
										}
										
										//Add the transition to the location's transition list
										orderedResets(id,dnfID,H->ha,locIterator,posTrans,assignmentResets,H->ha->contr_vars);
										assignmentResets = NULL;
										
										posTrans = NULL; //Just to be safe :)
									}
									
									transIterator = transIterator->next;
								}
							}
							
							//If its a negedge or an anyedge and this is location Q (the current location is the eventLocation), we want to exit this location on the negedge
							if((eType == 2 || eType == 0) && strcmp(locIterator->name,eventContext)==0){
								struct transition* transIterator = locIterator->transitions;
								while(transIterator){
									//If not an original transition skip
									if(!transIterator->original){transIterator = transIterator->next; continue;}
									
									//If it is an original transition
									struct transition* negTrans = duplicateTransision(transIterator);
									negTrans->next = NULL;
									negTrans->original = 0;
									negTrans->when = addConditionToList(negTrans->when,duplicateConditionList(when));
									removeConditionFromList(&negTrans->reset,"level");                                                                                       
									negTrans->reset = addConditionToList(negTrans->reset,duplicateConditionList(levelResets));
									
									//Order Resets for assignments
									assignmentResets = generateAssignmentResets(seq_expr->assignments);
									assignmentResets = reverseConditionList(assignmentResets);
									
									struct condition* allResets = duplicateConditionList(temporalResets);
									negTrans->reset = addConditionToList(negTrans->reset,allResets);
									
									if(id==expr_count-1){
										//Feature Computation
										assignmentResets = addConditionToList(assignmentResets,featureAssignments);
										char varName[MAX_STR_LENGTH] = "time";
										resetToZero(negTrans->reset,varName);
										
										strcpy(negTrans->gotoLoc,name);
									}
									
									//Add the transition to the location's transition list
									orderedResets(id,dnfID,H->ha,locIterator,negTrans,assignmentResets,H->ha->contr_vars);
									assignmentResets = NULL;
									
									negTrans = NULL; //Just to be safe :)
									
									transIterator = transIterator->next;
								}
							}
						} else {
							
							#ifdef DEBUG_ON 
							if(when!=NULL){
								printf("[levelSequence] When Clause is as follows:\n");printConditionList(when);
							} else	
								printf("[levelSequence] When Clause is Null\n");
							
							#endif
							
							trans = createTransitionOnEvent(when,syncLabel,duplicateConditionList(levelResets),locIterator->name,H->ha->contr_vars);
							
							#ifdef DEBUG_ON 
							if(trans!=NULL)
								printf("[levelSequence] Transition Created\n");
							else	
								printf("[levelSequence] Transition NOT Created\n");
							
							#endif
							
							//Order Resets for assignments
							assignmentResets = generateAssignmentResets(seq_expr->assignments);
							#ifdef DEBUG_ON 
							printf("[levelSequence] Assignment Resets from Sub-expression  =  %p\n",assignmentResets);
							printConditionList(assignmentResets);
							#endif
							
							assignmentResets = reverseConditionList(assignmentResets);
							
							#ifdef DEBUG_ON 
							printf("[levelSequence] Reversing Assignment Resets =  %p\n",assignmentResets);
							printConditionList(assignmentResets);
							#endif
							
							struct condition* allResets = duplicateConditionList(temporalResets);
							trans->reset = addConditionToList(trans->reset,allResets);
							//assignmentResets = addConditionToList(assignmentResets,allResets);
							#ifdef DEBUG_ON 
							printf("\n[levelSequence] [HERE] id = %d  expr_count = %d\n",id, expr_count);
							#endif
							
							if(id==expr_count-1){
								//Feature Computation
								assignmentResets = addConditionToList(assignmentResets,featureAssignments);
								char varName[MAX_STR_LENGTH] = "time";
								resetToZero(trans->reset,varName);
								/*      
								 *									char* name = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
								 *									//char* gotoName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
								 *									sprintf(name,"FINAL");
								 *									//sprintf(gotoName,"%sS%dR%d",loc->name, ExprID, id);
								 *									struct condition* whileInvariant = duplicateConditionList(locIterator->invariant);
								 *									struct condition* derivative = createZeroDerivative(H->ha->contr_vars);
								 *									//struct condition* resetList = createIdentityResets(H->ha->contr_vars);
								 *									H->ha->locations = addToLocationList(H->ha->locations,name,whileInvariant,derivative,NULL);
								 */
								strcpy(trans->gotoLoc,name);
							}
							#ifdef DEBUG_ON
							printf("[levelSequence] All Assignment Resets  =  %p\n",assignmentResets);
							printConditionList(assignmentResets);
							#endif
							
							orderedResets(id,dnfID,H->ha,locIterator,trans,/*addConditionToList(assignmentResets,duplicateConditionList(temporalResets))*/\
							assignmentResets,H->ha->contr_vars);
							assignmentResets = NULL;
							//Add the transition to the location's transition list
							
							#ifdef DEBUG_ON
							printConditionList(featureAssignments);
							printConditionList(trans->reset);
							#endif
							
							#ifdef DEBUG_ON
							printf("[levelSequence] The transition is original = %d\n",trans->original);
							#endif
							//locIterator->transitions = addTransitionToList(locIterator->transitions, trans);
							trans = NULL; //Just to be safe :)
							
						}
					}
					
					//Check the next conjunct
					dnf = dnf->next;
				}
				
				if(seq_expr->expr->dnf==NULL){
					//No PORVs only the event
					#ifdef DEBUG_ON 
					printf("---------------- DNF IS NULL --------------\n");
					#endif
					
					if(stateEvent){
						//A Posedge Transition entering location Q = "eventContext" from the current location
						if(eType == 1 || eType == 0){
							struct transition* transIterator = locIterator->transitions;
							while(transIterator){
								//If not an original transition skip
								if(!transIterator->original){transIterator = transIterator->next; continue;}
								
								//If transition is not to Q skip
								if(strcmp(transIterator->gotoLoc,eventContext)!=0){transIterator = transIterator->next; continue;}			
								else{
									//If it is to Q then make a copy and add on all the other feature luggage onto it
									struct transition* posTrans = duplicateTransision(transIterator);
									posTrans->next = NULL;
									posTrans->original = 0;
									
									struct condition* when = duplicateConditionList(temporalGuards);
									when = addConditionToList(when,duplicateConditionList(levelGuard));
									
									posTrans->when = addConditionToList(posTrans->when,duplicateConditionList(when));
									removeConditionFromList(&posTrans->reset,"level");
									posTrans->reset = addConditionToList(posTrans->reset,duplicateConditionList(levelResets));
									
									//Order Resets for assignments
									assignmentResets = generateAssignmentResets(seq_expr->assignments);
									assignmentResets = reverseConditionList(assignmentResets);
									
									struct condition* allResets = duplicateConditionList(temporalResets);
									posTrans->reset = addConditionToList(posTrans->reset,allResets);
									
									if(id==expr_count-1){
										//Feature Computation
										assignmentResets = addConditionToList(assignmentResets,featureAssignments);
										char varName[MAX_STR_LENGTH] = "time";
										resetToZero(posTrans->reset,varName);
										
										strcpy(posTrans->gotoLoc,name);
									}
									
									//Add the transition to the location's transition list
									orderedResets(id,dnfID,H->ha,locIterator,posTrans,assignmentResets,H->ha->contr_vars);
									assignmentResets = NULL;
									
									posTrans = NULL; //Just to be safe :)
								}
								
								transIterator = transIterator->next;
							}
						}
						
						//If its a negedge or an anyedge and this is location Q (the current location is the eventLocation), we want to exit this location on the negedge
						if((eType == 2 || eType == 0) && strcmp(locIterator->name,eventContext)==0){
							struct transition* transIterator = locIterator->transitions;
							while(transIterator){
								//If not an original transition skip
								if(!transIterator->original){transIterator = transIterator->next; continue;}
								
								//If it is an original transition
								struct transition* negTrans = duplicateTransision(transIterator);
								negTrans->next = NULL;
								negTrans->original = 0;
								
								struct condition* when = duplicateConditionList(temporalGuards);
								when = addConditionToList(when,duplicateConditionList(levelGuard));
								
								negTrans->when = addConditionToList(negTrans->when,duplicateConditionList(when));
								removeConditionFromList(&negTrans->reset,"level");                                                                                       
								negTrans->reset = addConditionToList(negTrans->reset,duplicateConditionList(levelResets));
								
								//Order Resets for assignments
								assignmentResets = generateAssignmentResets(seq_expr->assignments);
								assignmentResets = reverseConditionList(assignmentResets);
								
								struct condition* allResets = duplicateConditionList(temporalResets);
								negTrans->reset = addConditionToList(negTrans->reset,allResets);
								
								if(id==expr_count-1){
									//Feature Computation
									assignmentResets = addConditionToList(assignmentResets,featureAssignments);
									char varName[MAX_STR_LENGTH] = "time";
									resetToZero(negTrans->reset,varName);
									
									strcpy(negTrans->gotoLoc,name);
								}
								
								//Add the transition to the location's transition list
								orderedResets(id,dnfID,H->ha,locIterator,negTrans,assignmentResets,H->ha->contr_vars);
								assignmentResets = NULL;
								
								negTrans = NULL; //Just to be safe :)
								
								transIterator = transIterator->next;
							}
						}
					} else {
						if(seq_expr->expr->event!=NULL){
							
							struct condition* when = duplicateConditionList(temporalGuards);
							if(!stateEvent){
								when = addConditionToList(when,duplicateConditionList(eventGuards));
							}
							when = addConditionToList(when,duplicateConditionList(levelGuard));
							
							struct transition* trans = createTransitionOnEvent(when, syncLabel, duplicateConditionList(levelResets), locIterator->name, H->ha->contr_vars);
							
							//Order Resets for assignments
							assignmentResets = generateAssignmentResets(seq_expr->assignments);
							
							#ifdef DEBUG_ON 
							printf("Assignment Resets  =  %p\n",assignmentResets);
							printConditionList(assignmentResets);
							#endif
							
							assignmentResets = reverseConditionList(assignmentResets);
							
							#ifdef DEBUG_ON 
							printf("Assignment Resets after reversing  =  %p\n",assignmentResets);
							printConditionList(assignmentResets);
							#endif
							
							if(id==expr_count-1){
								//Feature Computation
								assignmentResets = addConditionToList(assignmentResets,featureAssignments);
								/*
								 *									char* name = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
								 *									//char* gotoName = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
								 *									sprintf(name,"FINAL");
								 *									//sprintf(gotoName,"%sS%dR%d",loc->name, ExprID, id);
								 *									struct condition* whileInvariant = duplicateConditionList(locIterator->invariant);
								 *									struct condition* derivative = createZeroDerivative(H->ha->contr_vars);
								 *									//struct condition* resetList = createIdentityResets(H->ha->contr_vars);
								 *									H->ha->locations = addToLocationList(H->ha->locations,name,whileInvariant,derivative,NULL);
								 */
								strcpy(trans->gotoLoc,name);
							}
							
							#ifdef DEBUG_ON
							printf("Assignment Resets  =  %p\n",assignmentResets);
							
							printf("------------------------TRANS GOTO = %s\n",trans->gotoLoc);
							#endif
							
							struct condition* allResets = duplicateConditionList(temporalResets);
							trans->reset = addConditionToList(trans->reset,allResets);
							orderedResets(id,dnfID, H->ha,locIterator,trans,/*addConditionToList(assignmentResets,duplicateConditionList(temporalResets))*/assignmentResets,H->ha->contr_vars);
							
							assignmentResets = NULL;
							#ifdef DEBUG_ON
							printConditionList(trans->reset);
							#endif
							//printf("transition is original = %d\n",trans->original);
							//locIterator->transitions = addTransitionToList(locIterator->transitions, trans);
							
							trans = NULL; //Just to be safe :)
						}
					}
				}
				
				/*
				 * If the Location Does Not Satisfy the Expression and 
				 * it is a non level 0 location then all transitions 
				 * are retained at this level.
				 */
				
				/*
				 * Adding transitions at the same level:
				 * 1. When the location satisfies the sub-expression (with or without context)
				 * 		A. Transitions for firstmatch conjunct terms (negation of AND'ed firstmatch terms)
				 * 		B. Transitions for non-firstmatch conjunct terms
				 * 2. When the location does not satisfy the sub-expression
				 * 		Copy all outgoing transitions for the location and add level guards
				 * 		Location Context and Not satisfied.
				 * 					OR
				 * 		No location context and transitions exist at this level.
				 */
				
				int breakCycle = 0;
				if(id!=0){
					if(locHasTransAtLevel(locIterator,id)==0){
						//If location has no transitions at this level
						if(breakCycle == 0){
							struct transition* trans = locIterator->transitions;
							//For each location P not satisfying Expr
							while(trans!=NULL){
								//printf("trans->original = %d\n",(int)trans->original);
								if((int)(trans->original)==1){
									//Make a copy of each original Transition with level guard
									struct transition *newTrans = NULL;
									newTrans = addToTransitionList(newTrans,duplicateConditionList(trans->when),trans->sync,duplicateConditionList(trans->reset),trans->gotoLoc);
									newTrans->next = NULL;
									
									//Add guard level == id
									newTrans->when = addConditionToList(newTrans->when,levelGuard);
									
									//Add transition to List of Transition
									locIterator->transitions = addTransitionToList(locIterator->transitions, newTrans);	
									//end of IF
								}
								trans = trans->next;
								//end of transition WHILE
							}
						}
					} else {	
						/*
						* Location has transitions at this level
						* Add transitions for non-first match components
						* 1. Event
						* 2. Non-First-Match DNF
						*/
						if(seq_expr->expr->event==NULL || (seq_expr->expr->event->firstMatch == 0) ){
							//No event OR not a first-match event
							if(seq_expr->expr->dnf && nonFirstMatchExprs){
								struct PORVExpression* temp = nonFirstMatchExprs;
							
								struct transition* trans = locIterator->transitions;
								while(trans!=NULL){
									//printf("trans->original = %d\n",(int)trans->original);
									if((int)(trans->original)==1){
										//Make a copy of each original Transition with level guard
										struct transition *newTrans = NULL;
										newTrans = addToTransitionList(newTrans,duplicateConditionList(trans->when),trans->sync,duplicateConditionList(trans->reset),trans->gotoLoc);
										newTrans->next = NULL;
										
										//Add guard level == id
										newTrans->when = addConditionToList(newTrans->when,levelGuard);
										
										struct condition* nonFMGuards = NULL;
										
										nonFMGuards = generateConjunctGuards(temp->conjunct);
										newTrans->when = addConditionToList(newTrans->when,nonFMGuards);
										
										//Add transition to List of Transition
										locIterator->transitions = addTransitionToList(locIterator->transitions, newTrans);	
										//end of IF
									}
									trans = trans->next;
									//end of transition WHILE
								}	
							}
						}
					}
				}
				//Check the next location
				locIterator = locIterator->next;
				//end of location WHILE
			}
			//end
			
			seq_expr = seq_expr->next;
			id++;
			//end of WHILE
		}
		
		#ifdef DEBUG_ON 
		printf("[levelSequence] Sequencing End\n");
		#endif
		
		//Setting level 0 transitions to be gated by level == 0
		#ifdef DEBUG_ON 
		printf("[levelSequence] Gating Level 0 transitions.\n");
		#endif
		
		locIterator = H->ha->locations;
		transIterator = NULL;
		
		while(locIterator!=NULL){
			transIterator = locIterator->transitions;
			while(transIterator!=NULL){
				if(transIterator->original == 1){
					#ifdef DEBUG_ON
					struct condition* condIterator = transIterator->when;
					while(condIterator!=NULL){
						printFeatureCondition(condIterator);
						printf("\t");
						condIterator = condIterator->next;
					}printf("\n");
					#endif
					
					transIterator->when = addConditionToList(transIterator->when,createCondition("level","0",0));
				} 
				transIterator = transIterator->next;
			}
			locIterator = locIterator->next;
		}
		
		//end of protective IF
		free(strID);
		#ifdef DEBUG_ON 
		printf("[levelSequence] Sequencing Ends\n");
		#endif
	}
	
	#ifdef DEBUG_ON 
	printf("[levelSequence] ENDED\n");
	#endif
	return H;
}

struct phaver* addTimeBound(struct phaver* HA, double bound){
	if(HA!=NULL){
		struct location* locIterator = HA->ha->locations;
		while(locIterator!=NULL){
			char *strBound  = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			if(strcasecmp(locIterator->name,"final")==0)
				sprintf(strBound,"1");
			else sprintf(strBound,"%f",bound);
			
			struct condition* timeBound = createCondition("time",strBound,1);
			
			locIterator->invariant = addConditionToList(locIterator->invariant,timeBound);
			
			locIterator = locIterator->next;
		}
	}
	return HA;
}

char* getWorkPath(struct config* cfg){
	if(cfg){
		return cfg->workPATH;
	}
}
char* getSpaceExPath(struct config* cfg){
	if(cfg){
		return cfg->spaceExPATH;
	}
}
char* getLibPath(struct config* cfg){
	if(cfg){
		//printf("LibPath = [%s]\n",cfg->libPATH);fflush(stdout);
		return cfg->libPATH;
	}
}
char* getDReachPath(struct config* cfg){
	if(cfg){
		return cfg->dReachPATH;
	}
}


void setWorkPath(struct config* cfg, char* path){
	if(cfg){
		sprintf(cfg->workPATH,"%s",path);
	}
}
void setSpaceExPath(struct config* cfg, char* path){
	if(cfg){
		sprintf(cfg->spaceExPATH,"%s",path);
	}
}
void setLibPath(struct config* cfg, char* path){
	if(cfg){
		sprintf(cfg->libPATH,"%s",path);
	}
}
void setDReachPath(struct config* cfg, char* path){
	if(cfg){
		sprintf(cfg->dReachPATH,"%s",path);
	}
}

int checkBinExists(char* path){
	if(path){
		pid_t binPID=fork();
		if (binPID==0)
		{ //child process 
			//static char *argList[]={"./phaver","featureAutOut.pha",NULL};
			int devNull = open("/dev/null", O_WRONLY);
			int save_out = dup(fileno(stdout));
			dup2(devNull, STDOUT_FILENO);
			
			execlp(path,path,(char*)NULL);
			dup2(save_out, STDOUT_FILENO);
			close(save_out);
			close(devNull);
			fflush(stdout);
			//printf("CHILD EXITING\n");
			exit(-1);
		}
		else
		{
			int status;
			int val = waitpid(binPID,&status,0); // wait for child to exit 
			//printf("val = [%d] status =[%d],%d\n",val,status,WEXITSTATUS(status));
			if(WEXITSTATUS(status)!=1){
				return 0;
			} else return 1;
			
		}
	} 
	return 0;	
}

void validateEnvironment(char* libPath,char* workPath,char* spaceExPath,char* dReachPath){
	int lib,work,spaceEx,dReach;
	int workRW;
	lib = checkDirExists(libPath);
	work = checkDirExists(workPath);
	spaceEx = checkBinExists(spaceExPath)?1:checkDirExists(spaceExPath);
	dReach = checkBinExists(dReachPath)?1:checkDirExists(dReachPath);
	workRW = checkDirReadWritePermission(workPath);
	
	printf("\n------------------------------Path Status---------------------------------\n");
	printf("Lib Path:\t [%s]\n",lib?"OK":"MISSING");
	printf("Work Path:\t [%s] ",work?"OK":"MISSING");
	printf(": [%s]\n",workRW==0?"WRITE-OK":(workRW==1?"NO-WRITE-ACCESS, READ-OK":(abs(workRW)==abs(EACCES)?"NO-READ-ACCESS":"DIR-DOES-NOT-EXIST")));
	printf("SpaceEx Path:\t [%s]\n",spaceEx?"OK":"MISSING");
	printf("dReach Path:\t [%s]\n",dReach?"OK":"MISSING");
	
	if(!(lib && work && spaceEx && dReach)){
		
		printf("Cannot proceed with broken paths. Correct the path specifications.\n");
		exit(0);
	}
}
/*int main(){
 *	struct condition porv;
 *	sprintf(&(porv.LHS),"x");
 *	sprintf(&(porv.RHS),"12");
 *	porv.op = 1;
 *	
 *	return 0;
 * }*/
