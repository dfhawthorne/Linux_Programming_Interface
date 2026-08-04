#!/usr/bin/env bash
# --------------------------------------------------------------------------------------
# Test harnass for Exercise 30-2:
#
# --------------------------------------------------------------------------------------

if [[ -z "${LD_LIBRARY_PATH}" ]]
    then export LD_LIBRARY_PATH=".:../Shared_Libraries"
    else export LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:.:../Shared_Libraries"
fi

./Ex_2
