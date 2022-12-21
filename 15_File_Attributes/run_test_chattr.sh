#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Run tests comparing the results of using my_chattr and chattr commands on all
# file systems mounted in the test directory.
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# Establish the script environment:
# 1. Exit on unset shell variables
# 2. Ensure that the findmnt and chattr command is available
# 3. Ensure that the script has been invoked by the root user
# ------------------------------------------------------------------------------

set -o nounset 

if [[ -z "$(command -v findmnt)" ]]
then
    printf 'The findmnt utility must be installed.\nIt is available in the util-linux package.\n' >&2
    exit 1
fi

if [[ -z "$(command -v chattr)" ]]
then
    printf 'The chattr utility must be installed.\nIt is available in the e2fsprogs package.\n' >&2
    exit 1
fi

eff_uid=$(id --user)
if [[ "${eff_uid}" -ne 0 ]]
then
    printf '%s must be run as root\n' "$0" >&2
    exit 2
fi

# ------------------------------------------------------------------------------
# Local configuration
# ------------------------------------------------------------------------------

my_dir="$(dirname $0)"
pushd "${my_dir}" &>/dev/null
if [[ $? -ne 0 ]]
then
    printf '%s: Unable to change directory to %s\n' "$0" "${my_dir}" >&2
    exit 3
fi

parm_file="${my_dir}/my_chattr.dat"
fs_dir="/mount/test_fs"

# ------------------------------------------------------------------------------
# Validate configuration
# ------------------------------------------------------------------------------

if [[ ! -r "${parm_file}" ]]
then
    printf '%s: parm file, ''%s'', is either not found nor readable.\n' \
        "$0" "${parm_file}" >&2
    exit 1
fi 

if [[ ! -d "${fs_dir}" ]]
then
    printf '%s: directory, ''%s'', for file systens is either not found nor readable.\n' \
        "$0" "${fs_dir}" >&2
    exit 1
fi 

# ------------------------------------------------------------------------------
# Get a list of target file systems
# ------------------------------------------------------------------------------

fs_list=$(find "${fs_dir}" -type f -name target 2>/dev/null)
if [[ -z "${fs_list}" ]]
then
    printf '%s: No target file systems found.\n' "$0"
    exit 4
fi

# ------------------------------------------------------------------------------
# Do a test for each file system
# ------------------------------------------------------------------------------

cp /dev/null base_results.dat
cp /dev/null mine_results.dat

exec 3<"${parm_file}" 
while read -u3 mode
do 
    for fs_file in ${fs_list}
    do
        case "${fs_file}" in
            *base*)
                chattr "${mode}" "${fs_file}"
                printf '%s: %s %s\n' "${mode}" $(lsattr "${fs_file}") | \
                    sed -re 's/_base//' \
                        -e 's/^\+/ADD_/' \
                        -e 's/^\=/SET_/' \
                        -e 's/^\-/DEL_/' \
                        -e 's!(.*:) (.*) .*/(.*)/target!\3_\1 \2!' \
                        >>base_results.dat
                ;;
            *mine*)
                ./my_chattr "${mode}" "${fs_file}"
                printf '%s: %s %s\n' "${mode}" $(lsattr "${fs_file}") | \
                    sed -re 's/_mine//' \
                        -e 's/^\+/ADD_/' \
                        -e 's/^\=/SET_/' \
                        -e 's/^\-/DEL_/' \
                        -e 's!(.*:) (.*) .*/(.*)/target!\3_\1 \2!' \
                        >>mine_results.dat
                ;;
            *)  printf '%s: invalid file name (%s)\n' "$0" "${fs_file}" >&2
                ;;
        esac
    done
done

# ------------------------------------------------------------------------------
# Compare results
# ------------------------------------------------------------------------------

diff \
    <(sort -k1 base_results.dat) \
    <(sort -k1 mine_results.dat)

popd &>/dev/null
