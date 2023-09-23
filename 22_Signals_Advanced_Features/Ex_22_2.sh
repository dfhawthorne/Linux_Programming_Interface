#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Run test suite for solution to question 22-2.
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

./Ex_22_2 "${verbose_mode}" \
    "${idle_parms}" \
    "${ignore_list}" \
    "--log-file=${log_dir}/Ex_22_2_main.log" \
    >${log_dir}/Ex_22_2_stdout.log \
    2>${log_dir}/Ex_22_2.err &
bg_pid=$!
printf "%d" "${bg_pid}" >"${pid_file}"

sleep 3
for sig in {1..64}
do
    case "${sig}" in
        # Skip the following signals:
        # SIGKILL   - Killed
        # SIGSEGV   - Segmentation fault
        # SIGTSTP   - Stopped (signal)
        # SIGVTALRM - Virtual timer expired
         9|11|19|26|32|33)
            continue
            ;;
        *)
            pkill --pidfile "${pid_file}" --signal "${sig}"
            ;;
    esac
done
sleep 2

wait -f "${bg_pid}"
