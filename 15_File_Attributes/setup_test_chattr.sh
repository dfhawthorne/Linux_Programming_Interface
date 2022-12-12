#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Set up the test environment for the my_chattr program.
#
# Two (2) copies of file systems are created for each mkfs.* command found.
# One (1) copy is be manipulated by the real chattr command while the second
# copy is to be manipulated by the my_chattr program. The output of the lsattr
# command against each copy is then compared.
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# Establish the script environment:
# 1. Exit on unset shell variables
# 2. Ensure that the findmnt command is available
# 3. Ensure that the script has been invoked by the root user
# ------------------------------------------------------------------------------

set -o nounset 

if [[ -z "$(command -v findmnt)" ]]
then
    printf 'The findmnt utility must be installed.\nIt is available in the util-linux package.\n' >&2
    exit 1
fi

eff_uid=$(id --user)
if [[ "${eff_uid}" -ne 0 ]]
then
    printf '%s must be run as root\n' "$0" >&2
    exit 2
fi

# ------------------------------------------------------------------------------
# Create the directory for containing:
# 1. File systems
# 2. Results of tests
# ------------------------------------------------------------------------------

dir="test_chattr"
mkdir -p "${dir}"
if [[ ! -d "${dir}" ]]
then
    printf 'Unable to create %s as a directory\n' "${dir}" >&2
    exit 3
fi

# ------------------------------------------------------------------------------
# The following command finds all mkfs.* commands available on this o/s.
# Assumes that all such commands are in the PATH for the ROOT user.
# Be aware not all mkfs commands are files - some are links.
# The following file system types are not used:
# * cramfs: The format of the mkfs.cramfs command is not the standard one
# * fat   : The mount commnad cannot use this type
# * ntfs  : Requires an actual block device, not a simple file.
# ------------------------------------------------------------------------------

fs_list=$(find                            \
            $(echo "${PATH}" | tr : " ")  \
            -name "mkfs.*"                \
            -exec basename {} \;          \
            2>/dev/null |                 \
        sed -re 's/mkfs.//'               \
            -e '/(fat|cramfs|ntfs)/d'     \
        )

# ------------------------------------------------------------------------------
# Create the required file systems using files
# 16MB seems to be a reasonable size of a file system
# ------------------------------------------------------------------------------

fs_size=$((( 16 * 1024 * 1024 ))) 

pushd "${dir}" &>/dev/null

for fs_type in ${fs_list}
do
    for fs_usr in base mine
    do
        fs_data="${fs_type}_${fs_usr}.dat"
        mp="${fs_type}_${fs_usr}"
        if [[ ! -f "${fs_data}" ]]
        then
            fallocate --length "${fs_size}" "${fs_data}"
            mkfs."${fs_type}" "${fs_data}"
        fi
        [[ -d "${mp}" ]]      || \
            mkdir "${mp}"
        findmnt --mountpoint "${mp}" &>/dev/null || \
            mount "${fs_data}" "${mp}"
    done
done

popd &>/dev/null
