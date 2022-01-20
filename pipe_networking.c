#include "pipe_networking.h"

void * rot13(int client_socket){
    // rot13
    char *array = calloc(50, sizeof(char));
    while (read(client_socket,array, 50)) {
      int i = 0;
      int boundleft;
      int boundright;
      for (; i < strlen(array); i++){
        // used to exceed the char size limit of 127
        int target = array[i];
        int charater = 0;
        if (65 <= target && target <= 90){
          // cap charaters
          boundleft = 65;
          boundright = 90;
          charater = 1;

        }else if(97 <= target && target <= 122){
          // lower charaters
          boundleft = 97;
          boundright = 122;
          charater = 1;
        }
        target = target + 13;
        if (charater){
          if (boundright - target < 0){
            // pass the alphabet
            // wrap around
            target = boundleft + target - boundright - 1;
          }
        }

        array[i] = target;
    }
    printf("Processed array %s\n", array);
    write(client_socket, array, 50);
  }
}
/*=========================
  server_setup
  args:

  creates the WKP (upstream) and opens it, waiting for a
  connection.

  removes the WKP once a connection has been made

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_setup() {
  //use getaddrinfo
  struct addrinfo * hints, * results;
  hints = calloc(1,sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM; //TCP socket
  // hints->ai_flags = AI_PASSIVE; //only needed on server
  getaddrinfo("127.0.0.1", "9845", hints, &results);  //Server sets node to NULL

  //create socket
  int sd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

  bind(sd, results->ai_addr, results->ai_addrlen);

  //DO STUFF

  free(hints);
  freeaddrinfo(results);
  return sd;
}

/*=========================
  server_connect
  args: int from_client

  handles the subserver portion of the 3 way handshake

  returns the file descriptor for the downstream pipe.
  =========================*/
int server_connect(int sd) {
  int client_socket;
  socklen_t sock_size;
  struct sockaddr_storage client_address;
  sock_size = sizeof(client_address);
  client_socket = accept(sd,(struct sockaddr *)&client_address, &sock_size);
  // forking server part
  int f = fork();
  if (f){
    server_connect(sd);
  }else{
    rot13(client_socket);
  }
}


// /*=========================
//   server_handshake
//   args: int * to_client
//
//   Performs the server side pipe 3 way handshake.
//   Sets *to_client to the file descriptor to the downstream pipe.
//
//   returns the file descriptor for the upstream pipe.
//   =========================*/
// int server_handshake(int *to_client) {
//
//   int b, from_client;
//   char buffer[HANDSHAKE_BUFFER_SIZE];
//
//   printf("[server] handshake: making wkp\n");
//   b = mkfifo(WKP, 0600);
//   if ( b == -1 ) {
//     printf("mkfifo error %d: %s\n", errno, strerror(errno));
//     exit(-1);
//   }
//   //open & block
//   from_client = open(WKP, O_RDONLY, 0);
//   //remove WKP
//   remove(WKP);
//
//   printf("[server] handshake: removed wkp\n");
//
//
//   *to_client = open(buffer, O_WRONLY, 0);
//   //create SYN_ACK message
//   srand(time(NULL));
//   int r = rand() % HANDSHAKE_BUFFER_SIZE;
//   sprintf(buffer, "%d", r);
//
//   write(*to_client, buffer, sizeof(buffer));
//   //rad and check ACK
//   read(from_client, buffer, sizeof(buffer));
//
//   return from_client;
// }


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake() {

  struct addrinfo * hints, * results;
  hints = calloc(1,sizeof(struct addrinfo));
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM; //TCP socket
  getaddrinfo("127.0.0.1", "9845", hints, &results);  //Server sets node to NULL

  int sd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);

  bind(sd, results->ai_addr, results->ai_addrlen);


  free(hints);
  freeaddrinfo(results);

  socket(results->ai_family, results->ai_socktype, results->ai_protocol);
  connect(sd, results->ai_addr, results->ai_addrlen);

  return sd;

}
