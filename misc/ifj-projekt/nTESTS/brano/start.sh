echo "[ TESTY - KOREKTNE ]"
for i in `seq 1 7`
do
	rm tmp > /dev/null
	echo "*** TEST $i ***"
	cat "test$i.lua" | head -n 1
	../ifj11 "test$i.lua" > tmp
	vratil="$?"
	if [ "$vratil" -ne 0 ] ; then
		echo "Program vratil $vratil namiesto 0."
	fi
	diff tmp "test$i.out"
	
done

echo "[ TESTY - LEX. CHYBA ]"
for i in `seq 1 1`
do
	rm tmp > /dev/null
	echo "*** TEST $i ***"
	cat "lex-test$i.lua" | head -n 1
	../ifj11 "test$i.lua" > tmp
	vratil="$?"
	if [ "$vratil" -ne 1 ] ; then
		echo "Program vratil $vratil namiesto 1."
	fi
	diff tmp "test$i.out"
	
done
