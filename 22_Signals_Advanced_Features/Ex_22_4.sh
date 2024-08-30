#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Run test suite for solution to question 22-4.
# ------------------------------------------------------------------------------
# Note that we use "$@" to let each command-line parameter expand to a
# separate word. The quotes around "$@" are essential!
# We need TEMP as the 'eval set --' would nuke the return value of getopt.
# ------------------------------------------------------------------------------

TEMP=$(getopt --options 'vl:' --longoptions 'verbose,log-dir:'  -- "$@")

if [ $? -ne 0 ]; then
    echo 'Terminating...' >&2
    exit 1
fi

# Note the quotes around "$TEMP": they are essential!
eval set -- "$TEMP"
unset TEMP

verbose_mode=''
log_dir='logs/test4'

while true; do
    case "$1" in
        '-v'|'--verbose')
            verbose_mode='--verbose'
            shift
            continue
        ;;
        '-l'|'--log-dir')
            log_dir="$2"
            shift 2
            continue
        ;;
        '--')
            shift
            break
        ;;
        *)
            echo 'Internal error!' >&2
            exit 1
        ;;
    esac
done

pushd $(dirname $0) >/dev/null

mkdir -p ${log_dir}

./Ex_22_4 ${verbose_mode} \
    >${log_dir}/Ex_22_4_stdout.log \
    2>${log_dir}/Ex_22_4.err &
sleep 1
kill -s USR2 $!
wait -f $!
