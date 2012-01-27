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

msg=0

# If the SRC directory is not clean, run 'make clean' to get rid of the object 
# files in ./SRC
if [ -e ./SRC/cucumber.o ]; then
        msg=1
        echo -e "${LRED}T${LCYAN}I${LGREEN}G${LBLUE}T${YELLOW}E${LMAGENTA}N${LRED}'${WHITE}UP${DEFAULT}"
        make distclean
fi

# If the build directory is not clean
if [ -e depcomp ]; then

        # If the SRC directory was clean
        if [ $msg -eq 0 ]; then
                echo -e "${LRED}T${LCYAN}I${LGREEN}G${LBLUE}T${YELLOW}E${LMAGENTA}N${LRED}'${WHITE}UP${DEFAULT}"
        fi
        # Remove any files or directories indicated in .gitignore
        git clean -X -d -f
fi



# If the build directory is clean and the SRC directory is also clean
echo -e "${LRED}L${LCYAN}O${LGREEN}O${LBLUE}K${YELLOW}I${LMAGENTA}N${LRED}'${WHITE}TIGHT${DEFAULT}"
