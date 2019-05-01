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
#include <bits/stdc++.h>
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

          //Reading Group Descriptor Table
          unsigned int groupCount = (super.s_blocks_count - super.s_first_data_block) / super.s_blocks_per_group;
          unsigned int remainder = (super.s_blocks_count - super.s_first_data_block) % super.s_blocks_per_group;
          if(remainder > 0){
            groupCount++;
          }
          cout << "Group count: "<<dec << groupCount << endl;
          unsigned int blockSize = 1024 << super.s_log_block_size;
          group_descriptor groupDescriptor[groupCount];
          int groupdes_loc = superblock_loc+blockSize;
          int gb = readGroupDescriptor(f,groupdes_loc, groupDescriptor, groupCount);



          uint8_t* buf3 = new uint8_t[blockSize];
          vector<int> blocknumbers;
          vector<int> inodesnumbers;
          for (int j = 1; j<11;j++){
          Inode i= fetchInode(f,j,super,groupDescriptor,blockSize,filesystemstart,blocknumbers);
          std::vector<string> directories;
          std::vector<string> files;
          int totalnumbers=0;
         for (int j = 0 ; j*blockSize <i.i_size;j++){
                    fetchBlockfromFile(f,&i,j,buf3,blockSize,filesystemstart,blocknumbers);
                    totalnumbers=readDir(f,super,groupDescriptor,filesystemstart,blockSize,i.i_size,buf3,inodesnumbers, directories,files,blocknumbers);
                }
              }
        uint8_t* fBlock2 = fetchBlock(f,0,buf3,filesystemstart,blockSize,blocknumbers);
        fBlock2 = fetchBlock(f,1,buf3,filesystemstart,blockSize,blocknumbers);

            /*   Inode i2= fetchInode(f,7,super,groupDescriptor,blockSize,filesystemstart,blocknumbers);
                for (int j = 0 ; j*blockSize <i.i_size;j++){
                           fetchBlockfromFile(f,&i2,j,buf3,blockSize,filesystemstart,blocknumbers);
                           totalnumbers=readDir(f,super,groupDescriptor,filesystemstart,blockSize,i.i_size,buf3,inodesnumbers, directories,files,blocknumbers);

                       }*/

                //Comparing and Correcting Superblock;
                for (int i = 0 ; i < groupCount;i++){
                if (power357(i)|| i==0){
                  Superblock s1;
                  fetchBlock(f,super.s_blocks_per_group*i+1,(uint8_t*)&s1,filesystemstart,blockSize,blocknumbers);
                  compareSuperblock(super,s1);
                }
                }

                //Comparing and Correcting group_descriptor;
                group_descriptor g[groupCount];
                for (int i = 0 ; i < groupCount;i++){
                if (power357(i)|| i==0){
                  int locationGroup= (super.s_blocks_per_group*i+2)*blockSize+filesystemstart;
                  uint8_t* fBlock1 = fetchBlock(f,super.s_blocks_per_group*i+2,buf3,filesystemstart,blockSize,blocknumbers);
                  int geb = readGroupDescriptor(f,locationGroup, g, groupCount);
                  compareGroupDes(groupDescriptor,g,groupCount);

                }
              }

            //INode bitmap reading and processing and checks

            int inodeBitmap[super.s_inodes_per_group/8*groupCount];
            int size=0;
            for (int i=0; i < groupCount;i++){
                  uint8_t* bit = new uint8_t[blockSize];
                  uint8_t* fBlock1 = fetchBlock(f,groupDescriptor[i].inode_bitmap,bit,filesystemstart,blockSize,blocknumbers);
                    for (int j= 0; j <super.s_inodes_per_group/8; j++)
                      {
                        int val=(int)bit[j];
                        if((int)bit[j]>0){size++;}
                        inodeBitmap[i*super.s_inodes_per_group/8+j]=val;
                      }
                }
            int bitmapinode[super.s_inodes_per_group*groupCount];
            for (int i =0; i < super.s_inodes_per_group/8*groupCount;i++){
              int val = inodeBitmap[i];
              int *bits = new int [sizeof(int) *8];
              for(int k=0; k<8; k++){
                int mask =  1 << k;
                int masked_n = val & mask;
                int thebit = masked_n >> k;
                bitmapinode[i*8+(7-k)] = thebit;
              }
            }
          int checkinodebitmap[super.s_inodes_per_group*groupCount];
          for (int i = 0 ; i <super.s_inodes_per_group*groupCount;i++){checkinodebitmap[i]=0;}
          for (int i = 1 ; i <11;i++){inodesnumbers.push_back(i);}
          for (int i = 0 ; i <inodesnumbers.size();i++){
            unsigned int inodeNum=inodesnumbers.at(i);
            inodeNum--;
            unsigned int blockGroup=inodeNum/super.s_inodes_per_group;
            unsigned int index=inodeNum%super.s_inodes_per_group;
            unsigned int byteOffset=index/8;
            unsigned int bit=index%8;
            int inodeindex= blockGroup*super.s_inodes_per_group+byteOffset*8+(7-bit);
            checkinodebitmap[inodeindex]=1;
          }
          int inodematch=0;
          for (int i=0;i<super.s_inodes_per_group*groupCount;i++){
                      if (bitmapinode[i]==checkinodebitmap[i]){inodematch++;}

          }
          if (inodematch==32512)
          cout << "InodeBitmaps matched with InodeBitmpas reported by Group Descriptor"<<endl;
          //Fetching Inode table
          uint8_t* ibit = new uint8_t[blockSize];
          for (int i=0; i < groupCount;i++){
                uint8_t* fBlock1 = fetchBlock(f,groupDescriptor[i].inode_table,ibit,filesystemstart,blockSize,blocknumbers);
      }

          //Processing block blockBitmaps
          int bnum=0;
          uint8_t* bit = new uint8_t[blockSize];
          int blockBitmaps[blockSize*groupCount];
          for (int i=0; i < groupCount;i++){
          uint8_t* fBlock1 = fetchBlock(f,groupDescriptor[i].block_bitmap,bit,filesystemstart,blockSize,blocknumbers);
          for (int j= 0; j <blockSize; j++)
          {
            int val=(int)bit[j];
            if (val>0)bnum++;
            blockBitmaps[i*blockSize+j]=val;
          }
          }
        //  cout << "NOof blocks "<< bnum << endl;
          int bitmapblock[super.s_blocks_per_group*groupCount];
          for (int i =0; i < blockSize*groupCount;i++){
          int val = blockBitmaps[i];
          int *bits = new int [sizeof(int) *8];
          for(int k=0; k<8; k++){
            int mask =  1 << k;
            int masked_n = val & mask;
            int thebit = masked_n >> k;
            bitmapblock[i*8+(7-k)] = thebit;
          }
          }
          int check=0;
          for (int i =0;i< super.s_blocks_per_group*groupCount;i++){
            if (bitmapblock[i]==1)check++;
          }
          sort( blocknumbers.begin(),blocknumbers.end() );
          blocknumbers.erase( unique( blocknumbers.begin(), blocknumbers.end() ), blocknumbers.end() );
        int checkblockbitmap[blockSize*groupCount];
          for (int i = 0 ; i <super.s_inodes_per_group*groupCount;i++){checkinodebitmap[i]=0;}
        //  for (int i = 1 ; i <11;i++){inodesnumbers.push_back(i);}
          for (int i = 0 ; i <blocknumbers.size();i++){
            unsigned int blockNum=blocknumbers.at(i);
              unsigned int blockGroup=blockNum/super.s_blocks_per_group;
            unsigned int index=blockNum%super.s_blocks_per_group;
            unsigned int byteOffset=index/8;
            unsigned int bit=index%8;
            int blockindex= blockGroup*super.s_blocks_per_group+byteOffset*8+(7-bit);
            checkblockbitmap[blockindex]=1;
          }
      /*  for (int i=0;i<8192*16;i++){
          if (i%8==0  )cout << " ";
          if (i%1024==0  )cout << " "<< endl;
          if (i%8192==0){cout << "group starts "<< endl;}
          cout <<dec<< checkblockbitmap[i]<<"";
        }*/

        int blockmatch=0;
        for (int i=0;i<super.s_blocks_per_group*groupCount;i++){
                    if (bitmapblock[i]==checkblockbitmap[i]){blockmatch++;}

        }
        uint8_t* fBlock1 = fetchBlock(f,groupDescriptor[0].block_bitmap,bit,filesystemstart,blockSize,blocknumbers);

        cout << "Number of bitmapblock matched: "<< blockmatch<< " "<< check<< endl;


        cout <<"SIZEEEE"<< size << endl;
        if (inodematch==32512)
        cout << "InodeBitmaps matched with InodeBitmpas reported by Group Descriptor"<<endl;

        // print the super block
        printf("############# SUPERBLOCK #############\n");
        printSuperBlock(super);

        // print group descriptor table
        printf("##################### GROUP DESCRIPTOR TABLLE ###################\n");
        printBGDT(groupDescriptor,groupCount);

        //STATS
        printf("\n##################### GENERAL STATISTICS ###################\n");

        // calculate total filesystem size
        unsigned int fsSize = super.s_blocks_count * blockSize;
        printf("Total size of Filesystem: %u bytes\n\n", fsSize);


        // calculate free space
        unsigned int fSpace = super.s_free_blocks_count * blockSize;
        printf("Size available for files: %u bytes\n\n", fSpace);

        // calculate used space
        unsigned int uSpace = blockSize * (super.s_blocks_count - super.s_free_blocks_count);
        printf("Used Space: %u bytes\n\n", uSpace );

        // possible files and directories
        printf("Number of possible files and directories: %d\n\n", super.s_inodes_count );

        // number of files

        // number of directories

        //Block Size
        printf("Block Size: %d bytes\n\n", blockSize );
        //State of file system
        printf("State of the Filesystem: ");
        if (super.s_state==1){printf("CLEAN\n\n" );}
        else if (super.s_state==2){printf("File has an error\n\n" );}
        else printf("error reading super\n\n" );

        printf("############### CHECKING FILESYSTEM #################\n\n");
        //checking magic number
        if(super.s_magic != 61267){
          printf("INCORRECT MAGIC NUMBER\n" );
        }
        else {
          cout << hex << "Magic number is correct: " << super.s_magic << endl << endl;;
        }

          cout << "Block per group "<<dec << super.s_blocks_per_group<< endl;
          cout << "Frags per group "<<dec << super.s_frags_per_group<< endl;
          cout << "Inodes Per Group "<< dec<< super.s_inodes_per_group << endl;
}
