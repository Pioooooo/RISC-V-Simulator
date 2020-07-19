for file in $(ls -a data); do
	if test ${file##*.} = data; then
#		gcc data/${file%.*}.c -o std
#		./std
#		std=$?
    printf "%s: " ${file%.*}
		ans=$(./cmake-build-debug/RISC_V ./data/$file)
#		if [[ $std != $ans ]]; then
#			printf "%-20s %-4s %-4s\n" $file $std $ans
#		fi
#    printf "%s\n" $ans
	fi
done
#rm std
