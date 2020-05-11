#ifndef MAX_STR_LENGTH
	#define MAX_STR_LENGTH 256
#endif

//#define MAIN_DEBUG_ON

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include "haslacParser.tab.h"
#include "featureParser.tab.h"
#include "spaceExOutputParser.tab.h"
#include "drhParser.tab.h"
#include "drhOutParser.tab.h"
#include "structs.h"
//#include "featureCompute.h"
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h> /* for wait */
#include <sys/time.h>
#include <time.h>

extern FILE* haslacin;
extern FILE* haslacout;
extern int haslacparse(void);

extern FILE* featurein;
extern FILE* featureout;
extern int featureparse(void);

extern FILE* spaceExin;
extern FILE* spaceExout;
extern int spaceExparse(void);

extern FILE* drhin;
extern FILE* drhout;
extern int drhparse(void);

extern FILE* drhOutin;
extern FILE* drhOutout;
extern int drhOutparse(void);

extern FILE* configin;
extern FILE* configout;
extern int configparse(void);

FILE* drhOut;
FILE* runFile;
/*
extern struct config* forFETConfig;
extern struct phaver* HA;
extern struct feature* sysFeature;
extern struct interval* featureRange;

extern int kValue;
extern int lValue;

extern struct phaver* featureHA;
extern struct identifier* modelList;
extern struct identifier* featureList;

extern float maxNumber;
*/
/*---------------------DEFINITIONS------------------------*/

struct config* forFETConfig = NULL;
struct phaver* HA = NULL;
struct feature* sysFeature = NULL;
struct interval* featureRange = NULL;

int kValue = 0;
int lValue = 0;

struct phaver* featureHA = NULL;
struct identifier* modelList = NULL;
struct identifier* featureList = NULL;

float maxNumber = 0.0;

int main(int argc, char *argv[]) {
	//----------------------------------   Local Declarations  -----------------------------------
	time_t t = time(NULL);
	struct tm *tm = localtime(&t);
	
	char run_fileName[MAX_STR_LENGTH];
	
	int param_id;
	double samplingTime=0.0F;
	double timeHorizon=0.0F;
	char choice[2];
	char* featureName;
	char var[MAX_STR_LENGTH]; 
	struct identifier* id;
	char* syncLabel;
	int scenario=1;
	int finalLocID = 0;
	int internalModel_MAX_ID;
	
	struct timeval start, stop;
	
	char spaceExPath[MAX_STR_LENGTH];
	char dReachPath[MAX_STR_LENGTH];
	char workPath[MAX_STR_LENGTH];
	char libPath[MAX_STR_LENGTH];
	
	//--------------------------------    Configure Environment -----------------------------------
	if(argc>1){
		forFETConfig = createConfig();
		bzero(forFETConfig,sizeof(struct config));
		
		char configFileName[MAX_STR_LENGTH];
		
		strcpy(configFileName,argv[1]);
	
		configin = fopen(configFileName,"r");
		setbuf(stdout,NULL);
		configparse();
		fclose(configin);
		#ifdef MAIN_DEBUG_ON
			printf("Configuration Read\n");
		#endif
	}
	
	if(forFETConfig!=NULL){
		//printf("IN HERE [%s]\n",forFETConfig->libPATH);
		sprintf(libPath,"%s",getLibPath(forFETConfig));
		if(strlen(trim(libPath))>0){
			//printf("IN HERE-1\n");
			if(libPath[strlen(libPath)-1]=='/'){
				//printf("IN HERE-1A\n");
				libPath[strlen(libPath)-1]='\0';
			}
		} else {
			//printf("IN HERE-2\n");
			sprintf(libPath,"lib");
		}
		
		
	
		sprintf(workPath,"%s",getWorkPath(forFETConfig));
		if(strlen(trim(workPath))>0){
			//printf("IN HERE-3\n");
			if(workPath[strlen(workPath)-1]=='/'){
				//printf("IN HERE-3A\n");
				workPath[strlen(workPath)-1]='\0';
			}
		} else {
			//printf("IN HERE-4\n");
			sprintf(workPath,"work");
		}
		//printf("[%s]",getSpaceExPath(forFETConfig));
		sprintf(spaceExPath,"%s",getSpaceExPath(forFETConfig));
		if(strlen(trim(spaceExPath))==0){
			//printf("IN HERE-6\n");
			sprintf(spaceExPath,"spaceex/spaceex_exe/spaceex");
		}
		
		sprintf(dReachPath,"%s",getDReachPath(forFETConfig));
		//printf("[%s]",getDReachPath(forFETConfig));
		if(strlen(trim(dReachPath))==0){
			//printf("IN HERE-8\n");
			sprintf(dReachPath,"dReach");
		}
	} else {
		sprintf(libPath,"lib");
		sprintf(workPath,"work");
		sprintf(spaceExPath,"spaceex/spaceex_exe/spaceex");
		sprintf(dReachPath,"dReach");
	}
	validateEnvironment(libPath,workPath,spaceExPath,dReachPath);
	
	sprintf(run_fileName,"%s/run_%d%d%d%d%d%d.txt", workPath, tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
	runFile = fopen(run_fileName,"w");
	
	
	//printf("HERE\n");fflush(stdout);
	//------------------------------    Choose from Model Library    -----------------------------
	modelList = NULL;
	readModelList(&modelList,libPath);
	
	char* modelName = getModelName( requestModelChoice( displayModelList( modelList ) ), modelList );
	char fileName[MAX_STR_LENGTH];
	
	sprintf(fileName,"%s/analysisLog.txt",workPath);
	FILE* logger = fopen(fileName,"w");
	if(!logger){
		printf("[ERROR] Problems were encountered creating analysisLog.txt - Writing to the standard output only.\n");
	}
	
	sprintf(fileName,"%s/models/%s.ha",libPath,modelName);
	if(logger)
		fprintf(logger,"Analyzing Model [%s] at location [%s/models/%s.ha].\n",modelName,libPath,modelName);
	#ifdef MAIN_DEBUG_ON
		printf("[FV]: fileName = %s\n",fileName);
	#endif
	
	haslacin = fopen(fileName, "r");
	setbuf(stdout, NULL);
	
	//---------------------------------------------------------------------------------------------
	//Parse the Hybrid Automaton as a HASLAC File
	haslacparse();
	fclose(haslacin);
	#ifdef MAIN_DEBUG_ON
		printf("--------------------AUTOMATON H1: Before Splitting ----------------------\n");
		displayAutomaton(HA);
	#endif
	
	choice[0] = 'n'; choice[1] = '\0';
	do{
		//-----------------------------    List Modifiable parameters    -----------------------------
		if(choice[0] != 'n' || choice[0] != 'N'){
			do{
				param_id = requestParamChoice( displayModelParameters(HA) );
				if(runFile) fprintf(runFile,"%d\n",param_id);
				if(param_id == 0) break;
				editParameter(param_id,HA->params);
			} while(1);
		}
		
		if(choice[0] == 'n' || choice[0] == 'N'){
			#ifdef MAIN_DEBUG_ON
				printf("--------------------AUTOMATON H2: After Editing ----------------------\n");
				displayAutomaton(HA);
			#endif
			printf("\n------------------------------Tool Parameters-----------------------------\n");
			printf("\nTimescale: ");
			scanf("%lf",&samplingTime);
			
			printf("\nTrace Time Horizon: ");
			scanf("%lf",&timeHorizon);
			
			printf("\nMaximum expected rational value: ");
			scanf("%f",&maxNumber);
			
			if(runFile) fprintf(runFile,"%lf\n%lf\n%f\n",samplingTime,timeHorizon,maxNumber);
			
			if(logger){
				fprintf(logger,"\n-------------------------PARAMETERS FOR ANALYSIS-------------------------\n");
				fprintf(logger,"Timescale : %lf\n",samplingTime);
				fprintf(logger,"Trace Time Horizon : %lf\n", timeHorizon);
				fprintf(logger,"Maximum expected rational : %lf\n",maxNumber);
				
				fflush(logger);
			}
			printf("\n--------------------------------------------------------------------------\n");
			
			//Generate SpaceEx XML and CFG for model
			sprintf(fileName,"%s/%s.xml",workPath,modelName);
			
			#ifdef MAIN_DEBUG_ON
				printf("[FV]: FileName for SpaceEx Model Generation = %s\n",fileName);
			#endif
			
			writeSpaceEx(HA,fileName);
			
			sprintf(fileName,"%s/%s.cfg",workPath,modelName);
			
			writeSpaceExConfig(HA, fileName, samplingTime, timeHorizon,1,NULL);
		
			//-----------------------------    Choose a Feature    -----------------------------
			featureList = NULL;
			readFeatureList(&featureList, modelName,libPath);
			
			featureName = getFeatureName( requestFeatureChoice( displayFeaturelList( featureList ) ), featureList );
			
			sprintf(fileName,"%s/features/%s/%s.dat", libPath, modelName, featureName);
			if(logger)
				fprintf(logger,"Computing Feature [%s].\n",featureName);
			
			//printf("\nFileName = %s\n",fileName);
			#ifdef MAIN_DEBUG_ON
				printf("[FV]: fileName = %s\n",fileName);
				fflush(stdin); fflush(stdout);
			#endif
			
			featureHA = NULL;
			
			scanf("%c",choice);
			//printf("\nProceed with spliting? (y/n): ");
			//scanf("%c",choice);
			choice[0]='y';
			if(choice[0]!='y' && choice[0]!='Y'){
				exit(0);//break;
			}
			//struct phaver* HA is now LIVE
			//Add a variable for level sequencing
			
			//-------------------------------------- Feature Transformations -----------------------------------------
			sprintf(var,"level");
			id = createIdentifier(var);
			addLocalVariable(HA,id,0,0);
			#ifdef MAIN_DEBUG_ON
				printf("ADDED LOCAL VARIABLE: level\n");
			#endif
			
			//-----------------------------Parse the Feature and Tune the Hybrid Automaton-----------------------------
			featurein = fopen(fileName,"r");
			
			#ifdef MAIN_DEBUG_ON
				printf("[TOOL] Feature File Handler [%p]\n",featurein);
			#endif

			if(!featurein){
				printf("\nWe couldn't find the feature dat file at \"%s\". Make sure the feature spec is present at this location.\n",fileName);
				exit(0);
			}
			//printf("HERE\n");fflush(stdout);
			
			featureparse();
			
			#ifdef MAIN_DEBUG_ON
				printf("FEATURE STRUCTURE IN MEMORY\n");
			#endif
			
			fclose(featurein);
			
			//Update Feature Parameters
			getParamValueFromIO(sysFeature);
			
			id = createIdentifier(sysFeature->name);
			addLocalVariable(HA,id,DBL_MIN,0);
			
			#ifdef MAIN_DEBUG_ON
				printf("\n\n---------AUTOMATON H2: After Splitting, Before Level Sequencing---------\n");
				displayAutomaton(HA);
				printf("\n\n------------------------------ FEATURE F -------------------------------\n");
				printFeature(sysFeature);
				printf("\n\n-----------------------------PORV Details-------------------------------\n");
				printPORVDetails(sysFeature);	
			#endif	
			
			syncLabel = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			strcpy(syncLabel,getIdentifierName(HA->ha));
			
			if(HA->temporal!=1){
				sprintf(var,"time");
				id = createIdentifier(var);
				addLocalVariable(HA,id,0,0);
			}
			
			levelSequence(HA,sysFeature,syncLabel);
			
			//Adding Time Restrictions
			if(HA->temporal==1)
			{	
				HA = addTimeBound(HA,(double)timeHorizon);
			} else {
				/*sprintf(var,"time");
				id = createIdentifier(var);
				addLocalVariable(HA,id,0,0);
				
				struct location* finalLoc = getLocationByName(HA->ha->locations, "FINAL");
				
				if(finalLoc) addLocalVariableActivityToLocation(finalLoc, var, 1);
				else printf("\nFinal Location Not Found\n");
				
				#ifdef MAIN_DEBUG_ON
					printf("ADDED LOCAL VARIABLE: level\n");
				#endif
				*/
				
			}
			
			#ifdef MAIN_DEBUG_ON
				printf("\n\n---------AUTOMATON H3: After Level Sequencing---------\n");
				displayAutomaton(HA);
			#endif	
				
			sprintf(fileName,"%s/featureModel_%s_%s.xml",workPath,modelName,featureName);
			
			finalLocID = writeSpaceEx(HA,fileName);
			
			sprintf(fileName,"%s/featureModel_%s_%s.cfg",workPath,modelName,featureName);
			
			scenario = getSpaceExScenario();
			if(logger){
				fprintf(logger,"SpaceEx Scenarion (Algorithm) being used: %s\n",scenario==0?"LGG":"STC");
			}
			writeSpaceExConfig(HA,fileName,samplingTime,timeHorizon,scenario,featureName);
		}
		//--------------------------------ANALYSIS CHECK-------------------------------------
		//printf("Do you wish to re-tune the parameters of the model? (y/n) : ");
		//scanf("%s",choice);
		choice[0] = 'n';
	
	}while(!(choice[0]=='n' || choice[0] == 'N'));
	
	//spaceex/spaceex_exe/spaceex -m models/featureModel_batt_riseTime.xml -g models/featureModel_batt_riseTime.cfg -o sx.out
	char *spaceExModel = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
	char *spaceExConfig = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
	char *drhModel = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
	
	sprintf(spaceExModel,"%s/featureModel_%s_%s.xml",workPath,modelName,featureName);
	sprintf(spaceExConfig,"%s/featureModel_%s_%s.cfg",workPath,modelName,featureName);
	sprintf(drhModel,"%s/drh_%s_%s.drh",workPath,modelName,featureName);
	printf("\n****************************SpaceEx Analysis******************************\n");
	pid_t spaceExPID=fork();
	if (spaceExPID==0)
	{ //child process 
		//static char *argList[]={"./phaver","featureAutOut.pha",NULL};
		
		FILE* sxOut;
		
		sprintf(fileName,"%s/reachAnalysis.out",workPath);
		sxOut = fopen(fileName, "w");
		
		fflush(stdout);
		dup2(fileno(sxOut), 1);
		fclose(sxOut);
		
		sprintf(fileName,"%s/sx.out",workPath);
		
		//execlp("spaceex/spaceex_exe/spaceex","spaceex/spaceex_exe/spaceex","-m",spaceExModel,"-g",spaceExConfig,"-o","work/sx.out",(char*)NULL);
		execlp(spaceExPath,spaceExPath,"-m",spaceExModel,"-g",spaceExConfig,"-o",fileName,(char*)NULL);
		printf("ERROR: FAILED TO RUN SpaceEx\n");
		exit(127); // only if execv fails 
	}
	else
	{ // pid!=0; parent process 
		
		waitpid(spaceExPID,0,0); // wait for child to exit 
		sprintf(fileName,"%s/sx.out",workPath);
		
		spaceExin = fopen(fileName,"r");
		spaceExparse();
		if(featureRange!=NULL){
			printf("\n--------------------------------------------------------------------------\n");
			printf("\n              Feature Range found [%lf,%lf]\n",featureRange->left,featureRange->right);
			printf("\n--------------------------------------------------------------------------\n");
			
			if(logger){
				fprintf(logger,"\n-------------------------------------------------------------------------\n");
				fprintf(logger,"\n              Feature Range found [%lf,%lf]\n",featureRange->left,featureRange->right);
				fprintf(logger,"\n-------------------------------------------------------------------------\n");
				fflush(logger);
			}
			
		} else {
			printf("\n--------------------------------------------------------------------------\n");
			printf("                     Feature Range Could Not Be Computed\n");
			printf("\n--------------------------------------------------------------------------\n");
			
			if(logger){
				fprintf(logger,"\n--------------------------------------------------------------------------\n");
				fprintf(logger,"                     Feature Range Could Not Be Computed\n");
				fprintf(logger,"\n--------------------------------------------------------------------------\n");
				fflush(logger);
			}
			fclose(logger);
			exit(0);
		}
		printf("\n**************************************************************************\n");
		
		pid_t hystPID=fork();
		if(hystPID == 0){
			//java -jar Hyst.jar -dreach models/featureModel_batt_riseTime.xml models/featureModel_batt_riseTime.cfg 
			//Redirecting Output
			//FILE* drhOut = fopen(drhModel, "w+");
			//fflush(stdout);
			//dup2(fileno(drhOut), 1);
			//fclose(drhOut);
			
			execlp("java", "java", "-jar","Hyst.jar","-dreach",spaceExModel,spaceExConfig,"-o",drhModel,(char*)NULL);
			printf("ERROR: FAILED TO EXECUTE THE SPACEEX to DREACH CONVERTER : Hyst.jar\n");
			exit(127);
			
		} else {
			waitpid(hystPID,0,0); // wait for child to exit 
			//printf("\nDONE\n");
			
			//printf("The MaxNumber Entered is %f\n",maxNumber);
			//printf("\nDREACH MODEL FILE: %s\n",drhModel);
			drhin = fopen(drhModel,"r");
			//printf("HERE\n");fflush(stdout);
			char drhTemplateFileName[MAX_STR_LENGTH];
			sprintf(drhTemplateFileName,"%s/drhTemplate.drh",workPath);
			drhOut = fopen(drhTemplateFileName,"w");
			drhparse();
			
			//printf("\nHERE\n");fflush(stdout);
			char* buff = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			buff = getcwd(buff,MAX_STR_LENGTH);
			//printf("\n%s\n",buff);
			//printf("\n%s\n",get_current_dir_name());
			printf("\n*************************EXTREMAL TRACE SEARCH***************************\n");
			if(logger) fprintf(logger,"\n*************************EXTREMAL TRACE SEARCH***************************\n");
			
			printf("\nTOOL-TIP: The Extremal Trace Search uses a bound on the length of the \n");
			printf("          location path. The analysis explores all paths upto a maximum \n");
			printf("          specified path length.\n");
			
			int satCheckDepth = 10;
			printf("Bound on the Maximum Location Path Length: ");
			scanf("%d",&satCheckDepth);
			if(runFile) fprintf(runFile,"%d\n",satCheckDepth);
			
			float precision = 0.1;
			printf("\nTOOL-TIP: The Extremal Trace Search uses a precision limit.\n");
			  printf("          The analysis terminates when this precision limit is reached.\n");
			
			
			printf("Precision Limit: ");
			scanf("%f",&precision);
			if(runFile) fprintf(runFile,"%f\n",precision);
			
			if(logger){
				fprintf(logger,"Bound on the Maximum Location Path Length: %d\n",satCheckDepth);
				fprintf(logger,"Precision Limit: %f\n",precision);
				fflush(logger);
			}
			
			struct interval* featureRangeX = createIntervalInit(floor(featureRange->left - precision),ceil(featureRange->right + precision),featureRange->id);
			
			printf("\n----------------------------LEFT CORNER SEARCH----------------------------\n");
			
			float elapsed = 0.0;
			gettimeofday(&start,0);
			
			struct interval* satLeftRange = extremeTraceSearch(featureName, featureRangeX, precision, satCheckDepth, precision, -1, finalLocID,workPath,dReachPath);
			
			printf("\n----------------------------RIGHT CORNER SEARCH---------------------------\n");
			
			struct interval* satRightRange = extremeTraceSearch(featureName, featureRangeX, precision, satCheckDepth, precision, +1, finalLocID,workPath,dReachPath);
			
			gettimeofday(&stop,0);
			elapsed = timedDiff(start, stop);
			
			printf("\n**************************************************************************\n");
			printf("\nSMT Search Code Executed in : %f seconds.\n",elapsed/1000.0);
			
			#ifdef DEBUG_ON
				printf("[MAIN] Sat Left, Right Range is [%s,%s]\n",satLeftRange?"Not NULL":"NULL",satRightRange?"Not Null":"NULL");
			#endif
			
			if(satLeftRange){
				printf("LEFT CORNER = %lf : TRACE-FILE-ID = trace_%d \n",satLeftRange->left,satLeftRange->id);
				if(logger) fprintf(logger,"LEFT CORNER = %lf : TRACE-FILE-ID = trace_%d \n",satLeftRange->left,satLeftRange->id);
			} else {
				printf("Could not find the Left Corner.\n");
				if(logger) fprintf(logger,"Could not find the Left Corner.\n");
			}	
			
			if(satRightRange){
				printf("RIGHT CORNER = %lf : TRACE-FILE-ID = trace_%d \n",satRightRange->right,satRightRange->id);
				if(logger) fprintf(logger,"RIGHT CORNER = %lf : TRACE-FILE-ID = trace_%d \n",satRightRange->right,satRightRange->id);
			} else {
				printf("Could not find the Right Corner.\n");
				if(logger) fprintf(logger,"Could not find the Right Corner.\n");
			}
			
			printf("\nTraces may be viewed using the ODE visualization tool provided by dReach/dReal.\n");
			printf("Link to the tool:\n");
			printf("     https://github.com/dreal/dreal3/tree/master/tools/ODE_visualization\n");
			printf("\n*************************************************************************\n");
			
			if(logger){
				fprintf(logger,"\nTraces may be viewed using the ODE visualization tool provided by dReach/dReal.\n");
				fprintf(logger,"Link to the tool:\n");
				fprintf(logger,"     https://github.com/dreal/dreal3/tree/master/tools/ODE_visualization\n");
				fprintf(logger,"\n*************************************************************************\n");
				fflush(logger);
			}
			
			if(featureRange!=NULL){
				printf("\nSpaceEx Range:\t\t\t [%lf,%lf]\n",featureRange->left,featureRange->right);
			} else printf("\nSpaceEx could not compute the Feature Range.\n");
			if(satLeftRange){
				printf("Extremal Analysis Range:\t [%lf,%lf]\n",satLeftRange->left,satRightRange->right);
			} else printf("\nExtremal Analysis could not compute the Feature Range.\n");
		
			if(logger){
				fprintf(logger,"\n----------------------------ANALYSIS SUMMMARY----------------------------\n");
				if(featureRange!=NULL){
					fprintf(logger,"\nSpaceEx Range:\t\t\t\t [%lf,%lf]\n",featureRange->left,featureRange->right);
				} else fprintf(logger,"\nSpaceEx could not compute the Feature Range.\n");
				if(satLeftRange){
					fprintf(logger,"Extremal Analysis Range:\t [%lf,%lf]\n",satLeftRange->left,satRightRange->right);
				} else fprintf(logger,"\nExtremal Analysis could not compute the Feature Range.\n");
				fprintf(logger,"\n-------------------------------------------------------------------------\n");
				fflush(logger);
			}
			printf("\n***************************************************************************\n\n");
			//satCheck(featureName,satCheckDepth,precision,constructGoal(featureName,featureRange,finalLocID));
			
			
			
			//char* drhTemplateModel = (char*)malloc(sizeof(char)*MAX_STR_LENGTH);
			//sprintf(drhTemplateModel,"models/featureTemplate.drh");
			
			
			
		}
	}
}


