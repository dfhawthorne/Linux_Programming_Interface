#!/bin/bash
# ------------------------------------------------------------------------------
# Exercise 3-1.
# What is the significance of the values for magic2 in the reboot() call.
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# Manual page for reboot(2) says:
# This  system call fail (with the error EINVAL) unless magic equals
# LINUX_REBOOT_MAGIC1 (that is, 0xfee1dead) and magic2 equals
# LINUX_REBOOT_MAGIC2 (that is, 672274793).  However, since 2.1.17 also
# LINUX_REBOOT_MAGIC2A (that is, 85072278) and since 2.1.97 also
# LINUX_REBOOT_MAGIC2B (that is, 369367448) and since 2.5.71 also
# LINUX_REBOOT_MAGIC2C (that is, 537993216) are permitted  as values for magic2.
# (The hexadecimal values of these constants are meaningful.)
# ------------------------------------------------------------------------------

declare -a magic

magic[0]="LINUX_REBOOT_MAGIC2 672274793 original"
magic[1]="LINUX_REBOOT_MAGIC2A 85072278 2.1.17"
magic[2]="LINUX_REBOOT_MAGIC2B 369367448 2.1.97"
magic[3]="LINUX_REBOOT_MAGIC2C 537993216 2.5.71"


for mv in "${magic[@]}"
do parts=( ${mv} )
printf "%-20s: %10d = %08X (%s)\n" ${parts[0]} ${parts[1]} ${parts[1]} ${parts[2]}
done
    