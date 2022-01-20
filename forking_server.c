#include "pipe_networking.h"


int main() {

  int sd = server_setup();
  
  printf("Socket file descripter: %d\n", sd);
  listen(sd,10);
  server_connect(sd);
}
