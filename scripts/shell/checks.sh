#!/bin/bash

# -- Generate the error message
function error_message()
{
    echo "    ERROR: ${1}"; exit 1
}

# -- Assert a given directory exists
function assert_dir_exists() 
{
    if [ ! -d ${1} ]; then
        error_message "Directory ${1} does not exist"
    fi
}

# -- Assert extension of a file
function assert_file_extension()
{
    case "$1" in
        *.${2}) ;;
        *) error_message "${1} is not an .${2} file" ;;
    esac
}
