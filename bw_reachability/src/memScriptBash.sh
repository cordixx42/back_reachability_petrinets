#!/bin/bash

TIMEFORMAT="%M"
DIRECTORIES="allBenchmarks/mist/boundedPN"
TIMEOUT=60

for dir in $DIRECTORIES
do
    files=$(find ./$dir -type f -name "*.spec") # .spec of all subdirectories
    logfile=./$dir/results/petrimat.log

    # cp $logfile $logfile".bak" 2> /dev/null
    # > $logfile # Clear logs file

    echo "### Processing" $dir "###"
    echo "hihihhihi"
    echo $files


    skip=1
    for file in $files
    do
        if [ "$skip" -lt "0" ]; then
	    echo_time " Skiping $file."
	    skip=$(($skip+1))
	    continue
	    fi

		echo " Verifying $file..."
		echo " HHH"
		echo " $file"

		(time (timeout $TIMEOUT ./out/petrimat $file)) 1> temp_output \
															2> temp_time
				result=$?
				if [ "$result" = "0" ]; then
				output="Safe"
				elif [ "$result" = "1" ]; then
				output="Unsafe"
				elif [ "$result" = "124" ]; then
				output=""
				else
					output="Unknown"
				fi
		
		if [ "$output" = "" ]; then
			output="Timeout"
			elapsed=$((1000*$TIMEOUT))
		else
			elapsed=$(cat temp_time)    
		fi

		echo "   Result:" $output          # Console ouput
		echo "   Time:  " $elapsed "mb"    #

		echo $file,$output,$elapsed >> $logfile # Logs output;
	done
done