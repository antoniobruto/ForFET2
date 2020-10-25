
cd forFET/
printf "\n-----------------------------------------------------------------------\n\n"
printf "Experiment 1: Battery Charger \n"
printf "\n-----------------------------------------------------------------------\n"

printf "In this experiment, we are performing feature analysis on the battery \n"
printf "charger (a model from the library of models in the forFET/lib/models directory).\n "
printf "The goal is to measure the time it takes for the battery charger to reach\n" 
printf "its charging level from 10%% to 96%%. The charge level is described in terms\n"
printf "of a nominal voltage.\n\n"

printf "We first run SpaceEx (a reachability tool) to compute an over-approximation\n"
printf "of the desired range (it describes time). We use the LGG scenario of SpaceEx.\n"
printf "We then refine this range using dReach (an SMT solver).\n\n"

printf "The command-line inputs for this experiment are defined in the file\n"
printf "Exper_Input_1.txt (each input is on a new line). Note that this input\n"
printf "to the command-line executable might not be visible as we are piping it.\n\n"

printf "At the end of the experiment the tool will display the feature range\n"
printf "computed by SpaceEx and the refined range computed by dReach. The dReach\n"
printf "range should be tighter (smaller). The reachability results will be shown\n"
printf "in a new browser window. Two tabs will be displayed for the corner case\n"
printf "traces (left/smallest case and right/largest case).\n\n"

printf "Disclaimer: Note that this experiment uses urgency via first match semantics.\n"
printf "This is a feature of ForFET-SMT which did not exist in ForFET. This fact\n"
printf "makes the computation a lot faster (around 10x). As a comparison, check\n"
printf "the same example without first match semantics (Experiment 2).\n\n"

read -p "Do you want to continue with the experiment (y or n)?" #-n 1
echo
if [[  $REPLY =~ ^[Yy]$ ]]
then
	StartTime=$(date +%s)
	if test -f "forFET"; then
		./forFET default.cfg < ../Exper_Input_1
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
	cp work/trace_8.json dreal3-master/tools/ODE_visualization/data.json
	cd dreal3-master/tools/ODE_visualization/
	xdg-open http://0.0.0.0:8000
	timeout 15s ./run_websvr.sh
	
printf "\n-----------------------------------------------------------------------\n\n"
printf "OBSERVE THAT:\n"
printf "The feature range produced by SpaceEx is [6323.010000,7974.190000], while\n"
printf "the extremal analysis via SMT refines and significantly decreases \n"
printf "the range to [6957.612735,7788.793472].\n\n"
printf "The traces correspond to the left (minimum time) and right (maximum)\n"
printf "corner of the range. These two traces are displayed in two different\n"
printf "tabs in the browser (Firefox). In the tab, the user could observe\n" 
printf "different variable traces but should notice the variable 'v' indicating charge.\n"
printf "\n-----------------------------------------------------------------------\n"
fi
