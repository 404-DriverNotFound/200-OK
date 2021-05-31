STARTTIME=$(date +%s)
echo "\n\n\n\n\n\n\n\n\n\n\n\n\n" | ft_tester/tester http://localhost:8000
ENDTIME=$(date +%s)
echo "It takes $(($ENDTIME - $STARTTIME)) seconds to complete this task..."