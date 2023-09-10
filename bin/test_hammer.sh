#!/bin/bash

UNIT=bin/unit_hammer
WORKSPACE=/tmp/hammer.$(id -u)
FAILURES=0

error() {
    echo "$@"
    [ -r $WORKSPACE/test ] && (echo; cat $WORKSPACE/test; echo)
    FAILURES=$((FAILURES + 1))
}

cleanup() {
    STATUS=${1:-$FAILURES}
    rm -fr $WORKSPACE
    exit $STATUS
}

export LD_LIBRARY_PATH=$LD_LIBRRARY_PATH:.

mkdir $WORKSPACE

trap "cleanup" EXIT
trap "cleanup 1" INT TERM

echo "Testing hammer..."

if [ ! -x $UNIT ]; then
    echo "Failure: $UNIT is not executable!"
    exit 1
fi

TESTS=$($UNIT 2>&1 | tail -n 1 | awk '{print $1}')
for t in $(seq 0 1); do
    desc=$($UNIT 2>&1 | awk "/$t/ { print \$3 }")

    printf " %-50s ... " "$desc"
    valgrind --leak-check=full $UNIT $t &> $WORKSPACE/test
    if [ $? -ne 0 ] || [ $(awk '/ERROR SUMMARY:/ {print $4}' $WORKSPACE/test | sort -rn | head -n 1) -ne 0 ]; then
	error "Failure"
    else
	echo "Success"
    fi
done

printf " %-50s ... " "hammer_bandwidth"
$UNIT 0 2>&1 > /dev/null | grep -Ei 'Bandwidth: [0-9]+\.[0-9]{2} MB/s' > $WORKSPACE/test
if [ $? -ne 0 ] || [ $(wc -l < $WORKSPACE/test) -ne 5 ]; then
    error "Failure"
else
    echo "Success"
fi

for t in $(seq 2 $TESTS); do
    desc=$($UNIT 2>&1 | awk "/$t/ { print \$3 }")

    printf " %-50s ... " "$desc"
    valgrind --leak-check=full $UNIT $t &> $WORKSPACE/test
    if [ $? -ne 0 ] || [ $(awk '/ERROR SUMMARY:/ {print $4}' $WORKSPACE/test | sort -rn | head -n 1) -ne 0 ]; then
	error "Failure"
    else
	echo "Success"
    fi
done

printf " %-50s ... " "throw_elapsed"
$UNIT 2 2>&1 > /dev/null | grep -Ei 'Elapsed Time: [0-9]+\.[0-9]{2} seconds' > $WORKSPACE/test
if [ $? -ne 0 ] || [ $(wc -l < $WORKSPACE/test) -ne 5 ]; then
    error "Failure"
else
    echo "Success"
fi

printf " %-50s ... " "throw_clones"
strace -e clone $UNIT 2 2>&1 > /dev/null | grep -E 'CLONE_CHILD' > $WORKSPACE/test
if [ $? -ne 0 ] || [ $(wc -l < $WORKSPACE/test) -ne 5 ]; then
    error "Failure"
else
    echo "Success"
fi

TESTS=$(($TESTS + 3))

echo
echo "   Score $(echo "scale=4; ($TESTS - $FAILURES) / $TESTS.0 * 4.5" | bc | awk '{ printf "%0.2f\n", $1 }' ) / 4.50"
printf "  Status "
if [ $FAILURES -eq 0 ]; then
    echo "Success"
else
    echo "Failure"
fi
echo
