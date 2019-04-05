#include "vdi.h"
#include "vdifile.h"
#include "readVdi.h"
#include "mbr.h"
#include "superBlock.h"

#include <iostream>
#include <cstdint>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

int main(int  argc,  char* argv[]){
  //Opening VDI FIle
  int vfile;
  VDIFile* f =new VDIFile();
  vfile=vdifileopen(f,argv[1]);
  // Reading Header
  off_t offset;
  offset = VDISeek(f,0,SEEK_SET);
  int head=VDIread(f,&(f->header), sizeof(f->header));
  cout << hex<< "Header Magic Number: " <<f->header.imgsignature <<endl;


  /*   int map;
  int vdiMap[f->header.blocks];
  map= readMap(f,vdiMap);*/

  // Reading MBR
  BootRecord b;
  int mb= readMBR(f,b);

  // Reading Superblock
  Superblock super;
  int superblock_loc = b.Partition[0].abssector* 512 + 1024;
  cout << "loc: "<< superblock_loc<< endl;
  int s=readSuperblock(f, superblock_loc, super);
  cout <<hex<<"Absolute Sector Number: "<< b.Partition[0].abssector<< endl <<"Number sector: "<<  b.Partition[0].numsector<< endl;
  cout << hex<<"Superblock Magic Number: "<< super.s_magic<< endl;
  cout << hex << "S log size :"<< super.s_log_block_size<< endl;



}
