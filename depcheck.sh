#!/bin/sh 
#######################################################################
#                         Arawak setup script                         #
#######################################################################

# Formatting and text
POINT="0.1.0"
PNAME="Arawak"
TITLE="$PNAME $POINT Setup"
OK="OK"
NO="NO"
SUCCESS="All OK"
FAILURE="Missing dependencies, how unlucky!"

# Commands used as predicates by check()
LOOKPROG='command -v'
LOOKFONT='fc-list | grep'
LOOKLIB='man 3'

# Dependencies
DEP=("urxvt"   \
     "ncurses" \
     "libev"   \
     "fc-list" \
     "unifont" \
     "DejaVu")

# The command to use as a predicate
CHECK=("$LOOKPROG" \
       "$LOOKLIB"  \
       "$LOOKLIB"  \
       "$LOOKPROG" \
       "$LOOKFONT" \
       "$LOOKFONT" \
       "$LOOKFONT")

# Predicate for whether a dependency is missing or not
check() { 
        eval ${CHECK[$1]} $2 '>/dev/null 2>&1' 
}


#######################################################################
echo -e "$TITLE\n"
#######################################################################
for program in ${DEP[@]}; 
do
        echo -en "Searching for \"$program\"\t... "
        check $i $program && echo "$OK" || { echo "$NO"; break; }

        let i=${i:-0}+1
done

#######################################################################
[[ $i -lt ${#DEP[@]} ]] && echo -e "\n$FAILURE" || echo -e "\n$SUCCESS"
#######################################################################

