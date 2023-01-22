; ---------------------------------------------------------
; Regressione con istruzioni SSE a 32 bit
; ---------------------------------------------------------
; F. Angiulli
; 23/11/2017
;

;
; Software necessario per l'esecuzione:
;
;     NASM (www.nasm.us)
;     GCC (gcc.gnu.org)
;
; entrambi sono disponibili come pacchetti software 
; installabili mediante il packaging tool del sistema 
; operativo; per esempio, su Ubuntu, mediante i comandi:
;
;     sudo apt-get install nasm
;     sudo apt-get install gcc
;
; potrebbe essere necessario installare le seguenti librerie:
;
;     sudo apt-get install lib32gcc-4.8-dev (o altra versione)
;     sudo apt-get install libc6-dev-i386
;
; Per generare file oggetto:
;
;     nasm -f elf32 fss32.nasm 
;
%include "sseutils32.nasm"

section .data			; Sezione contenente dati inizializzati
	starta	equ	8
	startb	equ	12
	nn		equ	16
	nni		equ	20
	nnj		equ	24
	ris		equ	28

	C		equ	8
	A		equ	12
	B		equ 16
	n		equ	20
	nn2		equ	24

	dim		equ	4
	p		equ	4
section .bss			; Sezione contenente dati non inizializzati
	alignb 16
	wk		resd		1

section .text			; Sezione contenente il codice macchina
	global prodotto
prodotto:
	; sequenza di ingresso nella funzione [OPPURE start]
	push	ebp
	mov		ebp, esp
	push 	ebx
	push	esi
	push 	edi
	
	; lettura dei parametri
	MOV	EAX, [EBP + starta]
	MOV	EBX, [EBP + startb]
	MOV	ECX, [EBP + nni]
	MOV	EDX, [EBP + nnj]
	SHL		ECX, 2				;nni*4 (dimensione operandi)
	SHL		EDX, 2				;nnj*4 (dimensione operandi)
	ADD		EAX, ECX				;A[nn_x_i]
	ADD		EBX, EDX			;B[nn_x_j]
	;a questo punto volendo   possibile riutilizzare i registri ECX e EDX
	MOV	EDI, [EBP + nn]
	
	;corpo della funzione
	XORPS	XMM0, XMM0				;a=0;
	XOR		ESI, ESI				;contatore
c:	MOVUPS	XMM1, [EAX+ESI*4]
	MULPS	XMM1, [EBX+ESI*4]
	HADDPS	XMM1, XMM1
	HADDPS	XMM1, XMM1
	ADDSS	XMM0, XMM1
	ADD		ESI, 4				;4 elementi alla volta
	CMP		ESI, EDI
	JL		c
	MOV 	ECX, [EBP + ris]
	MOVSS	[ECX], XMM0
	;MOVSS	[C], XMM0
	;printss	C
	; sequenza di uscita della funzione [OPPURE stop]
	;
	pop		edi
	pop		esi
	pop		ebx
	mov		esp, ebp
	pop 	ebp
	ret

	global prodottoMatrici
prodottoMatrici:
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
		MOV		EDX, [EBP + n]
		;
		; corpo della funzione
		;
		XOR		EAX, EAX			; i=EAX=0
fori:	XOR		EBX, EBX			; j=EBX=0		
forj:  	MOV		EDI, EBX			; EDI=dim*j
		IMUL	EDI, EDX			; EDI=dim*j*n
		MOV		EDX, [EBP + C]		; XMM3=C
		ADD		EDX, EAX			; EDX = C + EAX
		MOVAPS	XMM0, [EDX + EDI]	; XMM0=C[i...i+p-1][j]
		MOV		EDX, [EBP + n]		; rimetto su EDX n
		XOR		ECX, ECX			; x=ECX=0
forx:	MOV		ESI, ECX			; ESI=dim*x*n
		IMUL	ESI, EDX			; ESI=dim*x*n
		MOV		EDX, [EBP + A]		; XMM4=A, carico su EDX A
		ADD		EDX, EAX			; EDX = A + EAX
		MOVAPS	XMM1, [EDX + ESI]	; XMM1=A[i...i+p-1][x]
		MOV		EDX, [EBP + B]		; XMM5 = B, EDX = B
		ADD		EDX, ECX			; EDX = B + ECX
		MOVSS	XMM2, [EDX + EDI]	; XMM2=B[x][j]
		MOV		EDX, [EBP + n]		; EDX = n
		SHUFPS	XMM2, XMM2, 0		
		MULPS	XMM1, XMM2			; XMM1=A[i...i+p-1][k]*B[k][j]
		ADDPS	XMM0, XMM1			; XMM0=XMM0+A[i...i+p-1][k]*B[k][j]
		ADD		ECX, dim			; x++, x=x+dim
		IMUL	EDX, EDX, dim		; EDX = n *dim
		CMP		ECX, EDX			; x<n?
		MOV		EDX, [EBP + n]		; EDX = n
		JL		forx
		MOV		EDX, [EBP + C]		; EDX = C
		ADD		EDX, EAX			; EDX = C + EAX
		MOVAPS	[EDX + EDI], XMM0	; C[i...i+p-1][j]=XMM0
		ADD		EBX, dim			; j++, j=j+dim
		MOV		EDX, [EBP + nn2]		; EDX = nn
		IMUL	EDX, EDX, dim		; EDX = nn * dim
		CMP		EBX, EDX			; j<nn?
		MOV		EDX, [EBP + n]		; EDX = n
		JL		forj
		ADD		EAX, dim*p			; i=i+p, i=i+dim*p
		IMUL	EDX, EDX, dim		; EDX = n*dim
		CMP		EAX, EDX			; i<n
		MOV		EDX, [EBP + n]		; EDX = n
		JL		fori
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
