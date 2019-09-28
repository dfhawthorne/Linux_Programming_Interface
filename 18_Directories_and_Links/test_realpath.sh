#!/bin/bash
# ---------------------------------------------------------------------
# Test cases for my version of realpath
# ---------------------------------------------------------------------
test_cases="/ // /.. /../ /. /./ /etc/.. /etc/../ ./test1.dat ./test.dat //etc //etc/ //etc/..//./.. //etc/..//./../ /fred /fred/ //etc/.. //etc/..// //etc/..//. //etc/..//./ //etc/../. //etc/.././ //etc/.././.."
# Create sample files used in test cases
[ -a test.dat ] || touch test.dat
[ -a test1.dat ] || ln -s test.dat test1.dat
[ -a /fred ] && printf "Directory '/fred' should not exist\n"
# Run each test case
for tc in ${test_cases}
do  printf "Test case: '%s' " ${tc}
    printf -v exp_result "realpath: %s --> %s" ${tc} $(realpath ${tc})
    act_result=$(./realpath ${tc})
    if [ "$act_result" = "$exp_result" ]
        then printf "PASSED\n"
        else printf "FAILED\n\tExpected: %s\n\tActual:   %s\n" "$exp_result" "$act_result"
    fi
done