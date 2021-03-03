#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Exercise 15-1 (c)
# ------------------------------------------------------------------------------

read_str="-r"
write_str="-w"
exec_str="-x"

for perm in {0..7}
do
    dir_name=test${perm}
    if [[ -d "${dir_name}" ]] 
    then
        chmod 700 "${dir_name}"
        rm -fR "${dir_name}"
    fi
    mkdir "${dir_name}"
    touch "${dir_name}"/old_file
done

printf "| Permission | Create File | Open File | Write File | Delete File |\n"
printf "| ---------- | ----------- | --------- | ---------- | ----------- |\n"

for read_perm in {0..1}
do
    for write_perm in {0..1}
    do
        for exec_perm in {0..1}
        do
            perm_str="${read_str:${read_perm}:1}${write_str:${write_perm}:1}${exec_str:${exec_perm}:1}"
            printf "| %s------ " "${perm_str}"
            (( perm = 4*read_perm + 2*write_perm + exec_perm ))
            dir_name=test${perm}
            chmod "${perm}00" "${dir_name}"
            touch "${dir_name}"/new_file 2>/dev/null      && \
                printf "| :heavy_check_mark: "            || \
                printf "| :negative_squared_cross_mark: "
            [[ -r "${dir_name}"/old_file ]]               && \
                printf "| :heavy_check_mark: "            || \
                printf "| :negative_squared_cross_mark: "
            [[ -w "${dir_name}"/old_file ]]               && \
                printf "| :heavy_check_mark: "            || \
                printf "| :negative_squared_cross_mark: "
            rm -f "${dir_name}"/old_file 2>/dev/null      && \
                printf "| :heavy_check_mark: "            || \
                printf "| :negative_squared_cross_mark: "
            printf "|\n"
        done
    done
done

