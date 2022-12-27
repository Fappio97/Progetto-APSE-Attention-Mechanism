#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int arow,acol,brow,bcol;
void generateMatrix(){
	FILE *fptr;
	//Creating the Matrix A
	printf("Enter the row size for martix A(<1001):");
	scanf("%d", &arow);
	printf("Enter the column size for martix A(<1001):");
	scanf("%d", &acol);

	//Creating the Matrix B
	printf("Enter the row size for martix B(<1001):");
	scanf("%d", &brow);
	printf("Enter the column size for martix B(<1001):");
	scanf("%d", &bcol);
}

void matrixMultiplication(){
	FILE *fptr;
	float matrixA[arow][acol], matrixB[brow][bcol], num;
	//Accessing file a.txt and storing value in matrixA
	fptr = fopen("matrixA.txt", "r");
	for(int i=0; i<arow; i++){
		for(int j=0; j<acol; j++){
			fscanf(fptr, "%f", &num);
			matrixA[i][j] = num;
		}
	}
	fclose(fptr);

	//Accessing file b.txt and storing the value in matrixB
	fptr = fopen("matrixB.txt", "r");
	for(int i=0 ; i<brow; i++){
		for(int j=0; j<bcol; j++){
			fscanf(fptr, "%f", &num);
			matrixB[i][j] = num;
		}
	}
	fclose(fptr);

	//Matrix Multiplication
	 if(acol != brow){
	 	printf("Matix Multiplication is not possible due to dimension conflicts\n");
	 }else{
	 	fptr = fopen("matrixC.txt", "w");
	 	for(int i=0; i<arow; i++){
	 		for(int j=0; j<bcol; j++){
	 			float sum = 0;
	 			for(int k=0; k<acol; k++){
	 				sum = sum + matrixA[i][k] * matrixB[k][j];
	 			}
	 			fprintf(fptr, "%f ", sum);
	 		}
	 		fprintf(fptr, "\n");
	 	}
	 	fclose(fptr);
	 	printf("Please check the file matrixC.txt for result\n");
	 }	
}

void matrixMultiplicationBias(){
	FILE *fptr;
	float matrixA[arow][acol], matrixB[brow][bcol], num, bias[brow];
	//Accessing file a.txt and storing value in matrixA
	fptr = fopen("matrixA.txt", "r");
	for(int i=0; i<arow; i++){
		for(int j=0; j<acol; j++){
			fscanf(fptr, "%f", &num);
			matrixA[i][j] = num;
		}
	}
	fclose(fptr);

	//Accessing file b.txt and storing the value in matrixB
	fptr = fopen("matrixB.txt", "r");
	for(int i=0 ; i<brow; i++){
		for(int j=0; j<bcol; j++){
			fscanf(fptr, "%f", &num);
			matrixB[i][j] = num;
		}
	}
	fclose(fptr);

    fptr = fopen("bias.txt", "r");
	for(int i=0; i<bcol; i++){
		fscanf(fptr, "%f", &num);
		bias[i] = num;
	}
	fclose(fptr);

	//Matrix Multiplication
	 if(acol != brow){
	 	printf("Matix Multiplication is not possible due to dimension conflicts\n");
	 }else{
	 	fptr = fopen("matrixBias.txt", "w");
	 	for(int i=0; i<arow; i++){
	 		for(int j=0; j<bcol; j++){
	 			float sum = 0;
	 			for(int k=0; k<acol; k++){
	 				sum = sum + matrixA[i][k] * matrixB[k][j];
	 			}
                sum += bias[j];
	 			fprintf(fptr, "%f ", sum);
	 		}
	 		fprintf(fptr, "\n");
	 	}
	 	fclose(fptr);
	 	printf("Please check the file matrixBias.txt for result\n");
	 }	
}

float funzione(float value) {

    int x = -1;
    if(value >= 0)
        x = 1;

    float result = (x *((-1/(value + 2)) + 0.5 )) + 0.5;

    return result;
}

void matrixMultiplicationInversaDivisioneFunzione(float radice){
	FILE *fptr;
	float matrixA[arow][acol], matrixB[brow][bcol], num;
	//Accessing file a.txt and storing value in matrixA
	fptr = fopen("matrixA.txt", "r");
	for(int i=0; i<arow; i++){
		for(int j=0; j<acol; j++){
			fscanf(fptr, "%f", &num);
			matrixA[i][j] = num;
		}
	}
	fclose(fptr);

	//Accessing file b.txt and storing the value in matrixB
	fptr = fopen("matrixB.txt", "r");
	for(int i=0 ; i<brow; i++){
		for(int j=0; j<bcol; j++){
			fscanf(fptr, "%f", &num);
			matrixB[i][j] = num;
		}
	}
	fclose(fptr);

	//Matrix Multiplication
	 if(acol != bcol){
	 	printf("Matix Multiplication is not possible due to dimension conflicts\n");
	 }else{
	 	fptr = fopen("matrixC.txt", "w");
	 	for(int i=0; i<arow; i++){
	 		for(int j=0; j<bcol; j++){
	 			float sum = 0;
	 			for(int k=0; k<acol; k++){
	 				sum = sum + matrixA[i][k] * matrixB[j][k];
	 			}
                sum /= radice;
                sum = funzione(sum);
	 			fprintf(fptr, "%f ", sum);
	 		}
	 		fprintf(fptr, "\n");
	 	}
	 	fclose(fptr);
	 	printf("Please check the file matrixC.txt for result\n");
	 }	
}

int main(){
	generateMatrix();
	
	// matrixMultiplicationBias();
	// matrixMultiplicationInversaDivisioneFunzione(sqrt(48));
    matrixMultiplication();
	return 0;
}