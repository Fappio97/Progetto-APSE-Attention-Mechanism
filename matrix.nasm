;Prodotto tra matrici memorizzate per colonne su vettori.
;Ricordando le convenzioni usate sul file "matrix.c" traduciamo il codice
;
;void prod(float* A, float* B, float* C, int n){
;	for(int i=0;  i<n;  i = i + p)
;		for(int  j=0;  j<n;  j++){
;			float[p] t = C[i...i+p-1][j];            // t = 0
;			for(int k=0; k<n; k++)
;				t = t + A[i...i+p-1][k]*B[k][j];
;			C[i...i+p-1][j]=t;
;		}
;}
; A[i][k] -> A[i+k*n] -> A[i*dim  + k*n*dim] -> i*dim= EAX,  k*n*dim=ESI
; B[k][j] -> A[k+j*n] -> A[k*dim + j*n*dim] -> k*dim= ECX,  j*n*dim=EDI
; C[i][j] ->   C[i+j*n] -> C[i*dim  + j*n*dim] -> i*dim =  EAX, j*n*dim=EDI
;
;
;
;
;
;
;
;
;
;
;
;
;
;
%include 'sseutils32.nasm'
section .data
	align 16
	A	dd	3.0, 7.0, 11.0, 15.0, 4.0, 8.0, 12.0, 16.0, 5.0, 9.0, 13.0, 17.0, 6.0, 10.0, 14.0, 18.0
	align 16
	B	dd	3.0, 7.0, 11.0, 15.0, 4.0, 8.0, 12.0, 16.0, 5.0, 9.0, 13.0, 17.0, 6.0, 10.0, 14.0, 18.0
	align 16
	C	dd	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	
	n	equ	4
	dim	equ	4
	p	equ	4
	
;A = [ [  3,   4,   5,   6],
;	  [  7,   8,   9, 10],
;	  [11, 12, 13, 14],
;	  [15, 16, 17, 18]]
;B = [ [  3,   4,   5,   6],
;	  [  7,   8,   9, 10],
;	  [11, 12, 13, 14],
;	  [15, 16, 17, 18]]
section .text
	global main
main:
		start
		XOR		EAX, EAX					; i=0
fori:		XOR		EBX, EBX					; j=0		
forj:  	IMUL	EDI,  EBX, n				; EDI=dim*j*n
		MOVAPS	XMM0, [C + EAX + EDI]		; XMM0=C[i...i+p-1][j]
		XOR		ECX, ECX					; k=0
fork:		IMUL	ESI,  ECX, n				; ESI=dim*k*n
		MOVAPS	XMM1, [A + EAX + ESI]		; XMM1=A[i...i+p-1][k]
		MOVSS	XMM2, [B + ECX + EDI]		; XMM2=B[k][J]
		SHUFPS	XMM2, XMM2, 0
		MULPS	XMM1, XMM2				; XMM1=A[i...i+p-1][k]*B[k][j]
		ADDPS	XMM0, XMM1				; XMM0=XMM0+A[i...i+p-1][k]*B[k][j]
		ADD		ECX, dim					; k++, k=k+dim
		CMP	ECX, dim*n				; k<n?
		JL		fork
		MOVAPS	[C + EAX + EDI], XMM0		; C[i...i+p-1][j]=XMM0
		ADD		EBX, dim					; J++, J=J+dim
		CMP	EBX, dim*n				; J<n?
		JL		forj
		ADD		EAX, dim*p				; i=i+p, i=i+dim*p
		CMP	EAX, dim*n				; i<n
		JL		fori
		printps	C, n*n/4
		stop
		
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	stop