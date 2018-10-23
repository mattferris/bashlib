#!/bin/bash
#
# bashlib - Common includes for bash scripts
#
# Copyright (c) 2018 Matt Ferris <matt@bueller.ca>
# Licensed under BSD 2-clause license
# github.com/mattferris/bashlib/blob/master/License.txt
#


#
# argument parsing
#

# return next parsed argument
bl_getopt_required=()
bl_getopt_optional=()
bl_getopt_matched=()
bl_getopt_args=($@)
bl_getopt_name=""
bl_getopt_value=""
bl_getopt_pos=0
bl_getopt_posargs=()
bl_getopt_return=0
bl_getopt() {

    # bail if no remaining arguments
    if [[ ${#bl_getopt_args[@]} = 0 ]]; then
        bl_debug 4 "bl_getopt: no more arguments"
        bl_getopt_return=1
        return 1
    fi

    # determine next argument
    local cur=${bl_getopt_args[0]}
    if [[ $cur = "--" ]]; then
        bl_debug 4 "bl_getopt: found --, end of arguments"
        bl_getopt_return=1
        return 1
    elif [[ ${cur:0:2} = "--" ]]; then
        bl_getopt_name=${cur#--}
        bl_debug 4 "bl_getopt: found option: $cur"
    else
        local positional=1
        let bl_getopt_pos=++bl_getopt_pos
        bl_getopt_name=$bl_getopt_pos
        bl_getopt_value=$cur
        bl_debug 4 "bl_getopt: found positional argument \$$bl_getopt_pos: $bl_getopt_value"
    fi

    bl_getopt_args=(${bl_getopt_args[@]:1})

    local matched=0
    local optpsec=""

    # have we already matched this argument?
    for i in ${!bl_getopt_matched[@]}; do
        if [[ ${bl_getopt_matched[$i]} = $bl_getopt_name ]]; then
            bl_debug 4 "tup_getopt: duplicate option: $bl_getopt_name"
            bl_getopt_return=3
            return 3
        fi
    done

    # is required?
    for i in ${!bl_getopt_required[@]}; do
        local name=${bl_getopt_required[$i]} 
        if [[ $name = $bl_getopt_name || $name = "${bl_getopt_name}:" ]]; then
            local matched=1
            local optspec=${bl_getopt_required[$i]}
            bl_getopt_matched=(${bl_getopt_matched[@]} $bl_getopt_name)
            unset bl_getopt_required[$i]
            bl_debug 4 "bl_getopt: required option: $bl_getopt_name"
        fi
    done

    # is optional?
    if [[ ! $matched = 1 ]]; then
        for i in ${!bl_getopt_optional[@]}; do
            local name=${bl_getopt_optional[$i]}
            if [[ $name = $bl_getopt_name || $name = "${bl_getopt_name}:" ]]; then
                local matched=1
                local optspec=${bl_getopt_optional[$i]}
                bl_getopt_matched=(${bl_getopt_matched[@]} $bl_getopt_name)
                unset bl_getopt_optional[$i]
                bl_debug 4 "bl_getopt: optional option: $bl_getopt_name"
            elif [[ $positional = 1 && $name = "@" ]]; then
                local matched=1
                bl_getopt_matched=(${bl_getopt_matched[@]} $bl_getopt_name)
                bl_debug 4 "bl_getopt: wildcard positional argument: $bl_getopt_name"
            fi
        done
    fi

    # did we match an undefined option?
    if [[ $matched = 0 ]]; then
        if [[ $positional = 1 ]]; then
            bl_debug 4 "bl_getopt: unexpected positional argument: $bl_getopt_value"
            bl_getopt_return=5
            return 5
        else
            bl_debug 4 "bl_getopt: option is undefined: $bl_getopt_name"
            bl_getopt_return=4
            return 4
        fi
    fi

    # requires an additional argument?
    if [[ ${optspec: -1} = ":" ]]; then
        bl_debug 4 "bl_getopt: option requires value: $bl_getopt_name"
        if [[ ${#bl_getopt_args[@]} = 0 ]]; then
            bl_debug 4 "bl_getopt: unable to get option value: exhausted argument list"
            bl_getopt_return=2
            return 2
        fi
        bl_getopt_value=${bl_getopt_args[0]}
        bl_getopt_args=(${bl_getopt_args[@]:1})
    fi 

    if [[ $positional = 1 ]]; then
        bl_getopt_posargs=(${bl_getopt_posargs[@]} $bl_getopt_value)
        bl_debug 4 "bl_getopt: saving positional argument $bl_getopt_name: $bl_getopt_value"
    fi

    bl_getopt_return=0
    return 0
}

# handle bl_getopt errors
bl_getopt_error() {
    case $bl_getopt_return in
        0) ;;
        1) ;;
        2) bl_error 1 "missing value for option: --$bl_getopt_name" ;;
        3) bl_error 1 "option already specified: --$bl_getopt_name" ;;
        4) bl_error 2 "invalid option specified: --$bl_getopt_name" ;;
        5) bl_error 2 "unexpected argument: $bl_getopt_value" ;;
        *) bl_error 4 "encountered unknown error while parsing arguments: code $bl_getopt_return" ;;
    esac
}
