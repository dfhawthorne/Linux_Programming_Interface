#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Run test suite for solution to question 22-3.
# ------------------------------------------------------------------------------
# Note that we use "$@" to let each command-line parameter expand to a
# separate word. The quotes around "$@" are essential!
# We need TEMP as the 'eval set --' would nuke the return value of getopt.
# ------------------------------------------------------------------------------

TEMP=$(getopt --options 'vn:l:s' --longoptions 'verbose,numsigs:,log-dir:,sigsuspend'  -- "$@")

if [ $? -ne 0 ]; then
    echo 'Terminating...' >&2
    exit 1
fi

# Note the quotes around "$TEMP": they are essential!
eval set -- "$TEMP"
unset TEMP

numsigs=0
sigsuspend=''
verbose_mode=''
log_dir='logs/test3'

while true; do
    case "$1" in
        '-v'|'--verbose')
            verbose_mode='--verbose'
            shift
            continue
        ;;
        '-s'|'--sigsuspend')
            sigsuspend='--sigsuspend'
            shift
            continue
        ;;
        '-n'|'--numsigs')
            numsigs="$2"
            shift 2
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

[[ ${numsigs} -gt 0 ]] \
    && sigs_parms="--num-sigs=${numsigs}" \
    || sigs_parms="--num-sigs=1000000"

./sig_speed_sigsuspend "${verbose_mode}" \
    "${sigs_parms}" \
    "${sigsuspend}" \
    "--log-file=${log_dir}/Ex_22_3_main.log" \
    >${log_dir}/Ex_22_3_stdout.log \
    2>${log_dir}/Ex_22_3.err
