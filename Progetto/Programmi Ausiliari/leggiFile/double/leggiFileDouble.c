#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <xmmintrin.h>
#include <dirent.h> 

#define	type		double
#define	MATRIX		type*
#define	VECTOR		type*

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

void save_data(char* filename, void* X, int n, int k) {
	FILE* fp;
	int i;
	fp = fopen(filename, "wb");
	if(X != NULL){
		fwrite(&k, 4, 1, fp);
		fwrite(&n, 4, 1, fp);
        for (int i = 0; i < n * k; i++) {
            if(i != 0 || (i) % k != 0)
                fprintf(fp, " ");
            if(i != 0 && i % k == 0)
                fprintf(fp, "\n");
            fprintf(fp, "%lf", ((double*)X)[i]);
            //X += sizeof(type)*k;
		}
	}
	else{
		int x = 0;
		fwrite(&x, 4, 1, fp);
		fwrite(&x, 4, 1, fp);
	}
	fclose(fp);
}

int main() {
    int* k;
    int a = 32, c = 2048;
    k = &a;
    int* n = &c;
    MATRIX b = load_data("out32_2048_8_64_48.ds2", n, k);

    save_data("out32_2048_8_64_48.txt", b, *n, *k);

    dealloc_matrix(b);

    MATRIX d = load_data("test_2048_48_64.os", n, k);

    save_data("test_2048_48_64_os.txt", d, *n, *k);

    dealloc_matrix(d);

    a = 64;
    k = &a;

    b = load_data("test_2048_48_64.ds", n, k);

    save_data("test_2048_48_64_ds.txt", b, *n, *k);

}
