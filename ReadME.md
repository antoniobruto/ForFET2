__________________________________________________________________________

**************************************************************************
                              ForFET-SMT

               ReadME - ForFET-SMT - IIT Kharagpur
              Created by Antonio A. Bruto da Costa
                  E-mail: antonio@iitkgp.ac.in

             Principal Investigator: Prof. Pallab Dasgupta

   Other developers: 
	SpaceEx to HASLAC Translator: Dr. Nikolaos Kekatos
**************************************************************************

Use: For feature based formal analysis of Hybrid Automata Models 

**************************************************************************

Requires:
1. C, C++ compilers, lex, bison, JAVA, 32 bit C libraries, Python 2.7
2. libglib2.0-dev, libjson-glib-dev
3. SpaceEx
4. dReach/dReal
5. HyST
6. A configuration for setting up ForFET paths (A default configuration
   has been provided in default.cfg).

NOTE: External tools SpaceEx, dReach/dReal, and HyST have all been included
in this ZIP and do not need to be separately downloaded. Other dependencies
(especially on libraries) can be installed using the script "install-libs.sh"
We assume that you are using the virtual machine provided below or a 
similarly (or better) configured system. 

The instructions below have been tested on the TACAS 2021 Ubuntu 20.04 
virtual machine (VM) hosted on Zenodo (https://zenodo.org/record/4041464#.X5Kiv4axW8g)

After having freshly imported the VM, the scripts provided with ForFET-SMT 
can be run to install additional dependencies and build ForFET-SMT.

There are two main ways of building ForFET-SMT. 
1. The quick auto installation and running of ForFET-SMT
2. The step-by-step installation process.

**************************************************************************

			QUICK-AUTO-INSTALL-AND-RUN
(NOTE: "Run >>" is only an indication of what to type in the terminal)
			
1. The first step is to set the file permissions.
	--------------------------------------------------
	Run >> sh permissions.sh	
	--------------------------------------------------	
		
2. A single script "quickSetup.sh", has been included that automates 
dependency installations and the process of building and running ForFET. 

	The second step is to build ForFET
	--------------------------------------------------
	Run >> ./quickSetup.sh	
	--------------------------------------------------	

3. To run experiments, first open the web-browser Firefox and then:
	A. Run all experiments:
	----------------------------------------------------
	Run >> ./runExperiments.sh
	----------------------------------------------------
	B. Run each experiment separately: Refer to the next section.

4. To restore Python Symlink from Python 2.7 to Python 3 (only if needed)
	---------------------------------------------------
	Run >> ./restore-python.sh
	---------------------------------------------------
		
**************************************************************************

			STEP-BY-STEP INSTRUCTIONS
(NOTE: "Run >>" is only an indication of what to type in the terminal)
			
1. Again, the first step is to set the file permissions.
	--------------------------------------------------
	Run >> sh permissions.sh	
	--------------------------------------------------
					
2. Install Libraries required by ForFET:
	---------------------------------------------------
	Run >> ./install-libs.sh
	---------------------------------------------------
	
3. Build ForFET-SMT:
	---------------------------------------------------
	Run >> ./buildForFET.sh
	---------------------------------------------------
	
4. To run experiments, first open the web-browser Firefox and then:
	A. Run all experiments:
	----------------------------------------------------
	Run >> ./runExperiments.sh
	----------------------------------------------------
	B. Run each experiment separately: Refer to the next section.

5. To restore Python Symlink from Python 2.7 to Python 3 (only if needed)
	---------------------------------------------------
	Run >> ./restore-python.sh
	---------------------------------------------------
	
**************************************************************************
		
			    DEMO EXPERIMENTS
			    
ForFET-SMT is a command line tool. It is also possible to redirect inputs to
ForFET-SMT from a file, enabling scripts to be written for experiments. 

To demonstrate ForFET-SMT, we provide scripts/instructions for the following:

-------------------------------------------------------------------------------

1. Demonstrating Extremal Value Analysis -- Timed Property

This experiment performs feature analysis on a battery charger model with 
first match (urgent) semantics. 

To run this experiment:
	----------------------------------------------------
	Run >> ./Experiment_1.sh
	----------------------------------------------------
	
-------------------------------------------------------------------------------

2. Non first match semantics -- State explosion and blowup

This experiment performs feature analysis on a battery charger model without 
first match semantics. The SMT computations are prone to time out.

This experiment aims to highlight the improvement by adding first match 
semantics and higlihghting the need and usability of ForFET-SMT.

To run this experiment:
	----------------------------------------------------
	Run >> ./Experiment_2.sh
	----------------------------------------------------

-------------------------------------------------------------------------------

3. Extremal Value Analysis -- Safety Property

This experiment performs feature analysis on a nuclear reactor model.

To run this experiment:
	----------------------------------------------------
	Run >> ./Experiment_3.sh
	----------------------------------------------------

-------------------------------------------------------------------------------

4. Visualization problems with Json and dReach

This experiment again uses the nuclear reactor model and shows the 
visualization problem caused by the JSON file generation.

To run this experiment:
	----------------------------------------------------
	Run >> ./Experiment_4.sh
	----------------------------------------------------

***************************************************************************
			RUNNING ALL EXPERIMENTS
	----------------------------------------------------
	You can run all the experiments by running
	Run >> ./runExperiments.sh
	----------------------------------------------------

***************************************************************************
	      PROBLEMS WITH VISUALIZING TRACES IN ForFET-SMT

In the scripts above, we use a timeout to run a webserver that is able
to render the output traces from ForFET-SMT. When the VM is running slow, this 
timeout could create problems with rendering. In these situations, we recommend
that you run the webserver manually. The instructions for doing this are below.

1.	The file-names of traces associated with the feature range corners 
	are specified inthe results produced by ForFET-SMT. These names can
	be identified by looking for lines in the terminal starting with 
	'LEFT CORNER' and 'RIGHT CORNER'. The trace file name follows the 
	keyword 'TRACE-FILE-ID'. 
	
	For instance the following may be found in the terminal:
	------------------------------------------------------------------
	SMT Search Code Executed in ...	
	LEFT CORNER = 6957.612735 : TRACE-FILE-ID = trace_1 
	RIGHT CORNER = 7788.793472 : TRACE-FILE-ID = trace_8
	
	...
	------------------------------------------------------------------
	You may then look for trace_1.json and trace_8.json in the directory
	forFET/work/
	
2.	To visualize trace_n (where n is a positive number), copy trace_n.json
	to the directory forFET/dreal3-master/tools/ODE_visualization/ and 
	rename it to data.json
	
	For instance, in the terminal, if the root directory of the tool is named
	ForFET-SMT, and if your current path is 'ForFET-SMT/forFET/work'
	then the following command achieves this.
	------------------------------------------------------------------
	Run >> cp trace_n.json ../dreal3-master/tools/ODE_visualization/data.json
	------------------------------------------------------------------	
	You may replace 'trace_n.json' with the name of the trace of your choice.

3. 	From the forFET/work directory, go to the directory 
	forFET/dreal3-master/tools/ODE_visualization/
	------------------------------------------------------------------
	Run >> cd ../dreal3-master/tools/ODE_visualization/
	------------------------------------------------------------------

4.	Start the web-server
	------------------------------------------------------------------
	Run >> ./run_websvr.sh 
	------------------------------------------------------------------
	If it does not execute, try running 'chmod 777 run_websvr.sh' and 
	then starting the web-server again.
	
5.	In a browser (FireFox), type the following address in the address bar
	------------------------------------------------------------------
	http://0.0.0.0:8000/
	------------------------------------------------------------------
	
	You should now see the trace you chose in the browser. 
	
	To quit the server use Ctrl+C while in the terminal where the 
	web-server is running.
	
***************************************************************************

To run ForFET standalone and use the tool:  
	---------------------------------------------------
	In the directory "forFET"
	Run "./forFET default.cfg"
	----------------------------------------------------
	Alternately refer to the next section on experiments
	to run guided experiments for the tool demonstration.
	----------------------------------------------------
	
***************************************************************************
	
External Tool Downloads (not-necessary since all binaries and libraries are already included in this version of ForFET-SMT)
1. SpaceEx from http://spaceex.imag.fr/sites/default/files/downloads/private/spaceex_exe-0.9.8f.tar.gz and Untar into the root folder of the tool
2. dReach/dReal from https://github.com/dreal/dreal3/releases/tag/v3.16.06.02
3. HyST from https://github.com/verivital/hyst

We are grateful to the authors of the tools listed above that we use with
ForFET.

**************************************************************************
		
			ADDITIONAL DOCUMENTATION

     Documentation for ForFET is available in the "docs" directory.

**************************************************************************

EXAMPLES: The new ForFET has a library consisting of hybrid systems and 
a features to start you out with.

**************************************************************************

                 OTHER SUPPORT TOOLS/SCRIPTS
	
[ SpaceEx to HASLAC Translator ]
	Requires: MATLAB/Octave
	Location: forFET/lib/translators/
	Examples: forFET/lib/translators/spaceex_models

************************************************************************** 
