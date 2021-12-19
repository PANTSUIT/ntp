#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include "ntp.h"


time_t _ntp(char* url, uint16_t port)
{
  char server_ip[32] = {0};
  struct hostent *host;
  host = gethostbyname(url);
  if(host != NULL){
    // printf("Host name: %s\n", host->h_name);
    //
    // printf("h_length: %d\n", host->h_length);
    //
    // for(int i = 0; host->h_aliases[i]; i++){
    //   printf("Aliases: %d, %s\n", i, host->h_aliases[i]);
    // }
    //
    // for(int i = 0; host->h_addr_list[i]; i++){
    //   printf("Address: %d, %s\n", i, inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
    // }
  } else {
    return -1;
  }
  snprintf(server_ip, 32, "%s", inet_ntoa(*(struct in_addr *)host->h_addr_list[0]));
  if(strlen(server_ip) == 0) {
    return -1;
  }

  int ret = 0;
  time_t new_time = 0;
  ntp_protocol_t send_pack, recv_pack;
  static const time_t diff_70 = 0x83AA7E80;

  int sock_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
  if (sock_fd < 0) {
    printf("socket error: %s (errno: %d)\n", strerror(errno), errno);
    exit(-1);
  }

  struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_addr.s_addr = inet_addr(server_ip),
    .sin_port = htons(123),
  };

  send_pack.li = 0;
  send_pack.vn = 3;
  send_pack.mode = 3;
  send_pack.stratum = 0;
  send_pack.poll = 4;
  send_pack.precision = -6;

  uint32_t* p32 = (uint32_t*)&send_pack;
  *p32 = htonl(*p32);

  struct timeval tv = {
    .tv_sec = 3,
    .tv_sec = 0
  };

  if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
    printf("set recv timtout error: %s (errno: %d)\n", strerror(errno), errno);
    exit(-1);
  }

  int try_count = 0;
  time_t origin_time = 0, receive_time = 0;
  time_t delay_time = 0, diff_time = 0;
  do {
    // printf("try_count: %d\n", try_count);
    memset(&recv_pack, 0x00, sizeof(ntp_protocol_t));

    origin_time = time(NULL) + diff_70;
    // printf("origin_time: %ld\n", origin_time);
    ret = sendto(sock_fd, &send_pack, sizeof(ntp_protocol_t), 0, (struct sockaddr*)&addr, sizeof(struct sockaddr));
    if (ret < 0) {
      printf("send error: %s (errno: %d)\n", strerror(errno), errno);
      return -1;
    }

    ret = recv(sock_fd, &recv_pack, sizeof(ntp_protocol_t), 0);
    if (ret < 0) {
      printf("recv error: %s (errno: %d)\n", strerror(errno), errno);
      sleep(3);
      try_count++;
      continue;
    }
    receive_time = time(NULL) + diff_70;
    // printf("receive_time: %ld\n", receive_time);

    recv_pack.root_delay = ntohl(recv_pack.root_delay);
    recv_pack.root_disperision = ntohl(recv_pack.root_disperision);
    recv_pack.reference_identifier = ntohl(recv_pack.reference_identifier);
    recv_pack.reference_timestamp = ntohl(recv_pack.reference_timestamp);
    recv_pack.originate_timestamp = ntohl(recv_pack.originate_timestamp);
    recv_pack.receive_timestamp = ntohl(recv_pack.receive_timestamp);
    recv_pack.transmit_timestamp = ntohl(recv_pack.transmit_timestamp);

    // printf("root_delay: %d\n", recv_pack.root_delay);
    // printf("root_disperision: %d\n", recv_pack.root_disperision);
    // printf("reference_identifier: %d\n", recv_pack.reference_identifier);
    // printf("reference_timestamp: %ld\n", recv_pack.reference_timestamp);
    // printf("originate_timestamp: %ld\n", recv_pack.originate_timestamp);
    // printf("receive_timestamp: %ld\n", recv_pack.receive_timestamp);
    // printf("transmit_timestamp: %ld\n", recv_pack.transmit_timestamp);

    diff_time = ((recv_pack.receive_timestamp - origin_time) - (recv_pack.transmit_timestamp- receive_time)) >> 1;
    delay_time = (receive_time - origin_time) - (recv_pack.transmit_timestamp - recv_pack.receive_timestamp);

    // printf("diff_time: %ld\n", diff_time);
    // printf("delay_time: %ld\n", delay_time);

    new_time = time(NULL) + diff_time + delay_time;

    // printf("new_time: %ld\n", new_time);
    printf("%s\n", ctime(&new_time));


  } while (ret < 0 && try_count < 5);

  close(sock_fd);
  return new_time;
}

int ntp_test(void)
{
  // int ret = 0;
  // if (argc < 2) {
  //   printf("-------- Usages: \n");
  //   printf(" %s <ntp_server> \n", argv[0]);
  //   exit(-1);
  // }
  //
  // ntp_protocol_t ntp_send_pack = { 0 }, ntp_recv_pack = { 0 };
  //
  // int c_sock_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
  // if (c_sock_fd < 0) {
  //   printf("socket error: %s (errno: %d)\n", strerror(errno), errno);
  //   exit(-1);
  // }
  //
  // char ntp_server[16];
  // snprintf(ntp_server, 16, "%s", argv[1]);
  // printf("server: %s\n", ntp_server);
  //
  // struct sockaddr_in addr = {
  //   .sin_family = AF_INET,
  //   .sin_addr = inet_addr(ntp_server),
  //   .sin_port = htons(123)
  // };
  // printf("server: %#X\n", addr.sin_addr.s_addr);
  //
  // uint8_t* p = (uint8_t*)&ntp_send_pack;
  //
  // ntp_send_pack.li = 0;
  // ntp_send_pack.vn = 3;
  // ntp_send_pack.mode = 3;
  // ntp_send_pack.stratum = 0;
  // ntp_send_pack.poll = 4;
  // ntp_send_pack.precision = -6;
  //
  // uint32_t* p32 = (uint32_t*)&ntp_send_pack;
  // *p32 = htonl(*p32);
  //
  // for (int i = 0; i < sizeof(ntp_protocol_t); i++) {
  //   printf("%#02X ", *(p + i));
  // }
  // printf("\n");
  // printf("-------- data end\n");
  //
  // printf("-------- send start\n");
  //
  //
  // struct timeval tv = {
  //   .tv_sec = 3,
  //   .tv_sec = 0
  // };
  //
  // if (setsockopt(c_sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
  //   printf("set recv timtout error: %s (errno: %d)\n", strerror(errno), errno);
  //   exit(-1);
  // }
  //
  // int count = 0;
  // do {
  //
  //   memset(&ntp_recv_pack, 0x00, sizeof(ntp_recv_pack));
  //
  //   ntp_send_pack.originate_timestamp = time(NULL);
  //   printf("originate_timestamp: %ld\n", ntp_send_pack.originate_timestamp);
  //
  //   printf("count: %d\n", count++);
  //   if (sendto(c_sock_fd, &ntp_send_pack, sizeof(ntp_send_pack), 0, (struct sockaddr*)&addr, sizeof(struct sockaddr)) < 0) {
  //     printf("send error: %s (errno: %d)\n", strerror(errno), errno);
  //   }
  //
  //   ret = recv(c_sock_fd, &ntp_recv_pack, sizeof(ntp_recv_pack), 0);
  //   if (ret < 0) {
  //     printf("recv error: %s (errno: %d)\n", strerror(errno), errno);
  //   }
  //
  //   p = (uint8_t*)&ntp_recv_pack;
  //   for (int i = 0; i < sizeof(ntp_protocol_t); i++) {
  //     printf("%#X ", *(p + i));
  //   }
  //   printf("\n");
  //
  //   ntp_recv_pack.root_delay = ntohl(ntp_recv_pack.root_delay);
  //   ntp_recv_pack.root_disperision = ntohl(ntp_recv_pack.root_disperision);
  //   ntp_recv_pack.reference_identifier = ntohl(ntp_recv_pack.reference_identifier);
  //   ntp_recv_pack.reference_timestamp = ntohl(ntp_recv_pack.reference_timestamp);
  //   ntp_recv_pack.originate_timestamp = ntohl(ntp_recv_pack.originate_timestamp);
  //   ntp_recv_pack.receive_timestamp = ntohl(ntp_recv_pack.receive_timestamp);
  //   ntp_recv_pack.transmit_timestamp = ntohl(ntp_recv_pack.transmit_timestamp);
  //
  //   printf("root_delay: %d\n", ntp_recv_pack.root_delay);
  //   printf("root_disperision: %d\n", ntp_recv_pack.root_disperision);
  //   printf("reference_identifier: %d\n", ntp_recv_pack.reference_identifier);
  //   printf("reference_timestamp: %ld\n", ntp_recv_pack.reference_timestamp);
  //   printf("originate_timestamp: %ld\n", ntp_recv_pack.originate_timestamp);
  //   printf("receive_timestamp: %ld\n", ntp_recv_pack.receive_timestamp);
  //   printf("transmit_timestamp: %ld\n", ntp_recv_pack.transmit_timestamp);
  //
  //   sleep(3);
  // } while (1);
  // close(c_sock_fd);
  

  char server_name[256] = {0};
  printf("Enter the ntp server name:");
  scanf("%s", server_name);
  getchar();
  _ntp(server_name, 123);
  
  return 0;
}
