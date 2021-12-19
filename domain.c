
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
  struct hostent *h;
  h = gethostbyname(argv[1]);
  if(h != NULL){
    printf("Host name: %s\n", h->h_name);

    printf("h_length: %d\n", h->h_length);

    for(int i = 0; h->h_aliases[i]; i++){
      printf("Aliases: %d, %s\n", i, h->h_aliases[i]);
    }

    for(int i = 0; h->h_addr_list[i]; i++){
      printf("Address: %d, %s\n", i, inet_ntoa(*(struct in_addr *)h->h_addr_list[i]));
    }
  }

  // int rc;
  // char buf[100];
  // struct addrinfo* result = NULL;
  // struct addrinfo hints = {
  //   0, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP, 0, NULL, NULL, NULL
  // };
  //
  // if ((rc = getaddrinfo("iservice.10010.com", NULL, &hints, &result)) == 0) {
  //
  //   struct addrinfo* res = result;
  //
  //   while (res) {
  //
  //     if (res->ai_family == AF_INET) {
  //
  //       printf("%s\r\n", inet_ntoa(((struct sockaddr_in*)res->ai_addr)->sin_addr));
  //
  //     } else if (res->ai_family == AF_INET6) {
  //
  //       printf("%s\r\n", inet_ntop(AF_INET6, &(((struct sockaddr_in6*)res->ai_addr)->sin6_addr), buf, 100));
  //     }
  //
  //     res = res->ai_next;
  //   }
  //
  //   freeaddrinfo(result);
  // }
  return EXIT_SUCCESS;
}
