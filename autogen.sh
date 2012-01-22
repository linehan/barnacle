#! /bin/sh

# Rename the HISTORY file to ChangeLog, so autotools doesn't have a cow
mv HISTORY ChangeLog

# Run the autotools routine
aclocal \
&& automake --add-missing \
&& autoconf

# Rename ChangeLog back to HISTORY
mv ChangeLog HISTORY
