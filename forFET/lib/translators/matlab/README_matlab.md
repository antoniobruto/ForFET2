Translator from SpaceEx to HASLAC
===

In this folder, we present the *MATLAB* translator. 

Requirements
---
Running the translator requires the installation of Matlab in your machine. The scipt does not make use of any other toolbox or add-on.

>*Note*: This can be verified by exploring the use of dependecies of the main/root MATLAB program file via

>`>> matlab.codetools.requiredFilesAndProducts('spaceex2haslac.m)`
or
``>> license('inuse')``

>More information [[1](https://www.mathworks.com/help/matlab/ref/matlab.codetools.requiredfilesandproducts.html)] and [[2](https://www.mathworks.com/help/matlab/ref/license.html)].

Installation
---

No special installation is needed. The files should be downloaded from the [ForFET2](https://github.com/antoniobruto/ForFET2) Github repository via ssh or https. 

Clone the repository via git as follows:

```
git clone https://github.com/antoniobruto/ForFET2
cd forFET/lib/translators/
```

Contents
---
This folder contains two MATLAB functions:

1. `spaceex2haslac.m` (main function)
2. `xml2struct.m` (used for converting XML files into MATLAB structures, borrowed from [[3](https://www.mathworks.com/matlabcentral/fileexchange/28518-xml2struct)])

Usage
---

To run the translator on a specific example, you simply need to navigate to the `forFET/lib/translators/matlab` directory and run in the command window `spaceex2haslac()`. 

The `spaceex2haslac()` can contain up to 2 inputs and will produce one text file (.ha). The first input corresponds to the SpaceEx model which contains the hybrid automaton and the second input is the configuration file which is used to define the initial conditions. The usage is as follows.

Using default SpaceEx model and configuration file:

```
spaceex2haslac 
```

Specifying only the SpaceEx model and not providing any configuration file (all variables will be initialized to zero and location with smallest SpaceEx index will be used) by

```
spaceex2haslac(SX_model) 
```

Specifying both the SpaceEx model and the configuration file by

```
spaceex2haslac(SX_model, cfg_file) 
```

Examples: 
```
spaceex2haslac('batt.xml','batt.cfg'),
```
```
spaceex2haslac('buck.xml')
```
>*Note*: If you want to call the translator from any directory, you should add this folder to the MATLAB path. 

>1. Write `addpath(genpath('path_to_forfet/lib/translators'))` in the command window or 
>2. Use MATLAB interface to i) go to the `Current Folder` on the left, ii) navigate to the desired destination,from the MATLAB interface and iii) select `Add to Path` and then `Selected Folders and Subfolders`. 

Translation
---

The aim of the `spaceex2haslac` is to perform syntactic translation while handling modeling differences. The modeling differences correspond to the HASLAC language as well as to the supported symbols, operators and notatios of `ForFET2`. 

- The translator can handle SpaceEx models that have single hybrid automata but no network of hybrid automata. That is the case as `ForFET` and ` ForFET2/ForFET-SMT` only support single hybrid automata. 

	> Note that the translator also supports SpaceEx model that have one base component (single automaton) and one network component (network automaton). The network is used for defining the parameter values and should only contain a mapping/bind to the base component. The translator generates a HASLAC model which can be parsed by ForFET. 

- The elements that we parse and get from the SpaceEx model are as follows.

	1. automaton name and structure
	2. parameters
		* controlled/continuous variables
		* constants
	3. locations
	4. outputs (local or global)
	5. invariants
	6. flows
	7. resets/assignments
	8. guards
	9. transitions/switchings
	10. number of locations
	11. number of ntransitions

- The expressions should ignore parenethesis and the disjunction of constraints is captured by `&&`,

- The initialization should contain the keyword `mode` and should be added first in the `initial` module.

- The name of the module should be the same as the name of the generated HASLAC file.