#ifndef MBR
#define MBR
struct  partitionTable{

unsigned char unused[8];
unsigned int abssector;
unsigned int numsector;

};

struct BootRecord{

unsigned char trash[0x1be];
partitionTable Partition[4];
unsigned char buffer[2];

};

#endif
