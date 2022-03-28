#ifndef _sharedMemory_h_
#define _sharedMemory_h_

int sharedMemory();

typedef signed int INT32;
#define  NOT_READY  -1
#define  FILLED     0
#define  TAKEN      1

struct Memory 
{
/*     INT32  status;
     INT32  pkt_index;
     INT32  data_cnt;
     */
     INT32  data;
};


#endif
