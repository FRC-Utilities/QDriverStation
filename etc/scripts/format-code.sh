# Description: This script changes the style format of
#              all the source code of the project.

# Style and format the source code recursively
astyle --pad-oper --pad-first-paren-out --align-pointer=type --remove-brackets --convert-tabs --max-code-length=80 --style=google --lineend=windows --suffix=none --recursive ../../*.h ../../*.cpp

# Notify the user that we have finished
echo ""
echo "Code styling complete!"
echo ""
