#!/bin/bash

# Description: This script changes the style format of
#              all the source code of the project.

# Run only on the directory of the script
cd "$(dirname ${BASH_SOURCE[0]})"

# Style and format recursively
astyle --pad-oper --pad-first-paren-out --align-pointer=type --remove-brackets --convert-tabs --max-code-length=100 --style=google --lineend=windows --suffix=none --recursive ../../*.h ../../*.cpp ../../*.c
