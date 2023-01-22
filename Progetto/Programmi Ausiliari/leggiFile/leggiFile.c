#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <xmmintrin.h>
#include <dirent.h> 

void* get_block(int size, int elements) { 
	return _mm_malloc(elements*size,16); 
}


float* alloc_matrix(int rows, int cols) {
	return (float*) get_block(sizeof(float),rows*cols);
}

void convertFormat(char* path) {
    FILE* fp;
    int rows, cols, status;

    char* p = malloc(strlen(path) + 6 + 1);
    strcpy(p, "input/");
    strcat(p, path);

    fp = fopen(p, "rb");

    printf("Apertura file %s", p);
        
    if (fp == NULL){
        printf("'%s': bad data file name!\n", p);
        exit(0);
    }
        
    status = fread(&cols, sizeof(int), 1, fp);
    status = fread(&rows, sizeof(int), 1, fp);

    float* data = alloc_matrix(rows,cols);
	status = fread(data, sizeof(float), rows*cols, fp);
	fclose(fp);

    // int i, j;
    // for(i = 0; i < rows * cols; ++i){
	// 	printf("%f  ", data[i]);
    //     if(i != 0 && i % rows == 0)
	// 	    printf("\n");
    // }

    free(p);
    char* dot = strtok(path, ".");
    p = malloc(strlen(dot) + 2 + 4 + 7 + 1);
    strcpy(p, "output/");
    strcat(p, dot);
    if(dot != NULL)
        dot = strtok(NULL, ".");
    strcat(p, dot);
    strcat(p, ".txt");
    printf("\n%s\n", p);

	fp = fopen(p, "w");
	if(data != NULL){
        fprintf(fp, "Righe = %d\n", rows); 
        fprintf(fp, "Colonne = %d\n", cols); 
		for (int i = 0; i < rows * cols; i++) {
            if(i != 0 || (i) % cols != 0)
                fprintf(fp, " ");
            if(i != 0 && i % cols == 0)
                fprintf(fp, "\n");
			fprintf(fp, "%f", data[i]);
		}
	}
	fclose(fp);

}

int main() {
    
    DIR *d;
    struct dirent *dir;
    d = opendir("input/");
    if (d) {
        while ((dir = readdir(d)) != NULL)
            if(strncmp("test", dir->d_name, 4) == 0 || strncmp("out", dir->d_name, 3) == 0) {
                printf("%s\n", dir->d_name);
                convertFormat(dir->d_name);
            }
        closedir(d);
    }
    return 0;
}
