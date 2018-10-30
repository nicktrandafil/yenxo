#!/bin/bash

if [ ! -d dcov ]; then mkdir dcov; fi
./test_rproject
lcov --directory . --capture --output-file dcov/coverage.info # capture coverage info
lcov --remove dcov/coverage.info '/usr/*' --output-file dcov/coverage.info # filter out system
genhtml dcov/coverage.info -o dcov
lcov --list dcov/coverage.info
