-- Fibonacci 
function fibonacci(n)
	local ret;
	local pomA;
	local pomB;
	local pom;
	
	if n < 2 then
		return n;
	else
		pom = n - 1;
		pomA = fibonacci( pom );
		pom = n - 2;
		pomB = fibonacci( pom );
		return pomA + pomB;
	end;
end

function main()
	local input = 1;
	local ret;
	--while input > 0 do
		--input = read( "*n" );
		input = 15;
		ret = fibonacci( input );
		write( "Fibonacciho cislo pre vstup ",input," je ",ret,".\n" );
	--end;	
end;
