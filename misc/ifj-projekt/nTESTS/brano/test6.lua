-- Stringy - konkatenacia, radenie, vyhladavanie
function main()

	local a = "abc";
	local b = "def";
	
	write( a..b.."feg" );
	write("\n");
	
	a = sort("qwertyuiop");
	
	write(a, "\n");
	
	b = find(a, "pqrt");
	write( b, "\n" );
	
	b = find(a, "p");
	write( b, "\n" );
	
	b = find(a, "");
	write( b, "\n" );
	
	write("","\n");
	
	write("abc".."", "\n");

end;
