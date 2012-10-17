-- Volanie funkcii (zahadzovanie a priradovanie argumentov) a funkcia type()
function lol(abc, def)
	local a;
	local b;
	
	a = type(abc);
	b = type(def);
	
	write("Typy:","\n");
	
	write(a, " ", b, "\n");
end

function main()
	local a;
	local b = "abc";
	
	a = lol();
	
	a = lol(b);
	a = lol(b, 5);
	
end;
