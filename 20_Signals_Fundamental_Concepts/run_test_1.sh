#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Run test suite for solution to questions 20-1, 20-2, and 20-3.
# ------------------------------------------------------------------------------
# Note that we use "$@" to let each command-line parameter expand to a
# separate word. The quotes around "$@" are essential!
# We need TEMP as the 'eval set --' would nuke the return value of getopt.
# ------------------------------------------------------------------------------

TEMP=$(getopt --options 'vs:i:l:nrw' --longoptions 'verbose,sleep-time:,ignore:,log-dir:,nodefer,reset-hand,slow-signal'  -- "$@")

if [ $? -ne 0 ]; then
    echo 'Terminating...' >&2
    exit 1
fi

# Note the quotes around "$TEMP": they are essential!
eval set -- "$TEMP"
unset TEMP

sleep_time=0
verbose_mode=''
ignore_list=''
log_dir='logs/test1'
nodefer=''
resethand=''
slow_signal=1

while true; do
    case "$1" in
        '-v'|'--verbose')
            verbose_mode='--verbose'
            shift
            continue
        ;;
        '-s'|'--sleep-time')
            sleep_time="$2"
            shift 2
            continue
        ;;
        '-i'|'--ignore')
            ignore_list="--ignore=$2"
            shift 2
            continue
        ;;
        '-l'|'--log-dir')
            log_dir="$2"
            shift 2
            continue
        ;;
        '-r'|'--reset-hand')
            resethand='--reset-hand'
            shift
            continue
        ;;
        '-n'|'--nodefer')
            nodefer='--nodefer'
            shift
            continue
        ;;
        '-w'|'--slow-signal')
            slow_signal=0
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
[[ ${sleep_time} -gt 0 ]] \
    && sleep_parms="--sleep-time=${sleep_time}" \
    || sleep_parms=""

[[ -f "${pid_file}" ]] && rm -f "${pid_file}"

./sig_receiver "${verbose_mode}" \
    "${sleep_parms}" \
    "${ignore_list}" \
    --use-sigaction  \
    "${resethand}" \
    "${nodefer}" \
    >${log_dir}/sig_receiver.log \
    2>${log_dir}/sig_receiver.err &
bg_pid=$!
printf "%d" "${bg_pid}" >"${pid_file}"
if [[ ${slow_signal} == 0 ]]
then
    for i in 1..100
    do
        pkill --signal 10 --pidfile "${pid_file}"
    done
    pkill --signal 2 --pidfile "${pid_file}"
else
    ./sig_sender  "${verbose_mode}" \
        --pid=${bg_pid} \
        --num-signals=1000000 \
        --signal=10 \
        --last-signal=2 \
        >${log_dir}/sig_sender.log \
        2>${log_dir}/sig_sender.err
fi

sleep $((( sleep_time + 1 )))
pkill --signal SIGINT --pidfile "${pid_file}"
sleep 1
pkill --signal SIGTERM --pidfile "${pid_file}"

popd >/dev/null

exit 0
