#!/bin/sh
###############################
# Tidy up the build directory #
###############################
LBLUE="\033[1;34m"
LGREEN="\033[1;32m"
LCYAN="\033[1;36m"
LRED="\033[1;31m"
LMAGENTA="\033[1;35m"
YELLOW="\033[1;33m"
WHITE="\033[1;37m"
DEFAULT="\033[0m"

WORKING_DIR=${PWD##*/}

echo -e "${LRED}T${LCYAN}I${LGREEN}G${LBLUE}T${YELLOW}E${LMAGENTA}N${LRED}'${WHITE}UP${DEFAULT}"

make distclean       2>/dev/null
rm -r SRC/.deps      2>/dev/null
rm SRC/gmon.out      2>/dev/null
rm depcomp           2>/dev/null
rm install-sh        2>/dev/null
rm missing           2>/dev/null
rm aclocal.m4        2>/dev/null
rm -r autom4te.cache 2>/dev/null

git clean -X -d -f # Remove any files or directories indicated in .gitignore
git gc # Clean junk files from repo, recompress and optimize
gitstats ${PWD} ~/gitstats_${PWD##*/}

echo -e "${LRED}L${LCYAN}O${LGREEN}O${LBLUE}K${YELLOW}I${LMAGENTA}N${LRED}'${WHITE}TIGHT${DEFAULT}"
