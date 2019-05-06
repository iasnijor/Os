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

          unsigned int blockSize = 1024 << super.s_log_block_size;
          group_descriptor groupDescriptor[groupCount];
          int groupdes_loc = superblock_loc+blockSize;
          int gb = readGroupDescriptor(f,groupdes_loc, groupDescriptor, groupCount);



          uint8_t* buf3 = new uint8_t[blockSize];
          uint8_t* buf4 = new uint8_t[blockSize];

          vector<int> blocknumbers;
          vector<int> inodesnumbers;
          std::vector<string> directories;
          std::vector<string> files;
          directories.push_back("root");
          int totalnumbers=0;
        for (int k = 1; k<super.s_first_ino;k++){
          Inode i= fetchInode(f,k,super,groupDescriptor,blockSize,filesystemstart,blocknumbers);
         for (int j = 0 ; j*blockSize <i.i_size;j++){
                    fetchBlockfromFile(f,&i,j,buf3,blockSize,filesystemstart,blocknumbers);
                    if (k==2){
                    readDir(f,super,groupDescriptor,filesystemstart,blockSize,i.i_size,buf3,inodesnumbers, directories,files,blocknumbers);}
              //   fetchBlock(f,i.i_block[j],buf4,filesystemstart,blockSize,blocknumbers);

                }
                }

        uint8_t*   fBlock2 = fetchBlock(f,1,buf3,filesystemstart,blockSize,blocknumbers);
                  //Fetching Inode table
                uint8_t* inodetable = new uint8_t[blockSize];
                  for (int j=0; j < groupCount;j++){

                    for (unsigned int k=0; k<super.s_inodes_per_group/8 ; k++){

                    fetchBlock(f,k+groupDescriptor[j].inode_table,inodetable,filesystemstart,blockSize,blocknumbers);

                    }
                  }
                  delete(inodetable);

                //Comparing and Correcting Superblock;
                for (int i = 0 ; i < groupCount;i++){
                if (power357(i)|| i==0||i==1){
                  Superblock s1;
                  fetchBlock(f,super.s_blocks_per_group*i+1,(uint8_t*)&s1,filesystemstart,blockSize,blocknumbers);
                  if(!compareSuperblock(super,s1)){
                    VDISeek(f,super.s_blocks_per_group*i+1,SEEK_SET);
                }
                }
                }

                //Comparing and Correcting group_descriptor;
                group_descriptor g[groupCount];
                for (int i = 0 ; i < groupCount;i++){
                if (power357(i)|| i==0||i==1){
                  int locationGroup= (super.s_blocks_per_group*i+2)*blockSize+filesystemstart;
                  uint8_t* fBlock1 = fetchBlock(f,super.s_blocks_per_group*i+2,buf3,filesystemstart,blockSize,blocknumbers);
                  int geb = readGroupDescriptor(f,locationGroup, g, groupCount);
                  compareGroupDes(groupDescriptor,g,groupCount);

                }
              }

        //    uint8_t* fBlock1 = fetchBlock(f,130048,buf3,filesystemstart,blockSize,blocknumbers);

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


          //Processing block blockBitmaps
          uint8_t* bit = new uint8_t[blockSize];
          int blockBitmaps[blockSize*groupCount];
            for (int i=0; i < groupCount;i++){
          uint8_t* fBlock1 = fetchBlock(f,groupDescriptor[i].block_bitmap,bit,filesystemstart,blockSize,blocknumbers);
          int limit=blockSize;
          if(i==15){limit=super.s_blocks_count% super.s_blocks_per_group/8;}
          for (int j= 0; j <limit; j++)
          {
            int val=(int)bit[j];
            blockBitmaps[i*blockSize+j]=val;
          }
          }

          int totalblocks=super.s_blocks_per_group*(groupCount-1)+(super.s_blocks_count% super.s_blocks_per_group);
          int totalblockbytes=blockSize*(groupCount-1)+super.s_blocks_count% super.s_blocks_per_group/8;
          int bitmapblock[totalblocks];
          for (int i =0; i <totalblockbytes;i++){
          int val = blockBitmaps[i];
          int index=8;
          if (i==totalblockbytes-1){index=7;  bitmapblock[i*8+7] =0;}
          for(int k=0; k<index; k++){
            int mask =  1 << k;
            int masked_n = val & mask;
            int thebit = masked_n >> k;
            bitmapblock[i*8+(7-k)] = thebit;
          }
          }

          int check=0;
          for (int i =0;i< totalblocks;i++){
            if (bitmapblock[i]==1)check++;
          }

          sort( blocknumbers.begin(),blocknumbers.end() );
          blocknumbers.erase( unique( blocknumbers.begin(), blocknumbers.end() ), blocknumbers.end() );
          int checkblockbitmap[totalblocks];
          for (int i = 0 ; i <totalblocks;i++){checkblockbitmap[i]=0;}
          for (int i = 1 ; i <11;i++){inodesnumbers.push_back(i);}
          for (int i = 0 ; i <blocknumbers.size();i++){
            unsigned int blockNum=blocknumbers.at(i)-super.s_first_data_block;
            unsigned int blockGroup=blockNum/super.s_blocks_per_group;
            unsigned int index=blockNum%super.s_blocks_per_group;
            unsigned int byteOffset=index/8;
            unsigned int bit=index%8;
            int blockindex= blockGroup*super.s_blocks_per_group+byteOffset*8+(7-bit);
            checkblockbitmap[blockindex]=1;
          }



        // print the super block
        printf("############# SUPERBLOCK #############\n");
        printSuperBlock(super);

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





        // number of directories
         int ndirec = 0;
        for(int i=0;i<groupCount;i++){ndirec+=groupDescriptor[i].used_dirs_count;}
        printf("Number of  directories : %d\n\n", ndirec );

        // number of files

        unsigned int fileNodes = super.s_inodes_count - super.s_free_inodes_count;
        // - 9 because first 11 (not inode 2, 11) are not file or dir
        fileNodes = (fileNodes - ndirec) - 9;



        printf("Number of files: %d\n\n", fileNodes );

        // number of blockGroups && groupDescriptor
        cout << dec << "Number of block groups: " << groupCount << endl << endl;

        printf("****************** GROUP DESCRIPTOR TABLLE *********************\n");
        printBGDT(groupDescriptor,groupCount);


        //Block Size
        printf("\nBlock Size: %d bytes\n\n", blockSize );
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

        int inodematch=0;
        for (int i=0;i<super.s_inodes_per_group*groupCount;i++){
                    if (bitmapinode[i]==checkinodebitmap[i]){inodematch++;}

        }
        if (inodematch==32512)
        cout << "InodeBitmaps matched with InodeBitmpas reported by Group Descriptor"<<endl << endl;

        int blockmatch=0;
         for (int i=0;i<totalblocks;i++){
                     if (bitmapblock[i]==checkblockbitmap[i]){blockmatch++;}

         }
         if(blockmatch==totalblocks){
         cout << "ALL Block Bitmaps  are macthed with the bitmap reported by group descriptor bitmaps."<< endl<<endl;}

         // dirs found
         int ndirectories=directories.size();
         printf("Number of  directories found: %d\n\n", ndirectories);

         // files found
         int nfiles=files.size();
         printf("Number of files found: %d\n\n", nfiles);
}
