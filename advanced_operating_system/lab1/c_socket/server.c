/**
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Avinal Kumar <avinal.xlvii@gmail.com>
 *
 * @file	server.c
 * @brief	server side implementation
 */

// Green
#define SUCCESS "\033[0;32m"
// Red
#define ERROR "\033[0;31m"
// Cyan
#define INFO "\033[0;36m"
// Clear
#define CLS "\033[0m"

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

/**
 * @brief initialize server side connection
 *
 * @param address   address to start connection on
 * @param port      port number
 * @return socket
 */
int initialize_server(in_port_t port) {
  int server_sock_desc, client_sock_desc;
  struct sockaddr_in server, client;

  memset(&server, 0, sizeof(server));
  memset(&client, 0, sizeof(client));

  if ((server_sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("%sSocket creation failed !!%s Exiting ...", ERROR, CLS);
    exit(EXIT_FAILURE);
  }

  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_family = AF_INET;
  server.sin_port = port;

  if (bind(server_sock_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
    printf("%sError in binding !!%s Exiting ...", ERROR, CLS);
    exit(EXIT_FAILURE);
  } else if (listen(server_sock_desc, 10) < 0) {
    printf("%sError in listening !!%s Exiting ...", ERROR, CLS);
    exit(EXIT_FAILURE);
  }

  socklen_t len = sizeof(client);
  if ((client_sock_desc =
           accept(server_sock_desc, (struct sockaddr *)&client, &len)) < 0) {
    printf("%sError in temporary socket creation !!%s Exiting ...", ERROR, CLS);
    exit(EXIT_FAILURE);
  }

  return client_sock_desc;
}

/**
 * @brief check connectivity with the client
 *
 * @param client_sock_desc socket
 */
void check_connection(int client_sock_desc) {
  printf("Checking connectivity with client ...\n");
  char *check_message = "hello", check_buffer[64];

  if ((read(client_sock_desc, check_buffer, sizeof(check_buffer)) > 0) &&
      (send(client_sock_desc, check_message, sizeof(check_message), 0) > 0) &&
      !strcmp(check_buffer, check_message)) {
    printf("%sConnectivity is good%s\n", SUCCESS, CLS);
  } else {
    printf("%sCould not connect to client !!%s Exiting ...", ERROR, CLS);
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief handles file transfer between client and server
 *
 * @param client_sock_desc socket
 */
void start_transfer(int client_sock_desc) {
  char filename[64];
  int filesize;
  read(client_sock_desc, filename, sizeof(filename));
  read(client_sock_desc, &filesize, sizeof(int));

  if (filesize == 0 && strlen(filename) == 0) {
    printf("%sConnection lost from client!!%s Exiting ...", ERROR, CLS);
    exit(EXIT_FAILURE);
  }
  printf(
      "\nGetting file properties ...\n\n\tFile Name: %s%s%s\n\tFile Size: %s%d "
      "bytes%s\n\n",
      INFO, filename, CLS, INFO, filesize, CLS);
  char rec_name[64] = "received-";
  strcat(rec_name, filename);
  printf("Getting %s from the client ...\n", filename);

  FILE *data;
  data = fopen(rec_name, "w");
  char buffer[(filesize > 10240 ? 10240 : filesize)];
  int n_bytes;
  do {
    n_bytes = read(client_sock_desc, buffer, sizeof(buffer));
    fwrite(buffer, 1, n_bytes, data);
  } while (n_bytes > 0);
  fclose(data);
  printf("%sFile received successfully!%s\n", SUCCESS, CLS);
}

int main(int argc, char const *argv[]) {
  int client_sock_desc = initialize_server(4700);
  check_connection(client_sock_desc);
  start_transfer(client_sock_desc);
  close(client_sock_desc);
  return 0;
}
