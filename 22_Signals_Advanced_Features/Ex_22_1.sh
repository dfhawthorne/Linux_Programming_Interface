#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Run test suite for solution to question 22-1.
# ------------------------------------------------------------------------------
# Note that we use "$@" to let each command-line parameter expand to a
# separate word. The quotes around "$@" are essential!
# We need TEMP as the 'eval set --' would nuke the return value of getopt.
# ------------------------------------------------------------------------------

TEMP=$(getopt --options 'vi:l:m' --longoptions 'verbose,idle-for:,log-dir:,mask'  -- "$@")

if [ $? -ne 0 ]; then
    echo 'Terminating...' >&2
    exit 1
fi

# Note the quotes around "$TEMP": they are essential!
eval set -- "$TEMP"
unset TEMP

idle_time=0
verbose_mode=''
log_dir='logs/test1'
mask_interrupt=''

while true; do
    case "$1" in
        '-v'|'--verbose')
            verbose_mode='--verbose'
            shift
            continue
        ;;
        '-i'|'--idle-for')
            idle_time="$2"
            shift 2
            continue
        ;;
        '-l'|'--log-dir')
            log_dir="$2"
            shift 2
            continue
        ;;
        '-m'|'--mask')
            mask_interrupt='--mask'
            shift
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

pid_file=${log_dir}/pid
[[ ${idle_time} -gt 0 ]] \
    && idle_parms="--idle_for=${idle_time}" \
    || idle_parms=""

[[ -f "${pid_file}" ]] && rm -f "${pid_file}"

./Ex_22_1 "${verbose_mode}" \
    "${idle_parms}" \
    "${ignore_list}" \
    "${mask_interrupt}" \
    "--log-file=${log_dir}/Ex_22_1_main.log" \
    >${log_dir}/Ex_22_1_stdout.log \
    2>${log_dir}/Ex_22_1.err &
bg_pid=$!
printf "%d" "${bg_pid}" >"${pid_file}"

sleep 3
pkill --pidfile "${pid_file}" --signal SIGSTOP
sleep 2
pkill --pidfile "${pid_file}" --signal SIGCONT

wait -f "${bg_pid}"
