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
#include<vector>
#include<array>
#include<bitset>
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
        uint8_t* fBlock = fetchBlock(f,260,buf,filesystemstart,blockSize);
      /*  for (int k = 0; k< blockSize; k++)
              {
                cout << (int)fBlock[k]<< "  ";
                printf("%x\n",fBlock[k]);
              }*/

        vector<int> inodesnumbers;
        Inode i= fetchInode(f,2,super,groupDescriptor,blockSize,filesystemstart);
        uint8_t* buf2 = new uint8_t[blockSize];
        uint8_t *check=fetchBlock(f, i.i_block[0],buf2,filesystemstart,blockSize);
      //  if(S_ISDIR(i.i_mode)){cout<< "yes it is dir"<< endl;}
      std::vector<string> directories;
      std::vector<string> files;
      int totalnumbers=0;
      for (int j=0; j<15;j++){
          if(i.i_block[j]!=0){
          uint8_t* buf3 = new uint8_t[blockSize];
          fetchBlockfromFile(f,&i,j,buf3,blockSize,filesystemstart);
           totalnumbers=readDir(f,super,groupDescriptor,filesystemstart,blockSize,i.i_size,buf3,inodesnumbers, directories,files);
    }}
      int totalBlocks=blockSize*groupCount;
       string  inodeBitmap[totalBlocks];

      //INode bitmap
      int size=0;
      for (int i=0; i < groupCount;i++){
        uint8_t* bit = new uint8_t[blockSize];
        uint8_t* fBlock1 = fetchBlock(f,groupDescriptor[i].inode_bitmap,bit,filesystemstart,blockSize);
        for (int j= 0; j <blockSize; j++)
        {
          if (j%1024==0)cout << " "<< endl;
          cout << (int) bit[j]<< " ";
          if((int)bit[j]>0){size++;}
         string val= bitset<8>(bit[j]).to_string();
          inodeBitmap[i*blockSize+j]=val;
        }
      }
      string blockBitmaps[totalBlocks];

      for (int i=0; i < groupCount;i++){
        uint8_t* bit = new uint8_t[blockSize];
        uint8_t* fBlock1 = fetchBlock(f,groupDescriptor[i].block_bitmap,bit,filesystemstart,blockSize);
        for (int j= 0; j <blockSize; j++)
        {

        string val=bitset<8>(bit[j]).to_string();
          blockBitmaps[i*blockSize+j]=val;
        }
      }

    /*  for (int i = 0 ;i <totalBlocks;i++){cout << blockBitmaps[i]<<" ";
    if (i%1024==0)cout << ""<<endl;}*/


  //     cout << "Directories" << endl;
    //  for(int i =0;i<directories.size();i++){cout <<"a"<< directories.at(i)<<endl;}
      //  cout << "Files"<< endl;
        //for(int i =0;i<files.size();i++){cout << files.at(i)<<endl;}
       //printinodeNumber(inodesnumbers);
       //cout << inodesnumbers.size()<<"equals "<< totalnumbers<<super.s_inodes_count-super.s_free_inodes_count<< endl;
              /*   for (int k = 0; k< 200; k++)
                    {
                      cout << (char)buf9[k]<< "  ";
                      printf("%x\n", buf9[k]);
                    }*/


        string checkinodebitmap[totalBlocks];
        for (int i = 0 ; i <totalBlocks;i++){checkinodebitmap[i]="00000000";}
        for (int i = 0 ; i <inodesnumbers.size();i++){
        unsigned int inodeNum=inodesnumbers.at(i);
        inodeNum--;
        unsigned int blockGroup=inodeNum/super.s_inodes_per_group;
        unsigned int index=inodeNum%super.s_inodes_per_group;
        unsigned int byteOffset=index/8;
        unsigned int bit=index%8;
    //    cout << blockGroup<<" "<<byteOffset<< " "<<bit<<" "<< inodeNum<<" ";
        int inodeindex= blockGroup*blockSize+byteOffset;
        checkinodebitmap[inodeindex][bit]='1';
      //  cout <<checkinodebitmap[inodeindex]<< endl;

        //bit=1<<bit;
        }


    //  printbitmaps(inodeBitmap,totalBlocks);
    //  printbitmaps(checkinodebitmap,totalBlocks);
      //  printbitmaps2(checkinodebitmap,inodeBitmap,totalBlocks);

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
        cout <<"SIZEEEE"<< size << endl;
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



}
