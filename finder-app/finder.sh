#! /bin/sh

filesdir=$1 
searchstr=$2

if [ $# -lt 2 ]; then 
    echo "To few arguments, should be 2. Got $#"
    exit 1
elif [ $# -gt 2 ]; then
    echo "To many argument, should be 2. Got $#"
    exit 1
fi

if [ ! -d $filesdir ]; then 
    echo "Files directory given: $filesdir, does not exist."
    exit 1
fi

number_files=$(grep -r $searchstr $filesdir/* | cut -f1 -d":" | uniq | wc -l)
number_matched_lines=$(grep -r $searchstr $filesdir/* | wc -l)

echo "The number of files are $number_files and the number of matching lines are $number_matched_lines"
    
