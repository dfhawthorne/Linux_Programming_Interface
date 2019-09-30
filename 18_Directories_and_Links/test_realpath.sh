#!/bin/bash
# ---------------------------------------------------------------------
# Test cases for my version of realpath
# ---------------------------------------------------------------------
base_cases="/ /etc /fred"
for tc in ${base_cases}
do  
    base_cases="${base_cases} /${tc}"
done
test_cases="${base_cases}"
delimiters="/ /. /.. /fred"
#test_cases="/ // /.. /../ /. /./ /etc/.. /etc/../ ./test1.dat ./test.dat //etc //etc/ //etc/..//./.. //etc/..//./../ /fred /fred/ //etc/.. //etc/..// //etc/..//. //etc/..//./ //etc/../. //etc/.././ //etc/.././.."
for tc in ${base_cases}
do  
    for dlm in ${delimiters}
    do
        test_cases="${test_cases} ${tc}${dlm}"
    done
done
test_cases="${test_cases} //etc/..//./.. //etc/..//./../"
# Create sample files used in test cases
[ -a test.dat ]  || touch test.dat
[ -a test1.dat ] || ln -s test.dat test1.dat
[ -a /fred ]     && printf "Directory '/fred' should not exist\n"
# Run each test case
for tc in ${test_cases}
do  
    real_result=$(realpath ${tc})
    act_result=$(./realpath ${tc})
    printf "Test case: '%s' " ${tc}
    printf -v exp_result "realpath: %s --> %s" ${tc} ${real_result}
    if [ "$act_result" = "$exp_result" ]
        then printf "PASSED\n"
        else printf "FAILED\n\tExpected: %s\n\tActual:   %s\n" "$exp_result" "$act_result"
    fi
done