#include <stdlib.h> 
#include <stdio.h>
#include <time.h>

static int rand_int(int n) {
	int limit = RAND_MAX - RAND_MAX % n;
	int rnd;
	srand(time(NULL));
	do{
		rnd = rand();
	}while (rnd >= limit);
	
	return rnd % n;
}

void shuffle(int *array, int n) {
	int i, j, tmp;

	for (i = n - 1; i > 0; i--) {
		j = rand_int(i + 1);
		tmp = array[j];
		array[j] = array[i];
		array[i] = tmp;
	}
}

int convert_to_bin(int n){
	
	int rem, i = 1, binary = 0;
    while(n != 0){
		rem = n%2;
        n /= 2;
        binary += rem*i;
        i *= 10;
    }
    return binary;
}

int main(){
	FILE *f = fopen("keyfile.txt", "w");
	int i = 0;
	int array[255];
	int bin_array[255];
	
	if (f == NULL){
		printf("Error opening file!\n");
		exit(1);
	}

	
	for(i = 0; i < 256; i++){
		array[i] = i;
		printf("Ordered - %d\n", array[i]);
	}
	shuffle(array, 256);
	
	for(i = 0; i < 256; i++){
		printf("Shuffle - %d\n", array[i]);
		bin_array[i] = convert_to_bin(array[i]);
		printf("Binary - %d\n", bin_array[i]);
		fprintf(f, "%08d\n",  bin_array[i]);
	}
	
	fclose(f);
	exit(0);
}
