#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Exercise 15-1 (c)
# ------------------------------------------------------------------------------

declare -a perm_str
perm_str[0]="---"
perm_str[1]="--x"
perm_str[2]="-w-"
perm_str[3]="-wx"
perm_str[4]="r--"
perm_str[5]="r-x"
perm_str[6]="rw-"
perm_str[7]="rwx"

yes_mark=":heavy_check_mark:"
no_mark=":negative_squared_cross_mark:"

for dir_perm in {0..7}
do
    dir_name="test${dir_perm}"
    if [[ -d "${dir_name}" ]] 
    then
        chmod 700 "${dir_name}"
        rm -fR "${dir_name}"
    fi
    mkdir "${dir_name}"
    for file_perm in {0..7}
    do
        touch "${dir_name}"/old_file${file_perm}
        touch "${dir_name}"/samp_file${file_perm}
        chmod "${file_perm}00" "${dir_name}"/old_file${file_perm}
        chmod "${file_perm}00" "${dir_name}"/samp_file${file_perm}
    done
done

# ------------------------------------------------------------------------------
# Table of Contents
# ------------------------------------------------------------------------------

printf "# Table of Contents\n\n"
printf "* [Exercise Results](#exercise-results)\n"
printf "  * [File Creation](#file-creation)\n"
printf "  * [File Access](#file-access)\n"

# ------------------------------------------------------------------------------
# Exercise Results
# ------------------------------------------------------------------------------

printf "\n# Exercise Results\n"

# ------------------------------------------------------------------------------
# File Creation
# ------------------------------------------------------------------------------

printf "\n## File Creation\n\n"

printf "| Permission | Create File |\n"
printf "| ---------- | ----------- |\n"

for dir_perm in {0..7}
do
    printf "| %s------ " "${perm_str[$dir_perm]}"
    dir_name="test${dir_perm}"
    chmod "${dir_perm}00" "${dir_name}"
    touch "${dir_name}"/new_file 2>/dev/null    && \
        printf "| %s " "${yes_mark}"            || \
        printf "| %s " "${no_mark}"
    printf "|\n"
done

# ------------------------------------------------------------------------------
# File Access
# ------------------------------------------------------------------------------

printf "\n## File Access\n\n"

printf "| Dir Perm | File Perm | Open File | Write File | Delete File |\n"
printf "| -------- | --------- | --------- | ---------- | ----------- |\n"

for dir_perm in {0..7}
do
    dir_name="test${dir_perm}"
    chmod "${dir_perm}00" "${dir_name}"
    for file_perm in {0..7}
    do
        [[ ${file_perm} == 0 ]]                               && \
            printf "| %s------ " "${perm_str[$dir_perm]}"     || \
            printf "| "
        printf "| %s------ " "${perm_str[$file_perm]}"
        [[ -r "${dir_name}"/old_file${file_perm} ]]           && \
            printf "| %s " "${yes_mark}"                      || \
            printf "| %s " "${no_mark}"
        [[ -w "${dir_name}"/old_file${file_perm} ]]           && \
            printf "| %s " "${yes_mark}"                      || \
            printf "| %s " "${no_mark}"
        rm -f "${dir_name}"/old_file${file_perm} 2>/dev/null  && \
            printf "| %s " "${yes_mark}"                      || \
            printf "| %s " "${no_mark}"
        printf "|\n"
    done
done

