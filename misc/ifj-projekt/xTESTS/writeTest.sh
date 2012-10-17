#!/bin/sh
writeTest(){
  exitCode=$1;
  dir=$2
   
  ls -1 $dir | grep --invert-match "^README$" | grep --invert-match "^EMPTY$" | grep "^${exitCode}_"| ( 
    while read file; do
      pgr=`./project "$dir/$file"  2>&1`
      err=$?;
      
      val=`valgrind --leak-check=full --leak-resolution=high -q ./project "$dir/$file"  2>&1`
       
      ref=`./project "$dir/$file" 2>/dev/null | diff - "$dir/ref_out_$file" `
  
      if [ "$err" != "$exitCode" -o "$pgr" != "$val" -o "$ref" != "" ]; then
          printf "TEST FAIL - "  >&2; 
          if [ "$pgr" != "$val" ] ; then
            printf "%s\n" $file >&2
            valgrind --leak-check=full --leak-resolution=high -q ./project "$dir/$file"
          else
            printf "[$err] %s\n" $file >&2
            ./project "$dir/$file" 2>/dev/null | diff - "$dir/ref_out_$file" >&2
          fi
      else  
        echo "TEST OK - $file";
      fi
    done
  )
} 

writeTest 0 $1
writeTest 1 $1
writeTest 2 $1
writeTest 3 $1
writeTest 4 $1

