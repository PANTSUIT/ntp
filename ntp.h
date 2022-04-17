

#ifndef __NTP_H_
#define __NTP_H_

#include <stdint.h>
#include <time.h>


/**
 * The timestamp of 1970 - 1900
 *
 */
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
  uint16_t _integer;
  uint16_t _fraction;

}ntp_timestamp_32_t;

typedef struct {
  uint32_t _integer;
  uint32_t _fraction;
}ntp_timestamp_64_t;

typedef struct {

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  uint8_t mode: 3;
  uint8_t vn: 3;
  uint8_t li: 2;

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__

  uint8_t li: 2;
  uint8_t vn: 3;
  uint8_t mode: 3;
#endif

  uint8_t stratum;
  uint8_t poll;
  int8_t precision: 8;

  ntp_timestamp_32_t root_delay;
  ntp_timestamp_32_t root_disperision;
  uint32_t reference_identifier;
  ntp_timestamp_64_t reference_timestamp;
  ntp_timestamp_64_t originate_timestamp;
  ntp_timestamp_64_t receive_timestamp;
  ntp_timestamp_64_t transmit_timestamp;
} ntp_protocol_t;

extern time_t _ntp(char* url, uint16_t port);

extern int ntp_test(void);

#endif /* __NTP_H_ */
