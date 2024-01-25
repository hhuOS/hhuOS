#! /bin/bash

global_sum=0

method(){

local sum=0

for file in *; do
  if [[ "${file}" == *([a-zA-Z]).h || "${file}" ==  *([a-zA-Z]).c || "${file}" == *([a-zA-Z]).cpp ]]; then
    printf "%s\n" "$file" 
    local res=$(wc -l "${file}" | grep -o -e "[0-9]*")
    sum=$((${sum} + ${res}))
  fi
  if [ -d ${file} ]; then
    cd ${file}
    method
    cd ..
  fi
done

global_sum=$((${global_sum}+${sum}))

}

method

printf "total lines : %d" "${global_sum}"
