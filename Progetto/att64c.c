/**************************************************************************************
* 
* CdL Magistrale in Ingegneria Informatica
* Corso di Architetture e Programmazione dei Sistemi di Elaborazione - a.a. 2020/21
* 
* Progetto dell'algoritmo Attention Mechanism 221 231 a
* in linguaggio assembly x86-64 + SSE
* 
* Fabrizio Angiulli, aprile 2019
* 
**************************************************************************************/

/*
* 
* Software necessario per l'esecuzione:
* 
*    NASM (www.nasm.us)
*    GCC (gcc.gnu.org)
* 
* entrambi sono disponibili come pacchetti software 
* installabili mediante il packaging tool del sistema 
* operativo; per esempio, su Ubuntu, mediante i comandi:
* 
*    sudo apt-get install nasm
*    sudo apt-get install gcc
* 
* potrebbe essere necessario installare le seguenti librerie:
* 
*    sudo apt-get install lib64gcc-4.8-dev (o altra versione)
*    sudo apt-get install libc6-dev-i386
* 
* Per generare il file eseguibile:
* 
* nasm -f elf64 att64.nasm && gcc -m64 -msse -O0 -no-pie sseutils64.o att64.o att64c.c -o att64c -lm && ./att64c -ds test_2048_48_64.ds -wq test_48_32_64.wq -wk test_48_32_64.wk -wv test_48_32_64.wv -bq test_32_64.bq -bk test_32_64.bk -bv test_32_64.bv -si 8 -n 64
* 
* oppure
* 
* ./runfss64
* 
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <xmmintrin.h>

#define	type		double
#define	MATRIX		type*
#define	VECTOR		type*

typedef struct {
	MATRIX ds; 	// dataset
	MATRIX wq; 	// pesi WQ
	MATRIX wk; 	// pesi WK
	MATRIX wv; 	// pesi WV
	MATRIX out;	// matrice contenente risultato (N x nn)
	VECTOR bq; 	// pesi bq
	VECTOR bk; 	// pesi bk
	VECTOR bv; 	// pesi bv
	int N;		// numero di righe del dataset
	int s; 		// prima dimensione del tensore S
	int n; 		// seconda dimensione del tensore S
	int d; 		// terza dimensione del tensore S
	int ns; 	// numero di tensori nel dataset
	int nn;		// numero di neuroni
	int display;
	int silent;
} params;

/*
* 
*	Le funzioni sono state scritte assumento che le matrici siano memorizzate 
* 	mediante un array (float*), in modo da occupare un unico blocco
* 	di memoria, ma a scelta del candidato possono essere 
* 	memorizzate mediante array di array (float**).
* 
* 	In entrambi i casi il candidato dovr� inoltre scegliere se memorizzare le
* 	matrici per righe (row-major order) o per colonne (column major-order).
*
* 	L'assunzione corrente � che le matrici siano in row-major order.
* 
*/

void* get_block(int size, int elements) { 
	return _mm_malloc(elements*size,32); 
}

void free_block(void* p) { 
	_mm_free(p);
}

MATRIX alloc_matrix(int rows, int cols) {
	return (MATRIX) get_block(sizeof(type),rows*cols);
}

void dealloc_matrix(MATRIX mat) {
	free_block(mat);
}

/*
* 
* 	load_data
* 	=========
* 
*	Legge da file una matrice di N righe
* 	e M colonne e la memorizza in un array lineare in row-major order
* 
* 	Codifica del file:
* 	primi 4 byte: numero di righe (N) --> numero intero
* 	successivi 4 byte: numero di colonne (M) --> numero intero
* 	successivi N*M*4 byte: matrix data in row-major order --> numeri floating-point a precisione singola
* 
*****************************************************************************
*	Se lo si ritiene opportuno, � possibile cambiare la codifica in memoria
* 	della matrice. 
*****************************************************************************
* 
*/
MATRIX load_data(char* filename, int *n, int *k) {
	FILE* fp;
	int rows, cols, status, i;
	
	fp = fopen(filename, "rb");
	
	if (fp == NULL){
		printf("'%s': bad data file name!\n", filename);
		exit(0);
	}
	
	status = fread(&cols, sizeof(int), 1, fp);
	status = fread(&rows, sizeof(int), 1, fp);
	
	MATRIX data = alloc_matrix(rows,cols);
	status = fread(data, sizeof(type), rows*cols, fp);
	fclose(fp);
	
	*n = rows;
	*k = cols;
	
	return data;
}

/*
* 	save_data
* 	=========
* 
*	Salva su file un array lineare in row-major order
*	come matrice di N righe e M colonne
* 
* 	Codifica del file:
* 	primi 4 byte: numero di righe (N) --> numero intero a 32 bit
* 	successivi 4 byte: numero di colonne (M) --> numero intero a 32 bit
* 	successivi N*M*4 byte: matrix data in row-major order --> numeri interi o floating-point a precisione singola
*/
void save_data(char* filename, void* X, int n, int k) {
	FILE* fp;
	int i;
	fp = fopen(filename, "wb");
	if(X != NULL){
		fwrite(&k, 4, 1, fp);
		fwrite(&n, 4, 1, fp);
		for (i = 0; i < n; i++) {
			fwrite(X, sizeof(type), k, fp);
			//printf("%i %i\n", ((int*)X)[0], ((int*)X)[1]);
			X += sizeof(type)*k;
		}
	}
	else{
		int x = 0;
		fwrite(&x, 4, 1, fp);
		fwrite(&x, 4, 1, fp);
	}
	fclose(fp);
}

// PROCEDURE ASSEMBLY

extern void prova(params* input);
extern void prodottoAVX(MATRIX A, MATRIX b, int nn, int nn_x_i, int nn_x_j, type* a);

// ######################################### COMPUTAZIONE NOSTRA ################################
// funzione che stampa una matrice
void stampaMatrice(MATRIX A, int a, int n, int d)
{
	for (int x = a; x < n; ++x)
	{
		printf("Riga %d\n", x);
		for (int y = 0; y < d; ++y)
			printf("%f ", A[d * x + y]);
		printf("\n");
	}
	printf("\n");
}

// funzione che fa un prodotto tra matrici
// MATRIX prodottoMatrici(MATRIX A, MATRIX B, int n, int nn)
// {
// 	// la matrice risultate tra il prodotto di matrice avrà dimensione n x nn
// 	MATRIX output = alloc_matrix(n, nn);

// 	for (int i = 0; i < n; ++i)
// 	{
// 		for (int j = 0; j < nn; ++j)
// 		{
// 			output[nn * i + j] = 0;
// 			for (int x = 0; x < n; ++x)
// 			{
// 				output[nn * i + j] += A[n * i + x] * B[nn * x + j];
// 			}
// 		}
// 	}

// 	return output;
// }

// // funzione che fa un prodotto fra matrici e somma il bias
// MATRIX prodottoMatriciBias(MATRIX DS, MATRIX pesi, VECTOR bias, int avanza_tensore_per_indice, int avanza_matrice_per_indice, int n, int d, int nn)
// {
// 	// la matrice risultate tra il prodotto di matrice avrà dimensione n x nn
// 	MATRIX output = alloc_matrix(n, nn);

// 	for (int i = 0; i < n; ++i)
// 	{
// 		for (int j = 0; j < nn; ++j)
// 		{
// 			output[nn * i + j] = 0;
// 			for (int x = 0; x < d; ++x)
// 				output[nn * i + j] += DS[d * i + x + avanza_tensore_per_indice + avanza_matrice_per_indice] * pesi[nn * x + j];
// 			output[nn * i + j] += bias[j];
// 		}
// 	}

// 	return output;
// }

void prodottoAllMatriciBias(MATRIX Q, MATRIX K, MATRIX V, MATRIX ds, MATRIX wq, MATRIX wk, MATRIX wv, VECTOR bq, VECTOR bk, VECTOR bv, int avanza, int n, int nn, int d)
{
	// la matrice risultate tra il prodotto di matrice avrà dimensione n x nn
	type q;
	type k;
	type v;

	for (int i = 0; i < n; ++i)
	{
		int d_x_i_avanza = d * i + avanza;
		for (int j = 0; j < nn; ++j)
		{
			q = 0;
			k = 0;
			v = 0;
			int indx_matr = nn * i + j;
			for (int x = 0; x < d; ++x)
			{
				int indx_pesi = nn * x + j;
				q += ds[d_x_i_avanza + x] * wq[indx_pesi];
				k += ds[d_x_i_avanza + x] * wk[indx_pesi];
				v += ds[d_x_i_avanza + x] * wv[indx_pesi];
			}
			Q[indx_matr] = q + bq[j];
			K[indx_matr] = k + bk[j];
			V[indx_matr] = v + bv[j];
		}
	}
}

// la funzione della traccia (punto 3)
type funzione(type value)
{

	int x = -1;
	if (value >= 0)
		x = 1;

	return (x * ((-1 / (value + 2)) + 0.5)) + 0.5;
}

// funzione che fa il prodotto fra matrice e matrice trasposta, poi divisione e applicazione della funzione
void prodottoMatriciInversa(MATRIX intermedio, MATRIX A, MATRIX B, type radice, int n, int nn)
{
	// la matrice risultate tra il prodotto di matrice avrà dimensione n x n
	type a;

	for (int i = 0; i < n; ++i)
	{
		int nn_x_i = nn * i;
		int n_x_i = n * i;
		for (int j = 0; j < n; ++j)
		{
			a = 0;
			int nn_x_j = nn * j;
			// for (int x = 0; x < nn; ++x)
			// 	a += A[nn_x_i + x] * B[nn * j + x];
			prodottoAVX(A, B, nn, nn_x_i, nn_x_j, &a);
			a /= radice;
			intermedio[n_x_i + j] = funzione(a);
		}
	}
}

// funzione che fa un prodotto tra matrici e le salva su input->output
void prodottoMatriciESalva(MATRIX output, MATRIX A, MATRIX B, int avanza, int n, int nn)
{
	// la matrice risultate tra il prodotto di matrice avrà dimensione n x nn
	type y;
	for (int i = 0; i < n; ++i)
	{
		int n_x_i = n * i;
		int indx_out = nn * i + avanza;
		for (int j = 0; j < nn; ++j)
		{
			y = 0;
			for (int x = 0; x < n; ++x)
				y += A[n_x_i + x] * B[nn * x + j];
			output[indx_out + j] = y;
		}
	}
}

void deallocaAllMatrici(MATRIX Q, MATRIX K, MATRIX V, MATRIX intermedio)
{
	dealloc_matrix(Q);
	dealloc_matrix(K);
	dealloc_matrix(V);
	dealloc_matrix(intermedio);
}

// ######################################### COMPUTAZIONE NOSTRA ################################

void att(params* input){
	// -------------------------------------------------
	// Codificare qui l'algoritmo Attention mechanism
	// -------------------------------------------------

	type radice = sqrt(input->d);

	int avanza_tensore = input->N * input->d / input->ns;
	int avanza_matrice = input->n * input->d;
	int avanza_tensore_out = input->N * input->nn / input->ns;
	int avanza_matrice_out = input->n * input->nn;

	// scorrimento dei tensori del DS
	for (int i = 0; i < input->ns; ++i)
	{
		for (int j = 0; j < input->s; ++j)
		{
			MATRIX Q = alloc_matrix(input->n, input->nn);
			MATRIX K = alloc_matrix(input->n, input->nn);
			MATRIX V = alloc_matrix(input->n, input->nn);
			MATRIX intermedio = alloc_matrix(input->n, input->n);

			// Q = prodottoMatriciBias(input->ds, input->wq, input->bq, avanza_tensore_per_indice, avanza_matrice_per_indice, input->n, input->d, input->nn);
			// K = prodottoMatriciBias(input->ds, input->wk, input->bk, avanza_tensore_per_indice, avanza_matrice_per_indice, input->n, input->d, input->nn);
			// V = prodottoMatriciBias(input->ds, input->wv, input->bv, avanza_tensore_per_indice, avanza_matrice_per_indice, input->n, input->d, input->nn);

			prodottoAllMatriciBias(Q, K, V, input->ds, input->wq, input->wk, input->wv, input->bq, input->bk, input->bv, (i * avanza_tensore) + (j * avanza_matrice), input->n, input->nn, input->d);

			// if(j == 1) {
			// 	// printf("MATRICE Q\n");
			// 	// stampaMatrice(Q, 0, input->n, input->nn);
			// 	// printf("\nMATRICE K\n");
			// 	// stampaMatrice(K, 0, input->n, input->nn);
			// 	printf("\nMATRICE V\n");
			// 	stampaMatrice(V, 0, input->n, input->nn);
			// 	return;
			// }

			// Q diventa il nostro S' nello pseudocodice della traccia
			// tanto non mi serve più il contenuto dentro Q
			prodottoMatriciInversa(intermedio, Q, K, radice, input->n, input->nn);

			// if(j == 0) {
			// 	printf("\nMATRICE intermedia\n");
			// 	stampaMatrice(intermedio, 0, input->n, input->n);
			// 	return;
			// }

			// dealloc_matrix(Q);

			// K diventa il nostro S^ nello pseudocodice della traccia
			// tanto non mi serve più il contenuto dentro Q

			prodottoMatriciESalva(input->out, intermedio, V, (i * avanza_tensore_out) + (j * avanza_matrice_out), input->n, input->nn);

			// if(i == 0 && j == 0) {
			// 	stampaMatrice(input->out, 0, 64, input->nn);
			// 	return;
			// }
			deallocaAllMatrici(Q, K, V, intermedio);
		}
	}
}

int main(int argc, char** argv) {

	char fname[256];
	char* dsfilename = NULL;
	char* wqfilename = NULL;
	char* wkfilename = NULL;
	char* wvfilename = NULL;
	char* bqfilename = NULL;
	char* bkfilename = NULL;
	char* bvfilename = NULL;
	clock_t t;
	float time;
	
	//
	// Imposta i valori di default dei parametri
	//

	params* input = malloc(sizeof(params));

	input->ds = NULL;
	input->wq = NULL;
	input->wk = NULL;
	input->wv = NULL;
	input->bq = NULL;
	input->bk = NULL;
	input->bv = NULL;
	input->s = -1;
	input->n = -1;
	input->d = -1;
	input->ns = -1;
	
	input->silent = 0;
	input->display = 0;

	//
	// Visualizza la sintassi del passaggio dei parametri da riga comandi
	//

	if(argc <= 1){
		printf("%s -ds <DS> -wq <WQ> -wk <WK> -wv <WV> -bq <bQ> -bk <bK> -bv <bV> -si <si> -n <n> [-s] [-d]\n", argv[0]);
		printf("\nParameters:\n");
		printf("\tDS: il nome del file ds2 contenente il dataset\n");
		printf("\tWQ: il nome del file ds2 contenente i pesi WQ\n");
		printf("\tWK: il nome del file ds2 contenente i pesi WK\n");
		printf("\tWV: il nome del file ds2 contenente i pesi WV\n");
		printf("\tbQ: il nome del file ds2 contenente i pesi bQ\n");
		printf("\tbK: il nome del file ds2 contenente i pesi bK\n");
		printf("\tbV: il nome del file ds2 contenente i pesi bV\n");
		printf("\tN: numero di righe del dataset\n");
		printf("\tsi: prima dimensione del tensore\n");
		printf("\tn: seconda dimensione del tensore\n");
		printf("\tnn: numero di neuroni\n");
		printf("\nOptions:\n");
		printf("\t-s: modo silenzioso, nessuna stampa, default 0 - false\n");
		printf("\t-d: stampa a video i risultati, default 0 - false\n");
		exit(0);
	}

	//
	// Legge i valori dei parametri da riga comandi
	//

	int par = 1;
	while (par < argc) {
		if (strcmp(argv[par],"-s") == 0) {
			input->silent = 1;
			par++;
		} else if (strcmp(argv[par],"-d") == 0) {
			input->display = 1;
			par++;
		} else if (strcmp(argv[par],"-ds") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing dataset file name!\n");
				exit(1);
			}
			dsfilename = argv[par];
			par++;
		} else if (strcmp(argv[par],"-wq") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing wq file name!\n");
				exit(1);
			}
			wqfilename = argv[par];
			par++;
		} else if (strcmp(argv[par],"-wk") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing wk file name!\n");
				exit(1);
			}
			wkfilename = argv[par];
			par++;
		} else if (strcmp(argv[par],"-wv") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing wv file name!\n");
				exit(1);
			}
			wvfilename = argv[par];
			par++;
		} else if (strcmp(argv[par],"-bq") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing bq file name!\n");
				exit(1);
			}
			bqfilename = argv[par];
			par++;
		} else if (strcmp(argv[par],"-bk") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing bk file name!\n");
				exit(1);
			}
			bkfilename = argv[par];
			par++;
		} else if (strcmp(argv[par],"-bv") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing bv file name!\n");
				exit(1);
			}
			bvfilename = argv[par];
			par++;
		} else if (strcmp(argv[par],"-si") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing si value!\n");
				exit(1);
			}
			input->s = atoi(argv[par]);
			par++;
		} else if (strcmp(argv[par],"-n") == 0) {
			par++;
			if (par >= argc) {
				printf("Missing n value!\n");
				exit(1);
			}
			input->n = atoi(argv[par]);
			par++;
		} else{
			printf("WARNING: unrecognized parameter '%s'!\n",argv[par]);
			par++;
		}
	}

	//
	// Legge i dati e verifica la correttezza dei parametri
	//

	if(dsfilename == NULL || strlen(dsfilename) == 0){
		printf("Missing ds file name!\n");
		exit(1);
	}

	if(wqfilename == NULL || strlen(wqfilename) == 0){
		printf("Missing wq file name!\n");
		exit(1);
	}

	if(wkfilename == NULL || strlen(wkfilename) == 0){
		printf("Missing wk file name!\n");
		exit(1);
	}

	if(wvfilename == NULL || strlen(wvfilename) == 0){
		printf("Missing wv file name!\n");
		exit(1);
	}

	if(bqfilename == NULL || strlen(bqfilename) == 0){
		printf("Missing bq file name!\n");
		exit(1);
	}

	if(bkfilename == NULL || strlen(bkfilename) == 0){
		printf("Missing bk file name!\n");
		exit(1);
	}

	if(bvfilename == NULL || strlen(bvfilename) == 0){
		printf("Missing bv file name!\n");
		exit(1);
	}

	input->ds = load_data(dsfilename, &input->N, &input->d);

	if(input->s <= 0){
		printf("Invalid value of si parameter!\n");
		exit(1);
	}

	if(input->n <= 0 || input->N % input->n != 0){
		printf("Invalid value of n parameter!\n");
		exit(1);
	}

	input->ns = (int) ceil((double) input->N / (input->s * input->n));

	// Caricamento matrici livelli
	int n, nn;
	input->wq = load_data(wqfilename, &n, &input->nn);

	if(input->d != n){
		printf("Invalid wq size!\n");
		exit(1);
	}

	input->wk = load_data(wkfilename, &n, &nn);

	if(input->d != n || input->nn != nn){
		printf("Invalid wk size!\n");
		exit(1);
	}

	input->wv = load_data(wvfilename, &n, &nn);

	if(input->d != n || input->nn != nn){
		printf("Invalid wv size!\n");
		exit(1);
	}

	// Caricamento bias
	input->bq = load_data(bqfilename, &n, &nn);

	if(n != 1 || input->nn != nn){
		printf("Invalid bq size!\n");
		exit(1);
	}

	input->bk = load_data(bkfilename, &n, &nn);

	if(n != 1 || input->nn != nn){
		printf("Invalid bk size!\n");
		exit(1);
	}

	input->bv = load_data(bvfilename, &n, &nn);

	if(n != 1 || input->nn != nn){
		printf("Invalid bv size!\n");
		exit(1);
	}

	input->out = alloc_matrix(input->N, input->nn);

	//
	// Visualizza il valore dei parametri
	//

	if(!input->silent){
		printf("Dataset file name: '%s'\n", dsfilename);
		printf("WQ file name: '%s'\n", wqfilename);
		printf("WK file name: '%s'\n", wkfilename);
		printf("WV file name: '%s'\n", wvfilename);
		printf("bQ file name: '%s'\n", bqfilename);
		printf("bK file name: '%s'\n", bkfilename);
		printf("bV file name: '%s'\n", bvfilename);
		printf("Dataset row number: %d\n", input->N);
		printf("Tensor first dimention: %d\n", input->s);
		printf("Tensor second dimention: %d\n", input->n);
		printf("Tensor third dimention: %d\n", input->d);
		printf("Dataset block number: %d\n", input->ns);
		printf("Layer neuron number: %d\n", input->nn);
	}

	// COMMENTARE QUESTA RIGA!
	// prova(input);
	//

	//
	// Attention Mechanism
	//

	t = clock();
	att(input);
	t = clock() - t;
	time = ((float)t)/CLOCKS_PER_SEC;

	if(!input->silent)
		printf("ATT time = %.3f secs\n", time);
	else
		printf("%.3f\n", time);

	//
	// Salva il risultato
	//
	sprintf(fname, "out32_%d_%d_%d_%d.ds2", input->N, input->s, input->n, input->d);
	save_data(fname, input->out, input->N, input->nn);
	if(input->display){
		if(input->out == NULL)
			printf("out: NULL\n");
		else{
			int i,j;
			printf("out: [");
			for(i=0; i<input->N; i++){
				for(j=0; j<input->nn-1; j++)
					printf("%f,", input->out[input->d*i+j]);
				printf("%f\n", input->out[input->d*i+j]);
			}
			printf("]\n");
		}
	}

	if(!input->silent)
		printf("\nDone.\n");

	return 0;
}
