/**
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Avinal Kumar <avinal.xlvii@gmail.com>
 *
 * @file	client.c
 * @brief	Client implementation
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
 * @brief initialize client side connection
 *
 * @param address   address to start connection on
 * @param port      port number
 * @return socket
 */
int initialize_client(in_addr_t address, in_port_t port) {
  int sock_desc;
  struct sockaddr_in client;
  memset(&client, 0, sizeof(client));

  if ((sock_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("%sSocket creation failed !!%s Exiting ...", ERROR, CLS);
    exit(EXIT_FAILURE);
  }

  client.sin_addr.s_addr = address;
  client.sin_family = AF_INET;
  client.sin_port = port;

  if (connect(sock_desc, (struct sockaddr *)&client, sizeof(client)) < 0) {
    printf("%sCould not connect to server !!%s Exiting ...", ERROR, CLS);
    exit(EXIT_FAILURE);
  }
  return sock_desc;
}

/**
 * @brief check connectivity with the server
 *
 * @param sock_desc socket
 */
void check_connection(int sock_desc) {
  printf("Checking connectivity with server ...\n");
  char *check_message = "hello", check_buffer[10];

  if ((send(sock_desc, check_message, sizeof(check_message), 0) > 0) &&
      (read(sock_desc, check_buffer, sizeof(check_buffer)) > 0) &&
      !strcmp(check_buffer, check_message)) {
    printf("%sConnectivity is good%s\n", SUCCESS, CLS);
  } else {
    printf("%sCould not connect to server !!%s Exiting ...", ERROR, CLS);
    exit(EXIT_FAILURE);
  }
}

/**
 * @brief handles file transfer between client and server
 *
 * @param sock_desc socket
 */
void start_transfer(int sock_desc) {
  char filename[64];
  FILE *data;
  do {
    printf("Please enter the filename you want to send: ");
    scanf("%s", filename);
    if (!(data = fopen(filename, "r"))) {
      printf("%sNo such file!!%s\n", ERROR, CLS);
    }
  } while (!data);

  fseek(data, 0, SEEK_END);
  int filesize = ftell(data);
  fseek(data, 0, SEEK_SET);
  printf(
      "\nSending file properties to the server ...\n\n\tFile Name: "
      "%s%s%s\n\tFile Size: %s%d bytes%s\n\n",
      INFO, filename, CLS, INFO, filesize, CLS);
  send(sock_desc, filename, sizeof(filename), 0);
  write(sock_desc, &filesize, sizeof(filesize));

  printf("Sending %s to the server ...\n", filename);

  char buffer[(filesize > 10240 ? 10240 : filesize)];

  do {
    int nbyte = fread(buffer, sizeof(buffer[0]), sizeof(buffer), data);
    write(sock_desc, buffer, nbyte);
  } while (!feof(data));

  printf("%sFile sent successfully!%s\n", SUCCESS, CLS);
  close(data);
}

int main(int argc, char const *argv[]) {
  int sock_desc = initialize_client(inet_addr("127.0.0.1"), 4700);
  check_connection(sock_desc);
  start_transfer(sock_desc);
  close(sock_desc);
  return 0;
}
