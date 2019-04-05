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
       int superblock_loc = b.Partition[0].abssector* 512 + 1024;
       cout <<dec << "loc: "<< superblock_loc<< endl;
       int s=readSuperblock(f, superblock_loc, super);
       cout <<hex<<"abs "<< b.Partition[0].abssector<< endl <<"Num sector " <<  b.Partition[0].numsector<< endl;
      cout << hex<<super.s_magic<< endl;


      unsigned int groupCount = (super.s_blocks_count - super.s_first_data_block) / super.s_blocks_per_group;



      unsigned int remainder = (super.s_blocks_count - super.s_first_data_block) % super.s_blocks_per_group;

      if(remainder > 0){
        groupCount++;
      }

      cout << "group count: "<< groupCount << endl;

      unsigned int blockSize = 1024 << super.s_log_block_size;

      cout <<dec << "Block size :" << blockSize + 1024 << endl;

      cout << sizeof(group_descriptor) * groupCount << endl;

      group_descriptor groupDescriptor[groupCount];
      int groupdes_loc = superblock_loc+1024;
      cout << dec << "group des :"<< groupdes_loc<< endl;
      int gb = readGroupDescriptor(f,groupdes_loc, groupDescriptor, groupCount);



      cout << groupDescriptor[0].block_bitmap<< endl;
      cout << groupDescriptor[0].inode_bitmap << endl;
      cout << groupDescriptor[0].inode_table << endl;



     }
