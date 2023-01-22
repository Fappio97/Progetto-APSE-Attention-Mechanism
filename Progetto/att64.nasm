; ---------------------------------------------------------
; Regression con istruzioni AVX a 64 bit
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
;     nasm -f elf64 regression64.nasm
;
;prodotto(MATRIX a, MATRIX b, int nn, int nn_x_i, int nn_x_j, double* a)
%include 'sseutils64.nasm'

section .data
	;starta	equ	8	RDI
	;startb	equ	12	RSI
	;nn		equ	16	RDX
	;nni		equ	20	RCX
	;nnj		equ	24	R8
	;ris		equ	28	R9
section .bss
;
section .text
	global prodottoAVX
prodottoAVX:
	push		rbp
	mov			rbp,rsp
	pushaq
	;corpo della funzione
	SHL			RCX, 3							;nni*8 (dimensione operandi)
	SHL			R8, 	 3							;nnj*8 (dimensione operandi)
	ADD			RDI, RCX							;A[nn_x_i]
	ADD			RSI, R8							;B[nn_x_j]
	
	VXORPD		YMM0, YMM0						;a=0;
	XOR			RAX, RAX						;contatore
c:	VMOVUPD		YMM1, [RDI+RAX*8]
	VMULPD		YMM1, [RSI+RAX*8]
	VHADDPD		YMM1, YMM1
	VPERM2F128	YMM2, YMM1, YMM1, 10000011b
	VADDSD		XMM1, XMM2
	VADDSD		XMM0, XMM1
	ADD			RAX, 4							;4 elementi alla volta
	CMP		RAX, RDX
	JL			c
	VMOVSD		[R9], XMM0
	;
	popaq
	mov 	rsp, rbp
	pop		rbp
	ret