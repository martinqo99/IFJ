#!/bin/sh
exitCode=$1;
dir=$2

export cntOK=0;
export cntFail=0;
  
ls -1 $dir | grep --invert-match "^README$" | { 
  while read file; do
    pgr=`./project "$dir/$file"  2>&1`
    err=$?;
    val=`valgrind --leak-check=full --leak-resolution=high -q ./project "$dir/$file"  2>&1`
  
    if [ "$err" != "$exitCode" -o "$pgr" != "$val" ]; then
      cntFail=$((cntFail + 1))
      printf "TEST FAIL - "  >&2;
      if [ "$pgr" != "$val" ] ; then
        printf "%s\n" $file >&2
        valgrind --leak-check=full --leak-resolution=high -q ./project "$dir/$file"
      else
        printf "[$err] %s\n" $file >&2 
      fi
    else  
      echo "TEST OK - $file";
      cntOk=$((cntOk + 1))
    fi
  done
}

cnt=$((cntOk + cntFail))
#printf "OK \t [%d / %d]\n" $cnt $cntOk >&2
#printf "FAIL \t [%d / %d]\n" $cnt $cntFail >&2