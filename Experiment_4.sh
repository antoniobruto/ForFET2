cd forFET/
printf "\n-----------------------------------------------------------------------\n\n"
printf "Experiment 4: Unsafe Atomic Reactor Temperature Control\n"
printf "\n-----------------------------------------------------------------------\n"

printf "An atomic reactor's temperature control strategy has been designed.\n"
printf "It is unclear if this strategy is safe. Features and SMT are used\n" 
printf "to examine situations where the atomic reactor goes into a meltdown.\n\n"

printf "The command-line inputs for this experiment are defined in the file\n"
printf "Exper_Input_4.txt (each input is on a new line). Note that this input\n"
printf "to the command-line executable might not be visible as we are piping it.\n\n"

printf "Disclaimer: This experiment aims to show that the dReach construction\n"
printf "of JSON files might be buggy or problematic.\n\n"

printf "At the end of the experiment, we generate two json files. One is the\n"
printf "original JSON file from dReach and the other is one that ForFET-SMT\n"
printf "has modified/postprocessed. The former file cannot be visualized \n"
printf "while the latter can.\n\n"

read -p " Do you want to continue with the experiment (y or n)?" #-n 1
echo
if [[  $REPLY =~ ^[Yy]$ ]]
then
	StartTime=$(date +%s)
	./forFET default.cfg < ../Exper_Input_4
	EndTime=$(date +%s)
	echo "It took $(($EndTime-$StartTime)) to seconds to run this experiment".

	rm -f dreal3-master/tools/ODE_visualization/data.json
	cp work/trace_4.json dreal3-master/tools/ODE_visualization/data.json
	cd dreal3-master/tools/ODE_visualization/
	printf "\n Plotting our modified json file."
	printf "The traces are displayed in the browser." 
	xdg-open http://0.0.0.0:8000
	timeout 10s ./run_websvr.sh
	
	printf "\n Plotting the original json file.\n"
	printf "The traces cannot be displayed in the browser. They are empty.\n" 
	rm -f dreal3-master/tools/ODE_visualization/data.json
	cd ../../../
	cp work/aut_7_2.smt2.json dreal3-master/tools/ODE_visualization/data.json
	cd dreal3-master/tools/ODE_visualization/
	xdg-open http://0.0.0.0:8000
	timeout 10s ./run_websvr.sh

printf "\n-----------------------------------------------------------------------\n"
printf "\nOBSERVE THAT:\n"
printf "dReach produces a json file at the end of the extremal trace search. \n"
printf "In this experiment, we have selected the depth to be 10. The generated\n"
printf "file is stored in the forFET/work directory and is called aut_7_2.smt2.json\n.\n"

printf "Manually, one can verify using the following command,\n"
printf "       -----------------------------------\n"
printf "   cat forFET/work/aut_7_2.smt2.json  | grep null\n"
printf "       -----------------------------------\n"
printf "that, the file contains 'null' JSON objects.\n"
printf "This JSON file cannot be processed and displayed in the browser\n"
printf "unlike the modified json file that we have auto-created.\n\n"
printf "It may be observed that in the file forFET/work/aut_7_2.smt2.json\n"
printf "the indexes of the variables around the NULL objects are not\n"
printf "consecutive. ForFET-SMT parses the erroneous JSON, eliminates NULL\n"
printf "objects, and re-indexes all variables to adhere to the format\n"
printf "acceptable for display in the browser.\n\n"
printf "Manually, one can verify the pocessed JSON using the following command,\n"
printf "       -----------------------------------\n"
printf "   cat forFET/work/trace_4.json  | grep null\n"
printf "       -----------------------------------\n"

printf "In the browser, the first tab displays the JSON file treated by \n"
printf "ForFET-SMT, while the second tab is empty and corresponds to the\n"
printf "original JSON file from dReach\n.\n" 


printf "\n-----------------------------------------------------------------------\n"
fi
