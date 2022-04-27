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

#include <sys/time.h>


#include "ntp.h"

void ntp_protocol_default(ntp_protocol_t *_protocol) {
  if(_protocol) {
    memset(_protocol, 0x00, sizeof(ntp_protocol_t));
    _protocol->li = 0;
    _protocol->vn = 3;
    _protocol->mode = 3;
    _protocol->stratum = 0;
    _protocol->poll = 4;
    _protocol->precision = -6;

    struct timeval _tv;
    gettimeofday(&_tv, NULL);

    _protocol->transmit_timestamp._integer = htonl(time(NULL) + SUB70) ;
    _protocol->transmit_timestamp._fraction = 0;
    printf("========> %sm size: %ld\n", __FUNCTION__, sizeof(ntp_protocol_t));
  }
}


void ntp_protocol_show(ntp_protocol_t _protocol) {
  printf("_prottocol->li: %d\n", _protocol.li);
  printf("_prottocol->vn: %d\n", _protocol.vn);
  printf("_prottocol->mode: %d\n", _protocol.mode);
  printf("_prottocol->stratum: %d\n", _protocol.stratum);
  printf("_prottocol->poll: %d\n", _protocol.poll);
  printf("_prottocol->precision: %d\n", _protocol.precision);

  printf("_prottocol->root_delay: integer: %d, fraction: %d\n", _protocol.root_delay._integer, _protocol.root_delay._fraction);
  printf("_prottocol->root_disperision: integer: %d, fraction: %d\n", _protocol.root_disperision._integer, _protocol.root_disperision._fraction);

  printf("_prottocol->reference_identifier: %d\n", _protocol.reference_identifier);

  printf("_prottocol->reference_timestamp: integer: %d, fraction: %d\n", _protocol.reference_timestamp._integer, _protocol.reference_timestamp._fraction);
  printf("_prottocol->originate_timestamp: integer: %d, fraction: %d\n", _protocol.originate_timestamp._integer, _protocol.originate_timestamp._fraction);
  printf("_prottocol->receive_timestamp: integer: %d, fraction: %d\n", _protocol.receive_timestamp._integer, _protocol.receive_timestamp._fraction);
  printf("_prottocol->transmit_timestamp: integer: %d, fraction: %d\n", _protocol.transmit_timestamp._integer, _protocol.transmit_timestamp._fraction);
}

void ntp_protocol_show_recv(ntp_protocol_t _protocol) {
  printf("_prottocol->li: %d\n", _protocol.li);
  printf("_prottocol->vn: %d\n", _protocol.vn);
  printf("_prottocol->mode: %d\n", _protocol.mode);
  printf("_prottocol->stratum: %d\n", _protocol.stratum);
  printf("_prottocol->poll: %d\n", _protocol.poll);
  printf("_prottocol->precision: %d\n", _protocol.precision);

  printf("_prottocol->root_delay: integer: %d, fraction: %d\n", ntohs(_protocol.root_delay._integer), _protocol.root_delay._fraction);
  printf("_prottocol->root_disperision: integer: %d, fraction: %d\n", ntohs(_protocol.root_disperision._integer), _protocol.root_disperision._fraction);

  printf("_prottocol->reference_identifier: %d\n", _protocol.reference_identifier);

  printf("_prottocol->reference_timestamp: integer: %d, fraction: %d\n", ntohl(_protocol.reference_timestamp._integer) - SUB70, _protocol.reference_timestamp._fraction);
  printf("_prottocol->originate_timestamp: integer: %d, fraction: %d\n", ntohl(_protocol.originate_timestamp._integer) - SUB70, _protocol.originate_timestamp._fraction);
  printf("_prottocol->receive_timestamp: integer: %d, fraction: %d\n", ntohl(_protocol.receive_timestamp._integer) - SUB70, _protocol.receive_timestamp._fraction);
  printf("_prottocol->transmit_timestamp: integer: %d, fraction: %d\n", ntohl(_protocol.transmit_timestamp._integer) - SUB70, _protocol.transmit_timestamp._fraction);
}

time_t _ntp(char* url, uint16_t port)
{
  char server_ip[32] = {0};
  struct hostent *host;
  host = gethostbyname(url);
  if(host != NULL){
    printf("Host name: %s\n", host->h_name);

    printf("h_length: %d\n", host->h_length);

    for(int i = 0; host->h_aliases[i]; i++){
      printf("Aliases: %d, %s\n", i, host->h_aliases[i]);
    }

    for(int i = 0; host->h_addr_list[i]; i++){
      printf("Address: %d, %s\n", i, inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
    }
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

  struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_addr.s_addr = inet_addr(server_ip),
    .sin_port = htons(123),
  };

  struct timeval tv = {
    .tv_sec = 3,
    .tv_sec = 0
  };

  int sock_fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
  // int sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock_fd < 0) {
    printf("socket error: %s (errno: %d)\n", strerror(errno), errno);
    exit(-1);
  }


  if (setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) < 0) {
    printf("set recv timtout error: %s (errno: %d)\n", strerror(errno), errno);
    exit(-1);
  }

  int try_count = 0;
  time_t origin_time = 0, receive_time = 0;
  time_t delay_time = 0, diff_time = 0;
  do {
    printf("try_count: %d\n", try_count);

    origin_time = time(NULL) + diff_70;
    printf("origin_time: %ld\n", origin_time);

    ntp_protocol_default(&send_pack);

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
    printf("receive_time: %ld\n", receive_time);
    

    ntp_protocol_show_recv(recv_pack);
    
    double t1, t2, t3, t4;


    t1 = ntohl(recv_pack.originate_timestamp._integer);
    t2 = ntohl(recv_pack.receive_timestamp._integer);
    t3 = ntohl(recv_pack.transmit_timestamp._integer);
    t4 = receive_time;

    printf("t1: %lf\n", t1);
    printf("t2: %lf\n", t2);
    printf("t3: %lf\n", t3);
    printf("t4: %lf\n", t4);

    diff_time = ((t2 - t1) + (t3 - t4)) / 2;
    delay_time = (t4 - t1) - (t3 - t2);

    printf("diff_time: %ld\n", diff_time);
    printf("delay_time: %ld\n", delay_time);

    // new_time = time(NULL) + diff_time + delay_time;
    new_time = time(NULL) + diff_time;

    printf(" %s\n", ctime(&new_time));

  } while (ret < 0 && try_count < 5);

  close(sock_fd);
  return new_time;
}

int ntp_test(void)
{
  time_t _t = time(NULL);
  printf("start time: %s\n", ctime(&_t));

  // _ntp(server_name, 123);
  _ntp("ntp.ntsc.ac.cn", 123);

  _t = time(NULL);
  printf("end time: %s\n", ctime(&_t));
  
  return 0;
}
