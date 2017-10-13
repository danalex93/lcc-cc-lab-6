#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 1024

char *find_codification(char *input, int size){
  if (size == 43){
    // the3quick9brown15fox19jumps25over30the34lazy39dog
    if (input[3] == ' ' && input[9] == ' ' && input[15] == ' ' && input[19] == ' ' && input[25] == ' ' && input[30] == ' ' && input[34] == ' ' && input[39] == ' '){
      char *output = (char *) malloc(sizeof(char)*26);

      output[input[36]-'a'] = 'a';
      output[input[10]-'a'] = 'b';
      output[input[7]-'a'] = 'c';
      output[input[40]-'a'] = 'd';
      output[input[2]-'a'] = 'e';
      output[input[16]-'a'] = 'f';
      output[input[42]-'a'] = 'g';
      output[input[1]-'a'] = 'h';
      output[input[6]-'a'] = 'i';
      output[input[20]-'a'] = 'j';
      output[input[8]-'a'] = 'k';
      output[input[35]-'a'] = 'l';
      output[input[22]-'a'] = 'm';
      output[input[14]-'a'] = 'n';
      output[input[12]-'a'] = 'o';
      output[input[23]-'a'] = 'p';
      output[input[4]-'a'] = 'q';
      output[input[11]-'a'] = 'r';
      output[input[24]-'a'] = 's';
      output[input[0]-'a'] = 't';
      output[input[5]-'a'] = 'u';
      output[input[27]-'a'] = 'v';
      output[input[13]-'a'] = 'w';
      output[input[18]-'a'] = 'x';
      output[input[38]-'a'] = 'y';
      output[input[37]-'a'] = 'z';

      return output;
    }
  }
  return NULL;
}

int main(int argc, char const *argv[])
{
  int i, j, k, l, bsn_counter, phrase_size[3], triplets;
  char helper;
  scanf("%d\n", &triplets);

  char *triplet[3];

  for(i = 0; i < triplets; i++){
    triplet[0] = (char*) malloc(sizeof(char)*MAX_SIZE);
    triplet[1] = (char*) malloc(sizeof(char)*MAX_SIZE);
    triplet[2] = (char*) malloc(sizeof(char)*MAX_SIZE);

    bsn_counter = 0;
    j = 0;

    while(bsn_counter < 3){
      if (j == MAX_SIZE){
        realloc(triplet[bsn_counter], MAX_SIZE*sizeof(char));
      }

      helper = getchar();
      
      if (helper == '\n' || helper == EOF){
        phrase_size[bsn_counter] = j;
        bsn_counter++;
        j = 0;
      } else {
        triplet[bsn_counter][j] = helper;
        j++;
      }
    }

    char *codification = NULL;
    /* check if base string in first phrase */
    codification = find_codification(triplet[0], phrase_size[0]);
    if (codification == NULL){
      codification = find_codification(triplet[1], phrase_size[1]);
      if (codification == NULL){
        codification = find_codification(triplet[2], phrase_size[2]);
      }
    }

    for (l = 0; l < 3; l++){
      for (k = 0; k < phrase_size[l]; k++){
        if (triplet[l][k] == ' '){
          printf(" ");
        } else {
          printf("%c", codification[triplet[l][k]-'a']);
        }
      }
      printf("\n");
    }

    free(triplet[0]);
    free(triplet[1]);
    free(triplet[2]);
  }

  return 0;
}