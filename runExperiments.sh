
# This script should be used to run the experiments.


printf "\n-----------------------------------------------------------------------\n"
printf  "\n\t\t Starting the Experiments\n"

printf "\n Please note that you can run every experiment individually.\n\n"

read -p "Press the 'Enter' key to start the 1st experiment."


./Experiment_1.sh
 
read -p "Do you want to continue with the 2nd experiment (y or n)?" #-n 1
echo
if [[  $REPLY =~ ^[Yy]$ ]]
then
	printf "\n-----------------------------------------------------------------------\n\n"
	printf "Experiment 2 is similar to experiment 1 with the difference that it\n"
	printf "does not employ first match semantics. First match semantics were\n"
	printf "not supported in ForFET but have been added in ForFET-SMT. The experiment\n"
	printf "might take a very long time to terminate so we have not addded it separately\n"
	printf "for completeness.\n\n"
	printf "To execute Experiment 2, do it separately, by typing ./Experiment2.sh in\n"
	printf "the terminal (or uncomment the corresponding line in the current script file.\n"
	#./Experiment_2.sh
	printf "\n-----------------------------------------------------------------------\n\n"
fi

read -p "Do you want to continue with the 3rd experiment (y or n)?" #-n 1
if [[  $REPLY =~ ^[Yy]$ ]]
then
	printf "\n-----------------------------------------------------------------------\n\n"
	./Experiment_3.sh
	printf "\n-----------------------------------------------------------------------\n\n"
fi

read -p "Do you want to continue with the 4th experiment (y or n)?" #-n 1
if [[  $REPLY =~ ^[Yy]$ ]]
then	
	printf "\n-----------------------------------------------------------------------\n\n"
	./Experiment_4.sh
	printf "\n-----------------------------------------------------------------------\n\n"
fi

printf "\n\n This is the end of the experiments.\n\n"
