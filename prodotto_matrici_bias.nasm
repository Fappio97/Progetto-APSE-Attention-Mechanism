%include 'sseutils32.nasm'
section .data
	; align 16
	; A	dd	3.0, 7.0, 11.0, 15.0, 4.0, 8.0, 12.0, 16.0, 5.0, 9.0, 13.0, 17.0, 6.0, 10.0, 14.0, 18.0
	; align 16
	; B	dd	3.0, 11.0, 19.0, 27.0, 4.0, 12.0, 20.0, 28.0, 5.0, 13.0, 21.0, 29.0, 6.0, 14.0, 22.0, 30.0, 7.0, 15.0, 23.0, 31.0, 8.0, 16.0, 24.0, 32.0, 9.0, 17.0, 25.0, 33.0, 10.0, 18.0, 26.0, 34.0 
	; align 16
	; C	dd	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	
	; n	equ	4
	; nn 	equ	8
	; dim	equ	4
	; p	equ	4
	;prodottoMatriciBias(MATRIX Q, MATRIX DS, MATRIX pesi, VECTOR bias, int avanza, int n, int d, int nn);
	q			equ	8
	data		equ	12
	wq			equ	16
	bq			equ	20
	avanza		equ	24
	n			equ 28		
	d			equ 32
	nn			equ 36

	dim		equ	4
	p		equ	4

section .text			; Sezione contenente il codice macchina

global	prodottoMatriciBias
prodottoMatriciBias:
		;
		; sequenza di ingresso nella funzione
		;
		PUSH	EBP
		MOV		EBP, ESP
		PUSH 	EBX
		PUSH	ESI
		PUSH 	EDI
		;
		; lettura dei parametri dal record di attivazione
		;
		MOVSS 	XMM3, [EBP + data]
		MOVSS	XMM4, [EBP + q]
		MOVSS	XMM5, [EBP + wq]
		MOV		EDX, [EBP + n]
		MOVSS	XMM6, [EBP + bq]
		;
		; corpo della funzione
		;
		XOR		EAX, EAX			; i=EAX=0
fori:	XOR		EBX, EBX			; j=EBX=0		
forj:  	MOV		EDI, EDX			; EDI=dim*j*n
		IMUL	EDI, EBX			; EDI=dim*j*n
		MOVD	XMM7, EDX			; XMM7=n, salvo su XMM7 n
		MOVD	EDX, XMM4			; XMM3=C
		ADD		EDX, EAX			; EDX = C + EAX
		MOVAPS	XMM0, [EDX + EDI]	; XMM0=C[i...i+p-1][j]
		MOVD	EDX, XMM7			; rimetto su EDX n
		XOR		ECX, ECX			; x=ECX=0
forx:	MOV		ESI, EDX			; ESI=dim*x*n
		IMUL	ESI, ECX			; ESI=dim*x*n
		MOVD	EDX, XMM3			; XMM4=A, carico su EDX A
		ADD		EDX, EAX			; EDX = A + EAX
		MOVAPS	XMM1, [EDX + ESI]	; XMM1=A[i...i+p-1][x]
		MOVD	EDX, XMM5			; XMM5 = B, EDX = B
		ADD		EDX, ECX			; EDX = B + ECX
		MOVSS	XMM2, [EDX + EDI]	; XMM2=B[x][j]
		MOVD	EDX, XMM7			; EDX = n
		SHUFPS	XMM2, XMM2, 0		
		MULPS	XMM1, XMM2			; XMM1=A[i...i+p-1][k]*B[k][j]
		ADDPS	XMM0, XMM1			; XMM0=XMM0+A[i...i+p-1][k]*B[k][j]
		ADD		ECX, dim			; x++, x=x+dim
		MOV		EDX, [EBP + d]
		IMUL	EDX, EDX, dim		; EDX = d *dim
		CMP		ECX, EDX			; x<d?
		MOVD	EDX, XMM7			; EDX = n
		JL		forx
		MOVD	EDX, XMM4			; EDX = C
		ADD		EDX, EAX			; EDX = C + EAX
		MOVAPS	[EDX + EDI], XMM0	; C[i...i+p-1][j]=XMM0
		ADD		EBX, dim			; j++, j=j+dim
		MOV		EDX, [EBP + nn]		; EDX = nn
		IMUL	EDX, EDX, dim		; EDX = nn * dim
		CMP		EBX, EDX			; j<nn?
		MOVD	EDX, XMM7			; EDX = n
		JL		forj
		ADD		EAX, dim*p			; i=i+p, i=i+dim*p
		IMUL	EDX, EDX, dim		; EDX = n*dim
		CMP		EAX, EDX			; i<n
		MOVD	EDX, XMM7			; EDX = n
		JL		fori
		MOVD	EDX, XMM4			; EDX = C
 		printps	EDX, n*nn/4
		;
		; sequenza di uscita della funzione [OPPURE stop]
		;
		pop		edi
		pop		esi
		pop		ebx
		pop		EAX
		pop		ECX
		POP		EDX
		mov		esp, ebp
		pop 	ebp
		ret