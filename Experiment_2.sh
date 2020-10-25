
cd forFET/

printf "\n-----------------------------------------------------------------------\n\n"
printf "Experiment 2: Battery Charger \n"
printf "\n-----------------------------------------------------------------------\n"

printf "In this experiment, we are performing feature analysis on the \n"
printf "battery charger (a model from the library of models in the \n"
printf "forFET/lib/models directory).\n\n"

printf "The goal is to measure the time it takes for the battery charger \n"
printf "to reach its charging level from 10%% to  96%%. The charge level\n"
printf "is described in terms of a nominal voltage.\n\n"

printf "We first run SpaceEx (a reachability tool) to compute an \n"
printf "over-approximation of the desired range (it describes time). \n"
printf "We use the LGG scenario of SpaceEx. Then, we refine this range \n"
printf "using dReach (an SMT solver).\n\n"

printf "The command-line inputs for this experiment are defined in the file\n"
printf "Exper_Input_2.txt (each input is on a new line). Note that this input\n"
printf "to the command-line executable might not be visible as we are piping it.\n\n"

printf "At the end of the experiment the tool will display the feature range\n"
printf "computed by SpaceEx and the refined range computed by dReach. The dReach\n"
printf "range should be tighter (smaller). The reachability results will be shown\n"
printf "in a new browser window. Two tabs will be displayed for the corner case\n"
printf "traces (left/smallest case and right/largest case).\n\n"

printf "Disclaimer: Note that this experiment does not use urgency via \n"
printf "first match semantics. This is a feature of ForFET-SMT which did \n"
printf "not exist in ForFET. This fact makes the computation a lot slower \n"
printf "and almost impossible to terminate.\n\n"

printf "You might have to press Ctrl-C to stop the analysis."

read -p "Do you want to continue with the experiment (y or n)?" #-n 1
echo
if [[  $REPLY =~ ^[Yy]$ ]]
then
	StartTime=$(date +%s)
	if test -f "forFET"; then
		./forFET default.cfg < ../Exper_Input_2
	else
		echo "You need to build the executable first. Run ./buildForFET.sh".
		exit $?
	fi
	EndTime=$(date +%s)
	echo "It took $(($EndTime-$StartTime)) to seconds to run this experiment".

	rm -f dreal3-master/tools/ODE_visualization/data.json
	cp work/trace_1.json dreal3-master/tools/ODE_visualization/data.json
	cd dreal3-master/tools/ODE_visualization/
	printf "\n Plotting on a Python-based web server. The figure opens in a browser." 
	xdg-open http://0.0.0.0:8000
	timeout 15s ./run_websvr.sh
	rm -f dreal3-master/tools/ODE_visualization/data.json
	cd ../../../
	cp work/trace_10.json dreal3-master/tools/ODE_visualization/data.json
	cd dreal3-master/tools/ODE_visualization/
	xdg-open http://0.0.0.0:8000
	timeout 15s ./run_websvr.sh

printf "\n-----------------------------------------------------------------------\n"
printf "\n OBSERVE that the feature ranges of SpaceEx are [6323.010000,15000.000000],\n"
printf "while the extremal analysis via SMT might never terminate.\n"
printf "\n-----------------------------------------------------------------------\n"
fi
