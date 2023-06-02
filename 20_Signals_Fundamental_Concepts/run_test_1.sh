#!/usr/bin/env bash
# ------------------------------------------------------------------------------
# Run test suite for solution to question 20-1
# ------------------------------------------------------------------------------
# Note that we use "$@" to let each command-line parameter expand to a
# separate word. The quotes around "$@" are essential!
# We need TEMP as the 'eval set --' would nuke the return value of getopt.
# ------------------------------------------------------------------------------

TEMP=$(getopt --options 'vs:' --longoptions 'verbose,sleep-time:'  -- "$@")

if [ $? -ne 0 ]; then
    echo 'Terminating...' >&2
    exit 1
fi

# Note the quotes around "$TEMP": they are essential!
eval set -- "$TEMP"
unset TEMP

sleep_time=0
verbose_mode=''

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

mkdir -p logs/test1

pid_file=logs/test1/pid
[[ ${sleep_time} -gt 0 ]] \
    && sleep_parms="--sleep-time=${sleep_time}" \
    || sleep_parms=""

[[ -f "${pid_file}" ]] && rm -f "${pid_file}"

./sig_receiver "${verbose_mode}" \
    "${sleep_parms}" \
    --use-sigaction  \
    >logs/test1/sig_receiver.log \
    2>logs/test1/sig_receiver.err &
bg_pid=$!
printf "%d" "${bg_pid}" >"${pid_file}"
./sig_sender  "${verbose_mode}" \
    --pid=${bg_pid} \
    --num-signals=1000000 \
    --signal=10 \
    --last-signal=2 \
    >logs/test1/sig_sender.log \
    2>logs/test1/sig_sender.err

sleep $((( sleep_time + 1 )))
pkill --signal SIGINT --pidfile "${pid_file}"
sleep 1
pkill --signal SIGTERM --pidfile "${pid_file}"

popd >/dev/null

exit 0
