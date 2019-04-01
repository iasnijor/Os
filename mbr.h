#ifndef MBR
#define MBR
struct __attribute__ ((packed)) partitionTable{

unsigned char unused[8];
unsigned int abssector;
unsigned int numsector;

};

struct __attribute__ ((packed)) BootRecord{
unsigned char unused[446];
partitionTable Partition[4];
unsigned short buffer;
};

#endif
