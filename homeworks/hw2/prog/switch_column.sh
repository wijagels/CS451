#! /usr/bin/env bash
echo "===" >> $1
awk ' {
if($0 != "===") {
    if(NF >= "'"$3"'" && NF >= "'"$2"'") {
        col = $"'"$2"'";
        $"'"$2"'" = $"'"$3"'";
        $"'"$3"'" = col;
        print >> "'"$1"'";
    } else {
    print >> "'"$1"'";
    }
}
}' $1
