#include "vdi.h"
#include "vdifile.h"
#include "readVdi.h"
#include "mbr.h"
#include "superBlock.h"
#include "dir.h"
#include <iostream>
#include <cstdint>
#include <math.h>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include<sys/stat.h>
#include <cstdio>
#include<list>
#include<array>
using namespace std;

int main(int  argc,  char* argv[]){
        // Opening a VDIFile
        int vfile;
        VDIFile* f =new VDIFile();
        vfile=vdifileopen(f,argv[1]);

        //Reading Header
        off_t offset;
        offset = VDISeek(f,0,SEEK_SET);
        int head=VDIread(f,&(f->header), sizeof(f->header));



        // debug code to make sure we getting the header magic number right
        //cout << hex<< "Header Magic Number:"<<f->header.imgsignature <<endl;

        // Reading MBR
        BootRecord b;
        int mb= readMBR(f,b);

        // Reading Superblock
        Superblock super;

        int superblock_loc = f->header.offsetdata+b.Partition[0].abssector* 512 + 1024;
        int filesystemstart=superblock_loc-1024;
        int s=readSuperblock(f, superblock_loc, super);

        // Debug code to make sure we are reading the superBlock
        cout <<hex<<"Absolute Sector Number "<< b.Partition[0].abssector<< endl <<"Number sector " <<  b.Partition[0].numsector<< endl;
        if (super.s_state==1){cout << "FIle is clean"<< endl;}
        else if (super.s_state==2){cout << "FIle has error"<< endl;}
        else cout << "Error reading Superblock"<< endl;

        //Reading Group Descriptor Table
        unsigned int groupCount = (super.s_blocks_count - super.s_first_data_block) / super.s_blocks_per_group;
        unsigned int remainder = (super.s_blocks_count - super.s_first_data_block) % super.s_blocks_per_group;
        if(remainder > 0){
          groupCount++;
        }
        cout << "Group count: "<< groupCount << endl;
        unsigned int blockSize = 1024 << super.s_log_block_size;
        group_descriptor groupDescriptor[groupCount];
        int groupdes_loc = superblock_loc+blockSize;
        int gb = readGroupDescriptor(f,groupdes_loc, groupDescriptor, groupCount);

        // print super block
        printSuperBlock(super);

        // print group descriptor table
        printBGDT(groupDescriptor,groupCount);


        uint8_t* buf = new uint8_t[blockSize];
        uint8_t* fBlock = fetchBlock(f, 259,buf,filesystemstart,blockSize);

        Inode i= fetchInode(f,2,super,groupDescriptor,blockSize,filesystemstart);
      //  Inode i2= fetchInode(f,4,super,groupDescriptor,blockSize,filesystemstart);
        uint8_t* buf2 = new uint8_t[blockSize];
            //   cout <<"Iblock number for Inode 2  " << i2.i_block[0]<<" mode "<< i2.i_mode <<" "<< i2.i_file_acl<< endl;
        uint8_t *check=fetchBlock(f, i.i_block[0],buf2,filesystemstart,blockSize);
      //  if(S_ISDIR(i.i_mode)){cout<< "yes it is dir"<< endl;}
        uint8_t* buf3 = new uint8_t[blockSize];
       fetchBlockfromFile(f,&i,0,buf3,blockSize,filesystemstart);
       int a[4];
      readDir(i.i_size,buf3,a);
         int totalBlocks=blockSize*groupCount;
         unsigned int allBitmap[blockSize*groupCount];
      //printing the buffer

      //INode bitmap
      for (int i=0; i < groupCount;i++){
        uint8_t* bit = new uint8_t[blockSize];
        uint8_t* fBlock1 = fetchBlock(f,groupDescriptor[i].inode_bitmap,bit,filesystemstart,blockSize);
        for (int j= 0; j <blockSize; j++)
        {
          int val= bin(bit[j]);
        //  cout <<dec << i*blockSize+j<< endl;
          allBitmap[i*blockSize+j]=val;
        }
      }
      int blockBitmaps[totalBlocks];
      for (int i=0; i < groupCount;i++){
        uint8_t* bit = new uint8_t[blockSize];
        uint8_t* fBlock1 = fetchBlock(f,groupDescriptor[i].block_bitmap,bit,filesystemstart,blockSize);
        for (int j= 0; j <blockSize; j++)
        {
          int val= bin(bit[j]);
        //  cout <<dec << i*blockSize+j<< endl;
          blockBitmaps[i*blockSize+j]=val;
        }
      }
      for (int i = 0; i < totalBlocks; i++){
      cout << blockBitmaps[i]<<allBitmap[i]<< " ";
      if (i%1024==0)cout <<" "<<endl;
    }

  /*    bool usedNotused[totalBlocks];
    for (int i=0;i < blockSize*groupCount;i++ ){
      if( allBitmap[i]==blockBitmaps[i])cout << "match "<< endl;
      else cout << "Error"<< endl;
      }
*/


    /*    for (int j=0; j< sizeof(a)/sizeof(*a);j++){
          Inode i= fetchInode(f,a[j],super,groupDescriptor,blockSize,filesystemstart);
          for (int k = 0 ; k<15;k++){
          uint8_t* buf8 = new uint8_t[blockSize];
          if(i.i_block[j]!=0){
          fetchBlockfromFile(f,&i,i.i_block[0],buf8,blockSize,filesystemstart);
          int c = readDir(i.i_size,buf8,a);
        }
        }
      }*/

       //Comparing and Correcting Superblock;
        for (int i = 0 ; i < groupCount;i++){
          if (power357(i)|| i==0){
            Superblock s1;
            fetchBlock(f,super.s_blocks_per_group*i+1,(uint8_t*)&s1,filesystemstart,blockSize);
            compareSuperblock(super,s1);
          }
        }

        //Comparing and Correcting group_descriptor;
      /*  group_descriptor g[groupCount];
        for (int i = 0 ; i < groupCount;i++){
          if (power357(i)|| i==0){
            int locationGroup= (super.s_blocks_per_group*i+2)*blockSize+filesystemstart;
            int geb = readGroupDescriptor(f,locationGroup, g, groupCount);
            compareGroupDes(groupDescriptor,g,groupCount);

          }
        }*/

        // calculate total filesystem size
        unsigned int fsSize = super.s_blocks_count * blockSize;
        printf("Total size of Filesystem: %u bytes\n", fsSize);

        // calculate free space
        unsigned int fSpace = super.s_free_blocks_count * blockSize;
        printf("Size available for files: %u bytes\n", fSpace);

        // calculate used used space
        unsigned int uSpace = blockSize * (super.s_blocks_count - super.s_free_blocks_count);
        printf("Used Space: %u bytes\n", uSpace );

        //Block Size
        cout << "Block Size:"<<dec<< blockSize<< " bytes"<< endl;
        //State of file system
        cout << "State of Filesystem: ";
        if (super.s_state==1){cout << "Clean"<< endl;}
        else if (super.s_state==2){cout << "FIle has error"<< endl;}
        else cout << "Error reading Superblock"<< endl;

        for (int i = 0 ; i<15; i++){

        }


}
