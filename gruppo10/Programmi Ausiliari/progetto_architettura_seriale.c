// Esecuzione
// gcc progetto_architettura_seriale.c -lm
// ./a.out

// -lm per math.h
// se no dovete fare -o eseguibile e fare ./eseguibile

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <assert.h>

// funzione che stampa una matrice
void stampaMatrice(float** A, int n, int d) {
    for(int x = 0; x < n; ++x) {
        for(int y = 0; y < d; ++y)
            printf("Matrice[%d][%d] = %f ", x, y, A[x][y]);
        printf("\n");
    }
    printf("\n");
} 

// funzione che fa un prodotto tra matrici
float** prodottoMatrici(float** A, float** B, int n, int nn) {
    // la matrice risultate tra il prodotto di matrice avrà dimensione n x nn
    float** output = (float**) malloc(n * sizeof(float*));
    for(int i = 0; i < n; ++i)
        output[i] = malloc(nn * sizeof(float));

    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < nn; ++j) {
            output[i][j] = 0;
            for(int x = 0; x < n; ++x)
                output[i][j] += A[i][x] * B[x][j]; 
        }
    }

    return output;
}

// funzione che fa un prodotto fra matrici e somma il bias
float** prodottoMatriciBias(float** A, float** pesi, float* bias, int n, int d, int nn) {
    // la matrice risultate tra il prodotto di matrice avrà dimensione n x n
    float** output = (float**) malloc(n * sizeof(float*));
    for(int i = 0; i < n; ++i)
        output[i] = malloc(nn * sizeof(float));

    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < nn; ++j) {
            output[i][j] = 0;
            for(int x = 0; x < d; ++x)
                output[i][j] += A[i][x] * pesi[x][j]; 
            output[i][j] += bias[j];
        }
    }

    return output;
}

// la funzione della traccia (punto 3)
float funzione(float value) {

    int x = -1;
    if(value >= 0)
        x = 1;

    float result = (x *((-1/(value + 2)) + 0.5 )) + 0.5;

    return result;
}

// funzione che fa il prodotto fra matrice e matrice trasposta, poi divisione e applicazione della funzione
float** prodottoMatriciInversa(float** A, float** B, float radice, int n, int d) {
    // la matrice risultate tra il prodotto di matrice avrà dimensione n x n
    float** output = (float**) malloc(n * sizeof(float*));
    for(int i = 0; i < n; ++i)
        output[i] = malloc(n * sizeof(float));

    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < n; ++j) {
            output[i][j] = 0;
            for(int x = 0; x < d; ++x)
                output[i][j] += A[i][x] * B[j][x]; 
            output[i][j] /= radice;
            output[i][j] = funzione(output[i][j]);
        }
    }

    return output;
}

/*
    Ogni programma C ha una funzione primaria che 
    deve essere denominata main. La main funzione 
    funge da punto di partenza per l'esecuzione del programma. 
    Generalmente, controlla l'esecuzione del programma indirizzando 
    le chiamate alle altre funzioni del programma.
*/
int main() {
    // inizio tempo
    clock_t start = clock();

    // --- INIZIO INPUT ---

    // numero di tensori del dataset DS (input)
    int N = 1;

    // numero di matrici del tensore S (input) 
    int s = 1;

    // dimensione delle matrici all'interno del tensore S (input)
    int n = 3;
    int d = 4;
    int nn = 2;

    // S1 è la matrice all'interno del tensore S (input)
 	float** S1; 
 	float** out; 
 	float** intermedio; 

    // vettore che contiene le s matrici all'interno del tensore S (input)
    float**** tensori = malloc(s * sizeof(float**));
    for(int i = 0; i < s; ++i) {
        tensori[i] = &S1;
        // printf("tensori[%d] = %p, S(%d) = %p\n", i, tensori[i], i, &S1);
    }
    
    // vettore che contiene gli N tensori del dataset DS (input)
    float****** dataset = malloc(N * sizeof(float***));
    for(int i = 0; i < N; ++i) {
        dataset[i] = &tensori;
        // printf("dataset[%d] = %p, tensori = %p\n", i, dataset[i], &tensori);
    }

    // matrici dei pesi dati (input)
 	float** Wq;
 	float** Wk;
 	float** Wv;

    // vettori bias (input)
    float* bq = malloc(nn * sizeof(float));
    float* bk = malloc(nn * sizeof(float));
    float* bv = malloc(nn * sizeof(float));

    // matrici durante le computazioni
    float** Q;
 	float** K;
 	float** V;

    // matrice per verificare che sia giusta
    //float** check;

    // allocazione della matrice
	S1 = (float**) malloc(n * sizeof(float*));
	out = (float**) malloc(n * sizeof(float*));
	intermedio = (float**) malloc(n * sizeof(float*));
	Wq = (float**) malloc(d * sizeof(float*));
	Wk = (float**) malloc(d * sizeof(float*));
	Wv = (float**) malloc(d * sizeof(float*));
	// check = (float**) malloc(n * sizeof(float*));
	
	for(int i = 0; i < n; ++i) {
		S1[i] = malloc(d * sizeof(float));
		out[i] = malloc(nn * sizeof(float));
		intermedio[i] = malloc(n * sizeof(float));
		// check[i] = malloc(n * sizeof(float));
        for(int j = 0; j < d; ++j)
            S1[i][j] = i + j;
	}

    for(int i = 0; i < d; ++i) {
        Wq[i] = malloc(nn * sizeof(float));
		Wk[i] = malloc(nn * sizeof(float));
		Wv[i] = malloc(nn * sizeof(float));
    }

    // formazione di input fissati per verificare che l'algoritmo funzioni
    for(int i = 0; i < nn; ++i) {
        bq[i] = 3;
        bk[i] = 2;
        bv[i] = 1;
    }

    // matrice di check solo per l'assert
    // puoi farti i calcoli e poi inserire la matrice
    // che ti aspetti qui, così puoi checkare quale posizione
    // fallisce non restituendo lo stesso value
    // check[0][0] = 32.952534;
    // check[0][1] = 32.952534;
    // check[0][2] = 32.952534;
    // check[1][0] = 32.969757;
    // check[1][1] = 32.969757;
    // check[1][2] = 32.969757;
    // check[2][0] = 32.977810;
    // check[2][1] = 32.977810;
    // check[2][2] = 32.977810;

    // formazione di input fissati per verificare che l'algoritmo funzioni
    for(int i = 0; i < d; ++i) {
        for(int j = 0; j < nn; ++j) {
            Wq[i][j] = 3;
            Wk[i][j] = 2;
            Wv[i][j] = 1;
        }
    }

    // la radice di d
    float radice = sqrt(d);
    // printf("Radice di %d = %f\n", d, radice);

    // --- FINE INPUT ---



    // --- INIZIO COMPUTAZIONE ---

    // scorrimento dei tensori del DS
    for(int i = 0; i < N; ++i) {
        // scorrimento delle matrici all'interno del tensore i-esimo
        for(int j = 0; j < s; ++j) {
            // printf("punto = %p, S1 = %p\n", dataset[i], &S1);
            float***** p = dataset[i];
            // printf("punto = %p, S1 = %p\n", p[i][j][0], &S1);
            Q = prodottoMatriciBias(p[i][j][0], Wq, bq, n, d, nn);
            K = prodottoMatriciBias(p[i][j][0], Wk, bk, n, d, nn);
            V = prodottoMatriciBias(p[i][j][0], Wv, bv, n, d, nn);

            // stampaMatrice(Q, n, nn);
            // stampaMatrice(K, n, nn);
            // stampaMatrice(V, n, nn);

            // Q diventa il nostro S' nello pseudocodice della traccia
            // tanto non mi serve più il contenuto dentro Q
            Q = prodottoMatriciInversa(Q, K, radice, n, nn);

            // stampaMatrice(Q, n, n);
            // stampaMatrice(V, n, nn);

            // K diventa il nostro S^ nello pseudocodice della traccia
            // tanto non mi serve più il contenuto dentro Q
            K = prodottoMatrici(Q, V, n, nn);

            // stampaMatrice(K, n, nn);
        } 
    }

    // --- FINE COMPUTAZIONE ---


    // fine tempo
    clock_t end = clock();

    // stampa tempo
    printf("Tempo computazione: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    // --- INIZIO FASE DI CHECK ---

    // print della matrice di output
    stampaMatrice(K, n, nn);

    // check con assert
    // for(int x = 0; x < n; ++x)
    //     for(int y = 0; y < nn; ++y)
    //         assert(K[x][y] == check[x][y] || printf("output[%d][%d] = %f is not equal to check[%d][%d] = %f\n", x, y, K[x][y], x, y, check[x][y]));

    // --- FINE FASE DI CHECK ---
    
    return 0;
}