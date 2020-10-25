cd forFET/
printf "\n-----------------------------------------------------------------------\n\n"
printf "Experiment 3: Unsafe Atomic Reactor Temperature Control\n"
printf "\n-----------------------------------------------------------------------\n"

printf "An atomic reactor's temperature control strategy has been designed.\n"
printf "It is unclear if this strategy is safe. Features and SMT are used\n" 
printf "to examine situations where the atomic reactor goes into a meltdown.\n"

printf "We first run SpaceEx (a reachability tool) to compute an over-approximation\n"
printf "of the desired range (it describes time). We use the LGG scenario of SpaceEx.\n"
printf "We then refine this range using dReach (an SMT solver).\n\n"

printf "The command-line inputs for this experiment are defined in the file\n"
printf "Exper_Input_3.txt (each input is on a new line). Note that this input\n"
printf "to the command-line executable might not be visible as we are piping it.\n\n"

printf "At the end of the experiment the tool will display the feature range\n"
printf "computed by SpaceEx and the range computed by dReach. In this case\n"
printf "the range produced by dReach cannot be refined, and is slightly larger, by\n"
printf "an epsilon precision quantity equal to 1. \n\n"

printf "Two traces, corresponding to the feature range corners are plotted\n"
printf "in a new browser window. Note that, in this case, both corners\n"
printf "of the feature range correspond to failures, that is in both cases the\n"
printf "reactor melts-down. The left corner may be interpretted as a \n"
printf "point-of-no-return, that is, the trace describes the latest point at\n"
printf "which, if possible, action could have been taken to prevent failure.\n"

read -p " Do you want to continue with the experiment (y or n)?" #-n 1
echo
if [[  $REPLY =~ ^[Yy]$ ]]
then
	StartTime=$(date +%s)
	./forFET default.cfg < ../Exper_Input_3
	EndTime=$(date +%s)
	echo "It took $(($EndTime-$StartTime)) to seconds to run this experiment".

	rm -f dreal3-master/tools/ODE_visualization/data.json
	cp work/trace_1.json dreal3-master/tools/ODE_visualization/data.json
	cd dreal3-master/tools/ODE_visualization/
	printf "\n Plotting on a Python-based web server. The figure opens in a browser." 
	xdg-open http://0.0.0.0:8000
	timeout 10s ./run_websvr.sh
	
	rm -f dreal3-master/tools/ODE_visualization/data.json
	cd ../../../
	cp work/trace_4.json dreal3-master/tools/ODE_visualization/data.json
	cd dreal3-master/tools/ODE_visualization/
	xdg-open http://0.0.0.0:8000
	timeout 10s ./run_websvr.sh

printf "\n-----------------------------------------------------------------------\n"
printf "\nOBSERVE THAT:\n"
printf "In the terminal, reachability analysis and SMT produced similar results,\n"
printf "albeit with a margin of error of 1, which is a parameter that was set during\n" 
printf "the analysis.\n\n"

printf "In the browser, the traces displayed shows how a meltdown is possible using\n"
printf "the existing strategy. The temperature of the reactor is controlled using two\n" 
printf "control rods, operated using timer c1, c2. The timers represent mechanical\n"
printf "constraints. A rod cannot be used unless it's timer has a value more than \n"
printf "or equal to 20. Additionally Rod 2 is more powerful (can be used at higher\n"
printf "temperatures than Rod 1. In the behaviour shown, as the reactors temperature\n"
printf "rises, Rod 2 is used first, the temperature goes down and Rod 2 is removed.\n"
printf "The temperature rises again, and Rod 1 is used because c2 has a value under 20.\n"
printf "The next time the temperature increases, the control is unable to use either\n"
printf "rods, since both timers, c1 and c2, have values are under 20, and this leads\n"
printf "to a reactor meltdown.\n\n"

printf "Such traces can be used by designers to examine their designs and refine them\n"
printf "to be safer or more robust\n"
printf "\n-----------------------------------------------------------------------\n"
fi
