#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold

void sigchld_handler(int s)
{
  // waitpid() might overwrite errno, so we save and restore it:
  int saved_errno = errno;

  while(waitpid(-1, NULL, WNOHANG) > 0);

  errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

char *find_codification(char *input){
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

int main(int argc, char const *argv[])
{
  /* Set-up socket connection */
  int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char s[INET6_ADDRSTRLEN];
  int rv;
  char *codification = NULL;
  char input[43];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
        p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
        sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(servinfo); // all done with this structure

  if (p == NULL)  {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  int size;

  while(1)
  {
    /* Receive communication from client */
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    /* Begin receiving string with encoded phrase */
    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    printf("server: got connection from %s\n", s);

    size = recv(sockfd, input, 43, 0);
    printf("Input: %s, size: %d\n", input, size);

    if (!fork()) {
      close(sockfd);

      /* Process codification */
      codification = find_codification(input);

      /* Send codification to client */
      if (send(new_fd, codification, 26, 0) == -1)
      {
        perror("send");
      }
      
      close(new_fd);

      exit(0);
    }
    close(new_fd);

    /* Process codification */
    codification = find_codification(input);
  }

  return 0;
}