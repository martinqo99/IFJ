-- Program 3: Prace s retezci a vestavenymi funkcemi
function main() --Hlavni telo programu
 
 local str1 = "Toto je nejaky text v programu jazyka IFJ11";
 local str2;
 local str3;
 local p;
 

 str2 = str1 .. ", ktery je ulozeny jako ";
 write(str2,"Konec");
 str3 = type(str2);
 str2 = str2 .. str3;
 
 write(str1, "\n", str2, "\n");
 
 p = find(str2, "text");
 write("Pozice retezce \"text\" v retezci str2: ", p, "\n");
 write("Zadejte nejakou posloupnost vsech malych pismen a-h, ");
 write("pricemz se pismena nesmeji v posloupnosti opakovat\n");
 --str1 = read("*l");
 str1 = "dcbaghfe";
 str2 = sort(str1);
 
 --while str2 ~= "abcdefgh" do
  --write("Spatne zadana posloupnost, zkuste znovu\n");
  --str1 = read("*l");
  --str2 = sort(str1);
 --end;
 
 if str2 == "abcdefgh" then
   write("Spravne.\n");
 else
   write("Nespravne.\n");
 end;
 
end;
