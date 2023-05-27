#!/bin/bash

mkdir -p build

set -e
cd build
ninja
something_failed=0
set +e

for test in EssaUtil/Tests/*; do
    if [ -f "$test" ] && [ -x "$test" ]; then
        $test
        if [ $? -eq 1 ]; then
            echo -e "\033[30;41m FAIL \033[m $(basename $test)"
            something_failed=1
        else
            echo -e "\033[30;42m PASS \033[m $(basename $test)"
        fi
    fi
done

exit $something_failed
