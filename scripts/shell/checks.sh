#!/bin/bash

# -- Generate the error message
function error_message()
{
    echo "    ERROR: ${1}"; exit 1
}

function info_message()
{
    echo "    -- ${1}";
}

# -- Assert variable is set
function assert_var_exists()
{
    if [[ -z ${2} ]]; then
        error_message "Variable ${1} is not set."
    fi
}

# -- Assert a given directory exists
function assert_dir_exists() 
{
    if [ ! -d ${1} ]; then
        error_message "Directory ${1} does not exist."
    fi
}

# -- Assert a file exists in a given directory
function assert_file_exists()
{
    if [ ! -f ${1} ]; then
        error_message "File ${1} does not exist."
    fi
}

# -- Assert extension of a file
function assert_file_extension()
{
    case "$1" in
        *.${2}) ;;
        *) error_message "${1} is not an .${2} file." ;;
    esac
}

# -- Check we are in the wanted directory
function assert_location()
{
    if [ "${1}/${2}" != "${3}" ]; then
        error_message "Not inside ${1}/${2}. Cowardly refusing to continue."
        exit 1
    fi
}
