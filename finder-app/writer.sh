#! /bin/bash

writefile=$1
writestr=$2


if [ $# -lt 2 ]; then 
    echo "To few arguments, should be 2. Got $#"
    exit 1
elif [ $# -gt 2 ]; then
    echo "To many argument, should be 2. Got $#"
    exit 1
fi

if [ ! -f $writefile ]; then 
    echo "File given: $writefile, did not exist. Creating."
    dirname=$(dirname $writefile)
    mkdir -p $dirname
    err=$?
    if [ ! $err -eq 0 ]; then
        echo "Error making directory, mkdir exited with: $err"
        exit 1
    fi
    touch $writefile
    err=$?
    if [ ! $err -eq 0 ]; then
        echo "Error creating file, touch exited with: $err"
        exit 1
    fi
else
    echo "File given: $writefile already exists. Overwriting."
    rm $writefile
    err=$?
    if [ ! $err -eq 0 ]; then
        echo "Error removing file, rm exited with: $err"
        exit 1
    fi
    touch $writefile
    err=$?
    if [ ! $err -eq 0 ]; then
        echo "Error creating file, touch exited with: $err"
        exit 1
    fi
fi

echo $writestr > $writefile  

err=$?
if [ ! $err -eq 0 ]; then
    echo "Error writing to file, exited with: $err"
    exit 1
fi
