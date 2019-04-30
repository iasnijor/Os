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
        // printSuperBlock(super);

        // print group descriptor table
        // printBGDT(groupDescriptor,groupCount);


        uint8_t* buf = new uint8_t[blockSize];
      //  uint8_t* fBlock = fetchBlock(f,260,buf,filesystemstart,blockSize,blocknumbers);
      /*  for (int k = 0; k< blockSize; k++)
              {
                cout << (int)fBlock[k]<< "  ";
                printf("%x\n",fBlock[k]);
              }*/

        vector<int> blocknumbers;
        vector<int> inodesnumbers;
        Inode i= fetchInode(f,2,super,groupDescriptor,blockSize,filesystemstart,blocknumbers);
        uint8_t* buf2 = new uint8_t[blockSize];
        uint8_t *check=fetchBlock(f, i.i_block[0],buf2,filesystemstart,blockSize,blocknumbers);
      //  if(S_ISDIR(i.i_mode)){cout<< "yes it is dir"<< endl;}
      std::vector<string> directories;
      std::vector<string> files;
      int totalnumbers=0;
      for (int j=0; j<15;j++){
          if(i.i_block[j]!=0){
          uint8_t* buf3 = new uint8_t[blockSize];
          fetchBlockfromFile(f,&i,j,buf3,blockSize,filesystemstart,blocknumbers);
           totalnumbers=readDir(f,super,groupDescriptor,filesystemstart,blockSize,i.i_size,buf3,inodesnumbers, directories,files,blocknumbers);
    }}
      int totalBlocks=blockSize*groupCount;
       int inodeBitmap[254*groupCount];

      //INode bitmap
      int size=0;
      for (int i=0; i < groupCount;i++){
        uint8_t* bit = new uint8_t[blockSize];
        uint8_t* fBlock1 = fetchBlock(f,groupDescriptor[i].inode_bitmap,bit,filesystemstart,blockSize,blocknumbers);
        for (int j= 0; j <254; j++)
        {
        int val=(int)bit[j];
        if((int)bit[j]>0){size++;}
        //string val= bitset<8>(bit[j]).to_string();
         inodeBitmap[i*254+j]=val;
        }
      }
      string blockBitmaps[totalBlocks];

      for (int i=0; i < groupCount;i++){
        uint8_t* bit = new uint8_t[blockSize];
        uint8_t* fBlock1 = fetchBlock(f,groupDescriptor[i].block_bitmap,bit,filesystemstart,blockSize,blocknumbers);
        for (int j= 0; j <blockSize; j++)
        {

        string val=bitset<8>(bit[j]).to_string();
          blockBitmaps[i*blockSize+j]=val;
        }
      }
    //    printbitmaps(inodeBitmap,totalBlocks);
        int bitmapinode[2032*groupCount];
        for (int i =0; i < 254*groupCount;i++){
          int val = inodeBitmap[i];
              int *bits = new int [sizeof(int) *8];
              for(int k=0; k<8; k++){
              int mask =  1 << k;
              int masked_n = val & mask;
              int thebit = masked_n >> k;
              bitmapinode[i*8+(7-k)] = thebit;
            }
                }
      /*   for (int i=0;i<2032*16;i++){
           if (i%8==0  )cout << " ";
                                    if (i%2032==0  )cout << " "<< endl;
                      cout <<dec<< bitmapinode[i]<<"";

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

        cout << "checks    "<< endl;
        cout << "checks    "<< endl;
        cout << "checks    "<< endl;
        int checkinodebitmap[totalBlocks];
        for (int i = 0 ; i <2032*16;i++){checkinodebitmap[i]=0;}
       for (int i = 1 ; i <11;i++){inodesnumbers.push_back(i);}
        for (int i = 0 ; i <inodesnumbers.size();i++){
        unsigned int inodeNum=inodesnumbers.at(i);
        inodeNum--;
        unsigned int blockGroup=inodeNum/super.s_inodes_per_group;
        unsigned int index=inodeNum%super.s_inodes_per_group;
        unsigned int byteOffset=index/8;
        unsigned int bit=index%8;
        //cout << blockGroup<<" "<<byteOffset<< " "<<bit<<" "<< inodeNum<<" "<< " ";
        int inodeindex= blockGroup*2032+byteOffset*8+(7-bit);
        //cout << inodeindex<< endl;
        checkinodebitmap[inodeindex]=1;

      //  cout <<checkinodebitmap[inodeindex]<< endl;

        //bit=1<<bit;
        }
        sort(inodesnumbers.begin(), inodesnumbers.end());
      //  printinodeNumber(inodesnumbers);
    /*    for (int i=0;i<2032*16;i++){
          if (i%8==0  )cout << " ";
                                   if (i%2032==0  )cout << " "<< endl;
                     cout << checkinodebitmap[i]<<"";

               }*/

       printinodeNumber(blocknumbers);
        cout << "size "<<blocknumbers.size()<< " "<< super.s_blocks_count-super.s_free_blocks_count<< endl;
        //  printbitmaps(blockBitmaps,totalBlocks);

    //  printbitmaps(inodeBitmap,totalBlocks);
  //    printbitmaps(checkinodebitmap,totalBlocks);
      //  printbitmaps2(checkinodebitmap,inodeBitmap,totalBlocks);

        //Comparing and Correcting Superblock;
        for (int i = 0 ; i < groupCount;i++){
          if (power357(i)|| i==0){
            Superblock s1;
            fetchBlock(f,super.s_blocks_per_group*i+1,(uint8_t*)&s1,filesystemstart,blockSize,blocknumbers);
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

        // check superblock
        for (int i = 0 ; i < groupCount;i++){
          if (power357(i)|| i==0){
            Superblock s1;
            fetchBlock(f,super.s_blocks_per_group*i+1,(uint8_t*)&s1,filesystemstart,blockSize,blocknumbers);
            compareSuperblock(super,s1);
          }
        }


}
