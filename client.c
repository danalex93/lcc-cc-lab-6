#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT "3490" // the port client will be connecting to 
#define MAXDATASIZE 100 // max number of bytes we can get at once 
#define MAX_SIZE 1024

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char const *argv[])
{
  int i, j, k, l, bsn_counter, phrase_size[3], triplets;
  int valid, correct;
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

    /* Check if coded phrase exists */
    valid = 0;
    for (l = 0; l < 3; l++){
      if (phrase_size[l] == 43){
        if (triplet[l][3] == ' ' && triplet[l][9] == ' ' && triplet[l][15] == ' ' && triplet[l][19] == ' ' && triplet[l][25] == ' ' && triplet[l][30] == ' ' && triplet[l][34] == ' ' && triplet[l][39] == ' '){
          valid = 1;
          correct = l;
          break;
        }
      }
    }
    
    if (valid == 0){
      printf("Invalid message for decoding!!");
      return -1;
    } 

    /* Send to Server */
    int sockfd, numbytes;  
    char codification[26];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
      if ((sockfd = socket(p->ai_family, p->ai_socktype,
          p->ai_protocol)) == -1) {
        perror("client: socket");
        continue;
      }

      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        perror("client: connect");
        close(sockfd);
        continue;
      }

      break;
    }

    if (p == NULL) {
      fprintf(stderr, "client: failed to connect\n");
      return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
        s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    printf("Enviar: %s\n", triplet[correct]);
    send(sockfd, &triplet[correct], 43, 0);

    /* Receive Codification from Server */
    if ((numbytes = recv(sockfd, codification, 26, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    close(sockfd);

    /* Print Decodification */
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

    /* Free Memory Allocation for Strings */
    free(triplet[0]);
    free(triplet[1]);
    free(triplet[2]);
  }

  return 0;
}