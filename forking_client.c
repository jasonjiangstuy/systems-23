#include "pipe_networking.h"


int main() {

  int sd = client_handshake();
  while (1) {
    char *array = calloc(50, sizeof(char));

    printf("what is your input (lower or uppercase)\n");
    fgets(array, 50, stdin);

    if (array[49] != '\0' && array[49] != '\n'){
      printf("%s\n", "sorry input too large");
      break;
    }

    if (array[strlen(array) -1 ] == '\n'){
      array[strlen(array) -1] = '\0';
    }
    // printf("%s\n", array);
    // send to server
    write(sd, array, 50);
    // recieve from server
    read(sd,array, 50);
    printf("child rot13: %s\n", array);
  }
}
