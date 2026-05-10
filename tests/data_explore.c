#include "stdio.h"
#include "stdlib.h"

int main(int argc, char *argv[]){
    char *file_name = argv[1];
    FILE *f = fopen(file_name,"r+");
    if(f==NULL){
        perror(file_name);
        exit(1);
    }
    for(int i=0; i<10; i++){
        float number1;
        float number2;
        fread(&number1,sizeof(float),1,f);
        fread(&number2,sizeof(float),1,f);
        printf("Numeros leidos son %f y %f\n",number1,number2);
    }
    fclose(f);
    return 0;
}