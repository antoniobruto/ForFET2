/* structs.h 
 * Where all the Phaver and Feature structures, and supporting
 * methods are defined
 */

//#define DEBUG_ON

#ifndef MAX_STR_LENGTH
	#define MAX_STR_LENGTH 1024
#endif
#define MAX_GUARDS 1024
#define MAX_TRANSITION_ASSIGNMENTS 1024
#define MAX_LOCATION_INVARIANTS 100
#define MAX_LOCATION_FLOWS 100

#ifndef STRUCTS_H

#define STRUCTS_H
struct condition{	//Condition Structure : used to represent assignments, invariants, derivatives, and resets.
	char LHS[MAX_STR_LENGTH];	//Left-hand side of operator
	char RHS[MAX_STR_LENGTH];	//Right-hand side of operator
	int op;		//Operator
			// 0 : ==
			// 1 : <=
			// 2 : := or =
			// 3 : >=
			// 4 : <
			// 5 : >
	struct condition* next; //Next in the Linked List
};


struct identifier{		//Identifier Structure : Linked List used to store any generic list of identifiers
	char name[MAX_STR_LENGTH];		//Identifier Name
	struct identifier* next;//Pointer to the next Identifier in the list or Null(If this is the last Identifier)
};

struct transition{		//Transition Structure : Represents location transitions
	int firstMatchTransition;	//True if the transition is added with first match semantics, false otherwise
	struct condition* when;	//Guard Condition
	char sync[MAX_STR_LENGTH];		//Synchronization Labels
	struct condition* reset;//Resets on Transition
	char gotoLoc[MAX_STR_LENGTH];	//Goto Location
	int original;		//Indicates if the transition is of the original automaton or added during the product construction.
	struct transition* next;//Next transition in the list
};


struct location{			//Location Structure : Represents a Hybrid Automaton Location
	char name[MAX_STR_LENGTH];			//Location Name
	char *originalName;		//Original Location Name : NULL with originalName == name
	struct condition* invariant;	//Location Invariant
	struct condition* derivative;	//Location Dynamics
	struct transition* transitions;	//Locations Outgoing Transitions
	struct transitionWrapper* incoming;	//Incoming Transitions
	struct location* next;		//Next location in the list
	struct PORV* labels;		//List of PORV Labels
	int type;
	//int requires;			//Requires Input Variations
};

struct transitionWrapper{		//Used for bookkeeping incoming transitions
	struct location *loc;		//Location with transition
	struct transition* trans;	//Transition of Interest
	struct transitionWrapper* next;
};

struct locationWrapper{			//Location Structure : Represents a Hybrid Automaton Location
	struct location* loc;		//Current location in the list
	int id;				//Used to reference location lists
	struct locationWrapper* next;	//Next location in the list
};

struct automaton{			//Automaton Structure : Represents a Complete Hybrid Automaton
	char name[MAX_STR_LENGTH];			//Automaton name
	struct identifier* contr_vars;	//List of control variables 
	struct identifier* synclabs;	//List of synchronization labels
	struct location* locations;	//List of locations
	struct identifier* var_inputs;	//Variable Inputs
	struct locationWrapper* reqList;	//List of locations that need input variations
};

struct initial{
	struct identifier* init_locs;	//Initial Locations
	struct condition* init_conds;	//Initial set of Valuations
	struct initial* next;		//Next Set
};

struct phaver{				//Phaver code File Wrapper
	struct condition* params;	//Paramenters OR constants of the Hybrid Automaton
	struct automaton* ha;		//Hybrid Automaton
	struct initial* init;		//Initial Conditions
	int temporal;			// 0 for non-temporal, 1 for temporal
};

/*-----------------------PHAVER---------------------------*/
struct PORV{				//List of PORV's conjuncted together
	struct condition* porv;
	int id;
	struct PORV* next;
};

struct eventType{
	int type;			//Event Type - posedge(1), negedge(2), either(0)
	int firstMatch;		//Mark Event as First Match
	struct PORV* porv;		//Event Condition
};

struct PORVExpression{
	int firstMatch;
	struct PORV* conjunct;		//List of PORVs conjuncted together
	struct PORVExpression* next;	//Next PORVConjunct in the Disjunction
};

struct expression{
	struct PORVExpression* dnf;	//Disjunction of Conjunction of PORVs
	struct eventType* event;	//Event
};	

struct timeDelay{
	float lower;			//Delay Lower Bound
	float upper;			//Delay Upper Bound
	int type;			//Eventually (1) or Interval Finite Bounds (0) or Finite Time (2)
};	

struct sequenceExpr{	
	struct expression* expr;	//Sub-sequence Expression
	struct timeDelay* delay;	//Delay associated with expr match
	struct condition* assignments;	//List of Assignments when expr matches within delay
	struct sequenceExpr* next;	//Next sub-expression
};

struct featureDef{
	struct identifier* localVars;	//List of Feature Local Variables
	struct sequenceExpr* seq;	//Sequence Expression - Behaviour to match
	struct condition* featureAssign;//Feature Computation assignments
};

struct feature{
	char name[MAX_STR_LENGTH];			//Feature Name
	struct identifier* params;	//Feature Parameters
	struct featureDef* def;		//Feature Definition (Behaviour and Feature Computation)
};


//--------------SpaceEx Related Structures-------------------
struct transitionSpaceEx{
	int sourceID;
	int targetID;
	char label[MAX_STR_LENGTH];
	//char sourceName[MAX_STR_LENGTH];
	//char targetName[MAX_STR_LENGTH];
	struct condition* guard[MAX_GUARDS];//1024
	struct condition* assignments[MAX_TRANSITION_ASSIGNMENTS];//1024
	struct transitionSpaceEx* next;
};

struct locationSpaceEx {
	int id;
	char name[MAX_STR_LENGTH];
	struct condition* invariants[MAX_LOCATION_INVARIANTS];
	struct condition* flows[MAX_LOCATION_FLOWS];
	struct locationSpaceEx* next;
};
 
//SpaceEx name of the component -> in this case, name of the automaton becomes the component ID
struct componentID {
	char name[MAX_STR_LENGTH];
	struct componentID* next;
};


struct value{
	double encL;
	double encR;
	double timeL;
	double timeR;
	struct value* next;
};

struct varValue{
	char key[MAX_STR_LENGTH];
	int mode;
	int step;
	struct value* values;
};

struct interval{
	double left;
	double right;
	int id;
};

struct config{
	char spaceExPATH[MAX_STR_LENGTH];
	char dReachPATH[MAX_STR_LENGTH];
	char libPATH[MAX_STR_LENGTH];
	char workPATH[MAX_STR_LENGTH];
};

struct keyItem{
	int step;
	int varID;
	char* keyPrefix;
};

/*---------------------DECLARATIONS-----------------------*/
char* operatorMap(int op);

struct condition* createCondition(char *LHS, char* RHS, int op);
struct condition* addConditionToList(struct condition* root, struct condition* cond);
struct condition* addToConditionList(struct condition* root, char *LHS, char* RHS, int op);
int countIdentifiers(struct identifier* list);
struct condition* duplicateConditionList(struct condition* root);
struct condition* createIdentityResets(struct identifier* root);
void printCondition(struct condition* cond);
void printConditionList(struct condition* cond);
struct condition* reverseConditionList(struct condition* root);
void printInvariant(struct condition* invariant);
int conditionCompare(struct condition* cond, char* LHS, char* RHS);
int inConditionList(struct condition* list, char* LHS, char* RHS);
int removeConditionFromList(struct condition** root, char* LHS);

//Identifiers
struct identifier* createIdentifier(char *id);
struct identifier* addIdentifierToList(struct identifier* root, struct identifier* ID);
struct identifier* addToIdentifierList(struct identifier* root, char *id);
int getIdentifierID(struct identifier* list, char *name);
struct identifier* duplicateIdentifierList(struct identifier* root);
void freeIdentifierList(struct identifier* id);
char* containsIdentifier(struct identifier* list1, struct identifier* list2);
void printIdentifier(struct identifier* id);	
void printIdentifierList(struct identifier* id);
char* getIdentifierName(struct automaton* ha);
//Transitions
struct transition* createTransition(struct condition* when, char *sync, struct condition* reset, char* gotoLoc);
struct transition* addTransitionToList(struct transition* root, struct transition* trans);
struct transition* addToTransitionList(struct transition* root, struct condition* when, char *sync, struct condition* reset, char* gotoLoc);
struct transition* addTransitionToLocationTransitionList(struct location* loc, struct transition* trans);
struct transition* duplicateTransision(struct transition* temp);
struct transition* duplicateTransitionList(struct transition* root);
struct transition* createTransitionOnEvent(struct condition* eventCond, char* syncLabel, struct condition* resets, char* gotoLoc, struct identifier* localVars);
void printTransition(struct transition* transition);
int locHasTransAtLevel(struct location* loc, int level);
        
struct transitionWrapper* addToPseudoTransitionList(struct transitionWrapper* root, struct location* loc, struct transition* trans);
struct transitionWrapper* duplicatePseudoTransitionList(struct transitionWrapper* transList);
void freeIncomingTransitions(struct transitionWrapper* incoming);
void processIncomingTransitions(struct location* root);

//Initial Conditions
struct initial* createInitial(struct identifier* init_locs, struct condition* init_conds);
struct initial* addInitialToList(struct initial* root, struct initial* init);
struct initial* duplicateInitialList(struct initial* init);
struct initial* duplicateInitialConditions(struct initial* init);
struct initial* checkInitial(struct location* loc, struct initial* init);
void printInitial(struct initial* init);
void printSpaceExConfigInitial(char* name, struct initial* init,FILE* out);

//Locations
struct location* createLocation(char* name, struct condition* invariant, struct condition* derivative, struct transition* transitions);
struct location* addLocationToList(struct location* root, struct location* loc);
struct location* addToLocationList(struct location* root, char* name, struct condition* invariant, struct condition* derivative, struct transition* transitions, int loctype);
struct location* duplicateLocation(struct location* loc);
struct location* getLocationByName(struct location* root, char *locName);
struct location* getUnmarkedLocation(struct location* root, char *locName, struct locationWrapper* marked);
void printLocation(struct location* loc);
void printLocationList(struct location* l);


struct locationWrapper* addToPseudoLocationList(struct locationWrapper* root, struct location* loc);
//struct locationWrapper* getPseudoLocation(struct locationWrapper* root, struct location* loc);
void printPseudoLocationList(struct locationWrapper *root);
void freePseudoLocationList(struct locationWrapper* root);

//Automata
struct automaton* createAutomaton(struct automaton* root, char* name, struct identifier* contr_vars, struct identifier* synclabs, struct location* locations);

struct phaver* createPhaverStruct(struct phaver* root, struct condition* params, struct automaton* ha, struct initial* init);

void zeroDerivative(char *eqn, char *var);
int charInList(char *c, char *list, int count);

//Config
struct config* createConfig();

/*---------------- Feature Method Declarations-----------------*/

struct eventType* createEvent(int type, struct PORV* porv);

struct PORV* createPORV(struct condition* porv, int id);
struct PORV* addPORVToList(struct PORV* root, struct PORV* porv);
struct PORV* duplicatePORVList(struct PORV* root);

struct PORVExpression* createPORVExpression(struct PORV* conjunct);
struct PORVExpression* addPORVExpressionToEOfList(struct PORVExpression* root, struct PORVExpression* porvExpr);

struct expression* createExpression(struct PORVExpression* porvExpr, struct eventType* event);
struct sequenceExpr* createSequenceExpr(struct expression* expr, struct timeDelay* delay, struct condition* assignments);
struct sequenceExpr* addSequenceExprToEOfList(struct sequenceExpr* root, struct sequenceExpr* seqExpr);

struct timeDelay* createTimeDelay(double lower, double upper, int type);

struct featureDef* createFeatureDef(struct identifier* localVars, struct sequenceExpr* seq, struct condition* featureAssign);
struct feature* createFeature(char *name, struct identifier* params, struct featureDef* def);

int isTruePORV(struct location* loc, struct PORV* porv);
int isTrueConjunct(struct location* loc, struct PORV* conjunct);
int isTruePORVExpr(struct location* loc, struct PORVExpression* PORVExpr);
int isTrueExpr(struct location* loc, struct expression* expr);


char* featureOperatorMap(int op);

void printFeatureCondition(struct condition* cond);
void printAssignmentList(struct condition* assignments);
void printPORV(struct PORV* porv);
void printPORVConjunct(struct PORV* conjunct);
void printEvent(struct eventType* event);
void printPORVExpression(struct PORVExpression* porvExpr);
void printExpression(struct expression* expr);
void printTimeDelay(struct timeDelay* delay);
void printSequenceExpr(struct sequenceExpr* seq);
void printFeatureDefinition(struct featureDef* def);
void printFeature(struct feature* featureStmt);
void printPORVDetails(struct feature* featureStmt);

void writeAutomaton(struct phaver* pha,FILE *out);
void displayAutomaton(struct phaver* pha/*, struct feature* sysFeature*/);
char* phOperatorMap(int op);
char* SpaceExOpMap(int op);
int writeSpaceEx(struct phaver* HA, char* fileName);
void writeSpaceExConfig(struct phaver* HA,char *fileName, double samplingTime, double timeHorizon, int scenario, char* featureName);

int readModelList(struct identifier** list,char* libPath);
int displayModelList(struct identifier* list);
int requestModelChoice(int count);
int displayModelParameters(struct phaver* HA);
int requestParamChoice(int count);
int editParameter(int id, struct condition* params);
char* getModelName(int id, struct identifier* list);
void readFeatureList(struct identifier** list,char* modelName, char* libPath);
int displayFeaturelList(struct identifier* list);
int requestFeatureChoice(int count);
char* getFeatureName(int id, struct identifier* list);
void updateStringParam(char* original, char* substr, double value);
void updatePORVParam(struct PORV* porvStruct, char* param, double value);
void updateFeatureParams(struct feature* featureStmt, double** values, int count);
char* replaceStrWithStr(char const * const original, char const * const pattern, char const * const replacement);
int requestFeatureParamChoice(int count);
int printFeatureParamList(struct identifier* params);
void getParamValueFromIO(struct feature* featureStmt);
char* strToUpper(char* str);
struct value* addToValues(struct value* root, double encL, double encR, double timeL, double timeR);
void freeValueList(struct value* root);
void freeVar(struct varValue* root);
int getSpaceExScenario();
char* trim(char *str);

struct interval* createInterval();
struct interval* createIntervalInit(double left, double right, int id);

//---------------------------SMT EXTENSIONS------------------------------------
char* constructGoal(char* featureName, struct interval* range,int id, int type);
struct interval* satCheck(char *featureName, int depth, float precision, char* goal, char* workPath,char* dReachPath);
struct interval* extractJSONFeatureInterval(char *featureName, int l, int depth, char* workPath);
struct interval* extremeTraceSearch(char* featureName, struct interval* range, double e, int depth, int precision, int dir, int id, char* workPath,char* dReachPath);

int fileCopy(const char* source, const char* dest);
float timedDiff(struct timeval t0, struct timeval t1);
#endif


#ifndef FBCOMPUT_H
#define FBCOMPUT_H

// Function Declarations

void addLocalVariableActivityToLocation(struct location* loc, char* id, int temporal);
void addIdentityResetToTransition(struct transition* trans, char* id);
struct phaver* addFeatureLocalVariables(struct phaver* pha, struct feature* f, double maxConst);
void addLocalVariable(struct phaver* pha, struct identifier* localVar, double maxConst, int temporal);
void addLocalVariables(struct phaver* pha, struct identifier* localVar, double maxConst, int temporal);
struct location *twoWaySplit(struct location *loc, struct PORV* porv, char* sync, struct identifier* vars);
struct location *threeWaySplit(struct location *loc, struct PORV* porv, char* sync, struct identifier* vars);
int splitLocation(struct location *loc, struct PORV* porv, char* sync, struct identifier* vars, struct initial* init);
void pSplitTransform(struct automaton* H, struct PORV* porv, char* sync, struct identifier* vars, struct initial* init);
void tuneForTemporalProperties(struct phaver* pha);
char* getLocationContext(struct PORV* porvConjunct);
struct phaver* splitPORVConjunct(struct phaver* HA, struct PORV* porvConjunct, char *sync);
struct phaver* splitOnEvent(struct phaver* HA, struct sequenceExpr* seq, char *sync);
struct locationWrapper* splitOnLocationContext(struct phaver* HA, char* context, struct locationWrapper* markedList, struct PORV* iterator, char* sync);
struct condition* generateEventGuard(struct eventType* event, struct condition* derivative);
struct condition* generateConjunctGuards(struct PORV* porv);
int countSequenceExpressions(struct sequenceExpr* seq_expr);
void markDelayForFinal(struct condition* activity);
void resetToZero(struct condition* assignmentResets,char* resetName);
void markOriginalTransitions(struct location* locIterator);
struct condition* generateAssignmentResets(struct condition* assignments);
int countConditions(struct condition* root);
struct condition* createZeroDerivative(struct identifier* vars);
void orderedResets(int ExprID, int dnfID,  struct automaton* H, struct location* loc, struct transition* trans, struct condition* resets, struct identifier* vars);
int isStateEvent(struct eventType* event);
struct phaver* levelSequence(struct phaver* H, struct feature* F, char* syncLabel);
struct phaver* addTimeBound(struct phaver* HA, double bound);
int isnumber(char* tempString);
struct keyItem* parseKey(char *src);
int fixNullsInJSON(char* oldFileName, char* newFileName);

char* getWorkPath(struct config* cfg);
char* getSpaceExPath(struct config* cfg);
char* getLibPath(struct config* cfg);
char* getDReachPath(struct config* cfg);

void setWorkPath(struct config* cfg, char* path);
void setSpaceExPath(struct config* cfg, char* path);
void setLibPath(struct config* cfg, char* path);
void setDReachPath(struct config* cfg, char* path);
void validateEnvironment(char* libPath,char* workPath,char* spaceExPath,char* dReachPath);
int checkBinExists(char* path);
#endif
