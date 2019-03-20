#include "vdi.h"
#include "vdifile.h"
#include "readVdi.h"
#include "mbr.h"
#include "superBlock.h"

#include <iostream>


 using namespace std;

  int main(int  argc,  char* argv[]){

    int vfile;
   VDIFile* f =new VDIFile();
   vfile=vdifileopen(f,argv[1]);
   BootRecord b;
   ssize_t num;
   num = read(f,&(f->header),sizeof(f->header));
   int headerMap[f->header.blocks];
   int h;

   h=readHeader(f,headerMap);
   int d=f->header.imgsignature;
cout << hex<< d<< endl;
   cout <<f->header.imgsignature<< endl;

   int mb= readMBR(f,b);
   Superblock super;
   int location= b.Partition[0].abssector*512+1024;
   cout << location<<endl;
   int s=readSuperblock(f,super, location);
   cout << b.Partition[0].abssector<< endl << b.Partition[0].numsector<< endl<< super.s_inode_size<< endl;
   cout <<super.s_inodes_count<<endl << super.s_magic << endl;



 }
