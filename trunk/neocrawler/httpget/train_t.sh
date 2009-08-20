#!/bin/bash


foreach_dir(){
	for file in $1/*
	do    
		if [  -d $file ]; then
			echo $file" is dir."  

			rm  -f  "$file/tmp_s.htm"
			rm  -f  "$file/tmp_dbg.htm"
			./12039  -td  "$file/" 
			#./12039  -md  "$file/"  "$file/"  "$file/"
		fi
	done

}



if (( $# == 1 )); then
     foreach_dir $1
else
     echo "Usage:    "$0" directory"
fi

