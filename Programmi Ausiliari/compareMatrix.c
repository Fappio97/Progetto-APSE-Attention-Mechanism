#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

int rows = 2048, cols = 32;

void compareMatrix(){
	FILE *fptr;
	int matrixA[rows][cols], matrixB[rows][cols];
    float num;
	//Accessing file a.txt and storing value in matrixA
	fptr = fopen("matrixA.txt", "r");
	for(int i=0; i<rows; i++){
		for(int j=0; j<cols; j++){
			fscanf(fptr, "%f", &num);
			matrixA[i][j] = (int)num;
		}
	}
	fclose(fptr);

	//Accessing file b.txt and storing the value in matrixB
	fptr = fopen("matrixB.txt", "r");
	for(int i=0 ; i<rows; i++){
		for(int j=0; j<cols; j++){
			fscanf(fptr, "%f", &num);
			matrixB[i][j] = (int)num;
		}
	}
	fclose(fptr);


	//Matrix Multiplication
	for(int x=0; x<rows; ++x){
	 	for(int y=0; y<cols; ++y){
	 		assert(matrixA[x][y] == matrixB[x][y] || printf("matrixA[%d][%d] = %d is not equal to matrixB[%d][%d] = %d\n", x, y, matrixA[x][y], x, y, matrixB[x][y]));
        }
	}	
}

int main(){
    compareMatrix();
	return 0;
}