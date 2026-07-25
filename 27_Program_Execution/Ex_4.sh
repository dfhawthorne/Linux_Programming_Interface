#!/usr/bin/env bash
# -----------------------------------------------------------------------------
# Exercise 27-4:
#
# Runs a test harnass for creating a daemon managed by systemd.
# -----------------------------------------------------------------------------

# Clean up any previous failed runs
for pid in $(ps -C Ex_4 -o pid=);do kill ${pid};done

./Ex_4 $@

echo "Program has terminated. But grandchild is still running"
ps -fC Ex_4
echo "Grandchild is now owned by the following process"
grandchild="$(ps -C Ex_4 -o pid=)"
owner_pid="$(ps -p ${grandchild} -o ppid=)"
ps -fp ${owner_pid}
echo "Grandchild is now killed"
kill -10 ${grandchild}
