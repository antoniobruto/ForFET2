__________________________________________________________________________

**************************************************************************
                                 ForFET

                    ReadME - ForFET - IIT Kharagpur
              Created by Antonio A. Bruto da Costa
                  E-mail: antonio@iitkgp.ac.in

             Principal Investigator: Prof. Pallab Dasgupta

	Other developers: 
		SpaceEx -> HASLAC Translator: Dr. Nikolaos Kekatos
**************************************************************************

Use: For feature based formal analysis of Hybrid Auomata Models 

**************************************************************************

TO BUILD: Run the build-script 'build' in the folder forFET

Requires 
1. C, C++ compilers, lex, bison, JAVA, 32 bit C libraries
2. libglib2.0-dev, libjson-glib-dev
3. Download SpaceEx from http://spaceex.imag.fr/sites/default/files/downloads/private/spaceex_exe-0.9.8f.tar.gz and Untar into the root folder of the tool
4. Download dReach/dReal from https://github.com/dreal/dreal3/releases/tag/v3.16.06.02
5. Write a configuration for setting up ForFET paths. Refer to tool documentation in the directory docs.

**************************************************************************

TOOL USAGE:
Build using the following syntax:
	./buildForFET

The binary for ForFET is present in the directory forFET.

**************************************************************************

EXAMPLES: The new ForFET has a library consisting of hybrid systems and 
a features to start you out with.

**************************************************************************

							OTHER TOOLS
	
	SpaceEx -> HASLAC Translator
	Requires: MATLAB/Octave
	Location: forFET/lib/translators/
	Examples: forFET/lib/translators/spaceex_models

**************************************************************************
