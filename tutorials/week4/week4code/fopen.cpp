#include <iostream>
#include <stdio.h>

using namespace std;


int main(int argc, char **argv){
    char *output_file;
    char *input_file;

    input_file = argv[1];
    output_file = argv[2];

    FILE * fpInput = fopen(input_file, "r");
    FILE * fpOutput = fopen(output_file, "w");

    if(fpInput && fpOutput)
    {
        char c;

        while((c= fgetc(fpInput)) != EOF)
        {
            fwrite(&c, sizeof(char), sizeof(char), fpOutput);
        }
    }

    fclose(fpInput);
    fclose(fpOutput);

    return 0;


}