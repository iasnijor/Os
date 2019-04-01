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

        int vfile;
       VDIFile* f =new VDIFile();
       vfile=vdifileopen(f,argv[1]);

          off_t offset;
         offset = VDISeek(f,0,SEEK_SET);
        int head=VDIread(f,&(f->header), sizeof(f->header));

          cout << hex<< "Magic Number:";
         cout <<f->header.imgsignature <<endl;


    /*   int map;
        int vdiMap[f->header.blocks];
       map= readMap(f,vdiMap);*/
         BootRecord b;



       int mb= readMBR(f,b);

       Superblock super;
       int finals = b.Partition[0].abssector* 512 + 1024;
       cout << "loc: "<< finals<< endl;
       int s=readSuperblock(f, finals, super);
       cout <<hex<<"abs "<< b.Partition[0].abssector<< endl <<"Num sector "<<  b.Partition[0].numsector<< endl;
      cout << hex<<super.s_magic<< endl;




     }
