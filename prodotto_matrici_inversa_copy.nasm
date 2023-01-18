%include 'sseutils32.nasm'
section .data
	align 16
	A	dd	3.0, 11.0, 19.0, 27.0, 4.0, 12.0, 20.0, 28.0, 5.0, 13.0, 21.0, 29.0, 6.0, 14.0, 22.0, 30.0, 7.0, 15.0, 23.0, 31.0, 8.0, 16.0, 24.0, 32.0, 9.0, 17.0, 25.0, 33.0, 10.0, 18.0, 26.0, 34.0
	align 16
	B	dd	3.0, 11.0, 19.0, 27.0, 4.0, 12.0, 20.0, 28.0, 5.0, 13.0, 21.0, 29.0, 6.0, 14.0, 22.0, 30.0, 7.0, 15.0, 23.0, 31.0, 8.0, 16.0, 24.0, 32.0, 9.0, 17.0, 25.0, 33.0, 10.0, 18.0, 26.0, 34.0 
	align 16
	C	dd	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	
	n	equ	4
	nn 	equ	8
	dim	equ	4
	p	equ	4
section .text			; Sezione contenente il codice macchina

global	main
main:
		start
		XOR		EAX, EAX					; i=0
fori:	XOR		EBX, EBX					; j=0		
forj:  	IMUL	EDI,  EBX, n				; EDI=dim*j*n
		MOVAPS	XMM0, [C + EAX + EDI]		; XMM0=C[i...i+p-1][j]
		XOR		ECX, ECX					; k=0
fork:	IMUL	ESI,  ECX, n				; ESI=dim*k*n
		MOVAPS	XMM1, [A + EAX + ESI]		; XMM1=A[i...i+p-1][k]
		MOVSS	XMM2, [B + ECX + EDI]		; XMM2=B[k][J]
		SHUFPS	XMM2, XMM2, 0
		MULPS	XMM1, XMM2				; XMM1=A[i...i+p-1][k]*B[k][j]
		ADDPS	XMM0, XMM1				; XMM0=XMM0+A[i...i+p-1][k]*B[k][j]
		ADD		ECX, dim					; k++, k=k+dim
		CMP	ECX, dim*n					; k<nn?
		JL		fork
		MOVAPS	[C + EAX + EDI], XMM0		; C[i...i+p-1][j]=XMM0
		ADD		EBX, dim					; J++, J=J+dim
		CMP	EBX, dim*nn				; J<n?
		JL		forj
		ADD		EAX, dim*p				; i=i+p, i=i+dim*p
		CMP	EAX, dim*n				; i<n
		JL		fori
		printps	C, n*n/4
		stop