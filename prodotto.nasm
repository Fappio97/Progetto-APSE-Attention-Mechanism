;// funzione che fa il prodotto fra matrice e matrice trasposta, poi divisione e applicazione della funzione
;void prodottoMatriciInversa(MATRIX intermedio, MATRIX A, MATRIX B, float radice, int n, int nn)
;{
;	// la matrice risultate tra il prodotto di matrice avrà dimensione n x n
;	float a;

;for (int i = 0; i < n; ++i)
;{
;	int nn_x_i = nn * i;
;	int n_x_i = n * i;
;	for (int j = 0; j < n; ++j)
;	{
;	  a = 0;
;     	  int nn_x_j = nn * j;
;	  for (int x = 0; x < nn; ++x)
;		a += A[nn_x_i + x] * B[nn_x_j + x];
;	  a /= radice;
;	  intermedio[n_x_i + j] = funzione(a);
;		}
;	}
;}


;prodotto(MATRIX a, MATRIX b, int nn, int nn_x_i, int nn_x_j, float* value)

%include 'sseutils32.nasm'

section .data
	starta	equ	8
	startb	equ	12
	nn		equ	16
	nni		equ	20
	nnj		equ	24
	ris		equ	28
section .bss
;
	alignb	16
	C	resd	1
section .text
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
	;a questo punto volendo � possibile riutilizzare i registri ECX e EDX
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

	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	