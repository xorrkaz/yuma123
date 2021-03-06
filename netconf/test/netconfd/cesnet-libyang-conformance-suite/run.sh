#!/bin/bash

if [ "$RUN_WITH_CONFD" != "" ] ; then
    # skipped test return value
    exit 77
fi

rm -rf tmp || true
mkdir tmp

LIB_YANG_PATH=../../../../../libyang
test()
{
    declare -a schema_load_fail=("${!2}")
    declare -a data_file_load_fail=("${!3}")

    FAIL=0
    OK=0

    for index in ${!schema_load_fail[*]}
    do
        #printf "%4d: %s\n" $index ${schema_load_fail[$index]}
        if [ ${schema_load_fail[$index]} == "0" ] ; then
            EXPECTED="OK"
        else
            EXPECTED="FAIL"
        fi
        MODULE=${1}/mod$(($index+1)).yang
        echo "Testing EXPECTED=$EXPECTED $MODULE ..."
        /usr/sbin/netconfd --validate-config-only --startup-error=stop --no-startup --modpath=${LIB_YANG_PATH}/tests/conformance/${1}/ --module=${LIB_YANG_PATH}/tests/conformance/${1}/mod$(($index+1)).yang 1>tmp/${1}_mod$(($index+1)).yang.stdout 2>tmp/${1}_mod$(($index+1)).yang.stderr
        RES=$?
        echo "RES="$RES
        if [ "$RES" != "0" ] ; then
            if [ "$EXPECTED" == "OK" ] ; then
                echo "FAIL: ${MODULE}"
                FAIL=$(($FAIL+1))
            else
                echo "OK:   ${MODULE}"
                OK=$(($OK+1))
            fi
        else
            if [ "$EXPECTED" != "OK" ] ; then
                echo "FAIL: ${MODULE}"
                FAIL=$(($FAIL+1))
            else
                echo "OK:   ${MODULE}"
                OK=$(($OK+1))
            fi
        fi
    done
    echo "TOTAL: OKs=$OK FAILs=$FAIL"
    return $FAIL
}
TOTAL_FAILS=0

TEST_DIR="sec5_1"
TEST_SCHEMA_COUNT=6
TEST_SCHEMA_LOAD_FAIL=(1 1 1 1 1 0)
TEST_DATA_FILE_COUNT=0
TEST_DATA_FILE_LOAD_FAIL=()
test $TEST_DIR TEST_SCHEMA_LOAD_FAIL[@] TEST_DATA_FILE_LOAD_FAIL[@]
TOTAL_FAILS=$(($TOTAL_FAILS+$?))

TEST_DIR=sec7_19_1
TEST_SCHEMA_COUNT=15
TEST_SCHEMA_LOAD_FAIL=(1 1 1 0 0 0 0 0 0 0 0 0 0 0 0)
TEST_DATA_FILE_COUNT=0
TEST_DATA_FILE_LOAD_FAIL=()
test $TEST_DIR TEST_SCHEMA_LOAD_FAIL[@] TEST_DATA_FILE_LOAD_FAIL[@]
TOTAL_FAILS=$(($TOTAL_FAILS+$?))

exit $TOTAL_FAILS
