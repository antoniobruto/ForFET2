clear
printf "\nBuilding ForFET Interface Parsers ...\n"
lex -Pconfig parsers/configParser.l
bison --warnings=none -d parsers/configParser.y -p config

cat parsers/preConfig|cat - lex.config.c > temp.c && mv temp.c lex.config.c
cat parsers/preConfig|cat - configParser.tab.c > temp.c && mv temp.c configParser.tab.c

lex -Phaslac parsers/haslacParser.l
bison --warnings=none -d parsers/haslacParser.y -p haslac

cat parsers/preHASLAC|cat - lex.haslac.c > temp.c && mv temp.c lex.haslac.c
cat parsers/preHASLAC|cat - haslacParser.tab.c > temp.c && mv temp.c haslacParser.tab.c

lex -Pfeature parsers/featureParser.l
bison --warnings=none -d parsers/featureParser.y -p feature

cat parsers/preFeature|cat - lex.feature.c > temp.c && mv temp.c lex.feature.c
cat parsers/preFeature|cat - featureParser.tab.c > temp.c && mv temp.c featureParser.tab.c

lex -PspaceEx parsers/spaceExOutputParser.l
bison --warnings=none -d parsers/spaceExOutputParser.y -p spaceEx

cat parsers/preSpaceEx|cat - lex.spaceEx.c > temp.c && mv temp.c lex.spaceEx.c
cat parsers/preSpaceEx|cat - spaceExOutputParser.tab.c > temp.c && mv temp.c spaceExOutputParser.tab.c

lex -Pdrh parsers/drhParser.l
bison --warnings=none -d parsers/drhParser.y -p drh

cat parsers/preDRH|cat - lex.drh.c > temp.c && mv temp.c lex.drh.c
cat parsers/preDRH|cat - drhParser.tab.c > temp.c && mv temp.c drhParser.tab.c

lex -PdrhOut parsers/drhOutParser.l
bison --warnings=none -d parsers/drhOutParser.y -p drhOut
cat parsers/preDRHOut|cat - lex.drhOut.c > temp.c && mv temp.c lex.drhOut.c
cat parsers/preDRHOut|cat - drhOutParser.tab.c > temp.c && mv temp.c drhOutParser.tab.c

printf "Parsers Processed. Building ForFET ... \n"

if ! g++ -w lex.feature.c featureParser.tab.c lex.haslac.c haslacParser.tab.c spaceExOutputParser.tab.c lex.spaceEx.c drhParser.tab.c lex.drh.c lex.drhOut.c drhOutParser.tab.c lex.config.c configParser.tab.c structs.c fbfvTool.c -o forFET `pkg-config --cflags --libs glib-2.0 json-glib-1.0`  ; then #2> buildLog.txt
	printf "ForFET build failure. Please check forFET/buildLogs.txt for a detailed error log.\n"
	exit 0
else 
	printf "ForFET build successfull. ForFET binary is available as forFET/forFET\n"
	exit 1
fi



