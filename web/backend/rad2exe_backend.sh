#!/bin/bash

# The server, just like the build script, need a few environment variables to be set:
# $WATCOM needs to point to the root directory of your Open Watcom installation
# Open Watcom's wcl program for your platform must be in the $PATH (that's $WATCOM/binl64
#   on a linux-x86_64
# $INCLUDE must point to the DOS headers of Open Watcom ($WATCOM/h, typically)
#
# I've got all of this in a file that I source like this:

. ~/owenv.sh

node rad2exe.js
