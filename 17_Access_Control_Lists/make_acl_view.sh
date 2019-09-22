#!/bin/bash
include_dirs="-I ../03_System_Programming_Concepts/ -I ../08_Users_and_Groups"
programs="acl_view.c ../08_Users_and_Groups/ugid_functions.c ../03_System_Programming_Concepts/error_functions.c"
gcc -o acl_view ${include_dirs} ${programs} -lacl
