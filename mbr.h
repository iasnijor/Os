#ifndef MBR
#define MBR
struct  partitionTable{
char unused[8];
int abssector;
int numsector;
}
struct diskpart{
char trash[446];
partitionTable Partition[4];
char buffer[2];

}
#endif MBR
