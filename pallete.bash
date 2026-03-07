a=0
# lt is less than operator
#Iterate the loop until a less than 10
while [ $a -lt 10 ]
do 
    # Print the values
    echo -e "\033["$a"mThis is text ["$a"\033[0m " 
    # increment the value
    a=`expr $a + 1`
done
a=0
# lt is less than operator
#Iterate the loop until a less than 10
echo ""
echo "BASE"
while [ $a -lt 16 ]
do 
    # Print the values
	padded=$(printf "%03d" "$a")
    echo -en "\033[38;5;"$a"m"$padded"\033[0m "
	# increment the value
    a=`expr $a + 1`
done
echo ""
echo "RGB"
while [ $a -lt 232 ]
do 
    # Print the values
	padded=$(printf "%03d" "$a")
    echo -en "\033[38;5;"$a"m"$padded"\033[0m "
	# increment the value
    a=`expr $a + 1`
done
echo ""
echo "GRAY"
while [ $a -lt 256 ]
do 
    # Print the values
	padded=$(printf "%03d" "$a")
    echo -en "\033[38;5;"$a"m"$padded"\033[0m "
	# increment the value
    a=`expr $a + 1`
done
echo ""
a=0
# lt is less than operator
#Iterate the loop until a less than 10
echo "BASE"
while [ $a -lt 16 ]
do 
    # Print the values
	padded=$(printf "%03d" "$a")
    echo -en "\033[48;5;"$a"m"$padded"\033[0m "
	# increment the value
    a=`expr $a + 1`
done
echo ""
echo "RGB"
while [ $a -lt 232 ]
do 
    # Print the values
	padded=$(printf "%03d" "$a")
    echo -en "\033[48;5;"$a"m"$padded"\033[0m "
	# increment the value
    a=`expr $a + 1`
done
echo ""
echo "GRAY"
while [ $a -lt 256 ]
do 
    # Print the values
	padded=$(printf "%03d" "$a")
    echo -en "\033[48;5;"$a"m"$padded"\033[0m "
	# increment the value
    a=`expr $a + 1`
done
echo ""
