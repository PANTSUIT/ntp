

#ifndef __NTP_H_
#define __NTP_H_

#include <stdint.h>
#include <time.h>

#define SUB70 0x83AA7E80

//阿里云授时服务器
//  ntp.aliyun.com
//
// 腾讯云授时服务器
//  time1.cloud.tencen.tom
//  time2.cloud.tencen.tom
//  time3.cloud.tencen.tom
//  time4.cloud.tencen.tom
//  time5.cloud.tencen.tom
//
//  ntp.tuna.tsinghua.edu.cn      清华大学（ipv4/ipv6）

// 苹果提供的授时服务器
// time1.apple.com
// time2.apple.com
// time3.apple.com
// time4.apple.com
// time5.apple.com
// time6.apple.com
// time7.apple.com

// Google提供的授时服务器
// time1.google.com
// time2.google.com
// time3.google.com
// time4.google.com

typedef struct {

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  uint32_t precision : 8;
  uint32_t poll : 8;
  uint32_t stratum : 8;
  uint32_t mode : 3;
  uint32_t vn : 3;
  uint32_t li : 2;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  uint32_t li: 2;
  uint32_t vn: 3;
  uint32_t mode: 3;
  uint32_t stratum: 8;
  uint32_t poll: 8;
  uint32_t precision: 8;
#endif

  uint32_t root_delay;
  uint32_t root_disperision;
  uint32_t reference_identifier;
  uint64_t reference_timestamp;
  uint64_t originate_timestamp;
  uint64_t receive_timestamp;
  uint64_t transmit_timestamp;

} ntp_protocol_t;

extern time_t _ntp(char* url, uint16_t port);

extern int ntp_test(void);

#endif /* __NTP_H_ */
