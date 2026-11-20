#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Do a verbose build of Ex_2
# ------------------------------------------------------------------------------

touch Ex_2.c unbalanced.c

VERBOSE=1 UNTHREADED=1 make test_2  >Ex_2.log 2>Ex_2.err
# VERBOSE=1 make test_2  >Ex_2.log 2>Ex_2.err

sed -nre '/MUTEX/s/^.*(0x[0-9a-f]+).*$/\1/p' Ex_2.log | \
sort | \
uniq -c

echo "=========================================================================="

sed -nre 's/^.*([Rr]elease(d)?|[Ll]ock(ed)?|[Uu]nlock(ed)?).*$/\1/p' Ex_2.log | \
sort | \
uniq -c

echo "=========================================================================="

sed -nre 's/^.*([Rr]elease(d)?|[Ll]ock(ed)?|[Uu]nlock(ed)?).*(0x[0-9a-f]+).*$/\5 \1/p' Ex_2.log | \
sort | \
uniq -c

echo "=========================================================================="

sed -nre 's/^([A-Za-z_][A-Za-z0-9_]+).*([Rr]elease(d)?|[Ll]ock(ed)?|[Uu]nlock(ed)?).*(0x[0-9a-f]+).*$/\6 \1 \2/p' Ex_2.log | \
sort | \
uniq -c

echo "=========================================================================="
