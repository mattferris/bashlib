#!/bin/bash
#
# tupper - A container image manager
#
# Copyright (c) 2018 Matt Ferris <matt@bueller.ca>
# Licensed under BSD 2-clause license
# github.com/mattferris/tupper/blob/master/License.txt
#


#
# error and debugging output
#

# print debug messages
bl_debug() {
    if [[ ! -z "$BL_DEBUG" && $1 -le $BL_DEBUG ]]; then
        shift
        echo "debug: $@" >&2
    fi
}

# print error messages
blerror() {
    local code=$1
    shift
    echo "error: $@" >&2
    exit $code
}

# trap SIGERR
bl_trap_error() {
    echo "error: caught error while running: $BASH_COMMAND" >&2
    exit 4
}
