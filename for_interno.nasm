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
	C		equ	8
	A		equ	12
	B		equ 16
	n		equ	20
	nn		equ	24
	i		equ 28
	j   	euq 32

	dim		equ	4
	p		equ	4
	
;A = [ [  3,   4,   5,   6],
;	  [  7,   8,   9, 10],
;	  [11, 12, 13, 14],
;	  [15, 16, 17, 18]]
;B = [ [ 3, 4, 5, 6, 7, 8, 9, 10],
;	  [ 11, 12, 13, 14, 15, 16, 17, 18],
;	  [19, 20, 21, 22, 23, 24, 25, 26],
;	  [27, 28, 29, 30, 31, 32, 33, 34]]

section .text			; Sezione contenente il codice macchina

global	forInterno
forInterno:
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
		MOVSS 	XMM3, [EBP + C]
		MOVSS	XMM4, [EBP + A]
		MOVSS	XMM5, [EBP + B]
		MOV		EDX, [EBP + n]
		MOV		EDI, [EBP + nn]
		MOV		EAX, [EBP + i]
		MOV		EBX, [EBP + j]
		;
		; corpo della funzione
		;
		XOR		ECX, ECX			; x=ECX=0
		IMUL	EDX, EAX
		MOVAPS	XMM0, [EDX + EBX]	; XMM0=C[i...i+p-1][j]
forx:	MOV		ESI, EDI
		IMUL	ESI, EAX
		MOVAPS	XMM1, [EBX + ESI]	; XMM1=A[i...i+p-1][x]



forx:	MOV		ESI, [EBP + n]		; ESI=dim*x*nn
		IMUL	ESI, EDX			; ESI=dim*x*nn
		MOVD	EDX, XMM4			; XMM4=A, carico su EDX A
		ADD		EDX, EAX			; EDX = A + EAX
		MOVAPS	XMM1, [EDX + ESI]	; XMM1=A[i...i+p-1][x]
		MOVD	EDX, XMM5			; XMM5 = B, EDX = B
		ADD		EDX, ECX			; EDX = B + ECX
		MOV		EDI, [EBP + n]		; EDI=dim*j*n
		IMUL	EDI, EBX			; EDI=dim*j*n
		MOVSS	XMM2, [EDX + EDI]	; XMM2=B[x][j]
		MOVD	EDX, XMM7			; EDX = n
		SHUFPS	XMM2, XMM2, 0		
		MULPS	XMM1, XMM2			; XMM1=A[i...i+p-1][k]*B[k][j]
		ADDPS	XMM0, XMM1			; XMM0=XMM0+A[i...i+p-1][k]*B[k][j]
		ADD		ECX, dim			; x++, x=x+dim
		MOVD 	EDX, XMM6			; EDX = nn
		IMUL	EDX, EDX, dim		; EDX = nn *dim
		CMP		ECX, EDX			; x<nn?
		MOVD	EDX, XMM7			; EDX = n
		JL		forx
		MOV		EDI, [EBP + nn]		; EDI=dim*j*n
		IMUL	EDI, EBX			; EDI=dim*j*n
		MOVD	EDX, XMM3			; EDX = C
		ADD		EDX, EAX			; EDX = C + EAX
		MOVAPS	[EDX + EDI], XMM0	; C[i...i+p-1][j]=XMM0
		ADD		EBX, dim			; j++, j=j+dim
		MOVD	EDX, XMM7			; EDX = n
		IMUL	EDX, EDX, dim		; EDX = n * dim
		CMP		EBX, EDX			; j<n?
		MOVD	EDX, XMM7			; EDX = n
		JL		forj
		ADD		EAX, dim*p			; i=i+p, i=i+dim*p
		IMUL	EDX, EDX, dim		; EDX = n*dim
		CMP		EAX, EDX			; i<n
		MOVD	EDX, XMM7			; EDX = n
		JL		forx
		;MOVD	EDX, XMM3			; EDX = C
 		;printps	EDX, n*nn/4
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