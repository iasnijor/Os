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
        // Opening a VDIFile
        int vfile;
        VDIFile* f =new VDIFile();
        vfile=vdifileopen(f,argv[1]);

        //Reading Header
        off_t offset;
        offset = VDISeek(f,0,SEEK_SET);
        int head=VDIread(f,&(f->header), sizeof(f->header));

        // debug code to make sure we getting the header magic number right
        cout << hex<< "Header Magic Number:"<<f->header.imgsignature <<endl;


        /*   int map;
        int vdiMap[f->header.blocks];
        map= readMap(f,vdiMap);*/

        // Reading MBR
        BootRecord b;
        int mb= readMBR(f,b);

        // Reading Superblock
        Superblock super;

        int superblock_loc = b.Partition[0].abssector* 512 + 1024;
        int filesystemstart=superblock_loc-1024;
        int s=readSuperblock(f, superblock_loc, super);
        // Debug code to make sure we are reading the superBlock
        cout <<hex<<"Absolute Sector Number "<< b.Partition[0].abssector<< endl <<"Number sector " <<  b.Partition[0].numsector<< endl;
        cout << hex<<"Superblock Magic Number: "<<super.s_magic<< endl;

        //Reading Group Descriptor Table
        unsigned int groupCount = (super.s_blocks_count - super.s_first_data_block) / super.s_blocks_per_group;
        unsigned int remainder = (super.s_blocks_count - super.s_first_data_block) % super.s_blocks_per_group;
        if(remainder > 0){
          groupCount++;
        }
        cout << "Group count: "<< groupCount << endl;
        unsigned int blockSize = 1024 << super.s_log_block_size;
        group_descriptor groupDescriptor[groupCount];
        int groupdes_loc = superblock_loc+1024;

        int gb = readGroupDescriptor(f,groupdes_loc, groupDescriptor, groupCount);
        cout << dec << groupDescriptor[0].block_bitmap<< endl;
        cout << groupDescriptor[0].inode_bitmap << endl;
        cout << groupDescriptor[0].inode_table << endl;

        uint8_t* buf = new uint8_t[blockSize];


        uint8_t* fBlock = fetchBlock(f, 259,buf,filesystemstart,blockSize);
        //cout <<dec<< "file star"<<filesystemstart << endl;
        cout <<uint8_t(*fBlock+3) << endl <<uint8_t(*fBlock+4)<< endl <<uint8_t(*fBlock+5) << endl;
        Inode i= fetchInode(f,2,super,groupDescriptor,blockSize,filesystemstart);
          uint8_t* buf2 = new uint8_t[blockSize];
        unsigned char *check=fetchBlock(f, i.i_block[0],buf2,filesystemstart,blockSize);
        cout <<uint8_t(*check+1) << endl;
        // calculate total filesystem size
        unsigned int fsSize = super.s_blocks_count * blockSize;
        printf("Total size of Filesystem: %u bytes\n", fsSize);

        // calculate free space
        unsigned int fSpace = super.s_free_blocks_count * blockSize;
        printf("Size available for files: %u bytes\n", fSpace);

        // calculate used used space
        unsigned int uSpace = blockSize * (super.s_blocks_count - super.s_free_blocks_count);
        printf("Used Space: %u bytes\n", uSpace );

}
