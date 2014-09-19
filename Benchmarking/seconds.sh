#!/bin/bash

end=$(date -d "+ 5 seconds" +%s)        # set end time with "+ 5 seconds"
declare -i count=0

while [ $(date +%s) -lt $end ]; do      # compare current time to end until true
    ./a.out $1
    ((count++))
    printf "working... %s\n" "$count"   # do stuf
done
