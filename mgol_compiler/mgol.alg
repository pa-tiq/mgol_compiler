inicio
	varinicio
		A lit;
		B int;
		C real;
		D int;		
	varfim;
	escreva "Digite B:";
	leia B;
	escreva "Digite A:";
	leia A;
	se(B>0)
		entao
		se(D<=2)
			entao
			escreva "B esta entre 2 e 4";
		fimse
	fimse
	B<-B+4;
	B<-B+2;
	B<-B+D;
	D<-B;
	C<-50.0;
	escreva "\nB=\n";
	escreva D;
	escreva "\n";
	escreva B;
	escreva "\n";
	escrev A;
fim