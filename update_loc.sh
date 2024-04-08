#!/usr/bin/zsh
#
a=$(grep -hv "^//" *.cpp | grep -v "^$" | wc -l); 
b=$(grep -hv "^//" *.h | grep -v "^$" | wc -l); c=$((a+b)); 
echo "$(date +'%Y-%m-%d %H:%M:%S'),$c" >> loc.csv
