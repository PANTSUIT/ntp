
#include <stdio.h>
#include "ntp.h"

int main(void) 
{
  ntp_test();
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  printf("Little endian!\r\n");
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
  printf("Big endian!\r\n");
#endif
  return 0;
}


