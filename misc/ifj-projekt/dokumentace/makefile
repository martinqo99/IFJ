#
# Projekt: IFJ
# Autor:   Vendula Poncová
# Datum:   2011
# 
 
NAME=dokumentace

all: $(NAME).pdf

$(NAME).pdf: $(NAME).dvi
	dvipdf $(NAME).dvi

$(NAME).dvi: $(NAME).tex
	latex $(NAME).tex
	latex $(NAME).tex

ps:
	latex $(NAME).tex
	latex $(NAME).tex
	dvips -t a4 $(NAME).dvi
	ps2pdf $(NAME).ps

vlnka:
	vlna $(NAME).tex

convert:
	convert img/LA-verze2.png img/LA.eps

clean:
	rm -rf $(NAME).aux $(NAME).log $(NAME).out $(NAME).toc $(NAME).dvi *~
