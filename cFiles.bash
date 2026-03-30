#!C:\msys64\usr\bin\bash.exe bash
function lookInFolder(){
	
	local items=($(ls))
	local path=($1)

	for item in "${items[@]}"
	do
		if [[ -f $item ]];
		then	
			if [[ $item == *.c ]];
			then
				echo "$path\\\\$item\","
			fi
		else
			cd $item
			lookInFolder "$path\\\\$item"
			cd ..
		fi
	
	done

}
lookInFolder "\"\${workspaceFolder}"