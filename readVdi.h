#ifndef READVDI
#define READVDI
#include "vdifile.h"
#include "vdi.h"
#include "mbr.h"
#include "superBlock.h"
#include "groupDescriptor.h"
#include "inode.h"
#include <cstdint>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;

  int vdifileopen(VDIFile *v,  char* name){
    v->file = open(name,O_RDWR);
    return v->file;
  }

  void vdiFileClose(VDIFile *v){
    if (!v)
    return;
    close(v->file);
  }

  off_t VDISeek (VDIFile* f, off_t off, int whence){
off_t location;
        if (whence == SEEK_SET)
	{
		location = lseek(f->file, off, whence);
		if (location < 0)
		{
			cout << "Error seeking the vdi header" << endl;
			return 1;
		}
		f->cursor = location;
	}
	if (whence == SEEK_CUR)
	{
		location = lseek(f->file, off, whence);
		if (location < 0)
		{
			cout << "Error seeking the vdi header" << endl;
			return 1;
		}
		f->cursor += off;
	}
	if (whence == SEEK_END)
	{
		location = lseek(f->file, off, whence);
		if (location < 0)
		{
			cout << "Error seeking the vdi header" << endl;
			return 1;
		}
		f->cursor = off + f->filesize;
	}
	return f->cursor;


  }


  // int vdiMapRead(VDIFile* f, unsigned int vdiMap[]){
  //   off_t offset = lseek(f->file, f-> header.offsetblock, SEEK_SET);
  //   int read = read(f->file, vdiMap, 4 * (f->header.blocks));
  // }


  ssize_t VDIread(VDIFile *v,void *buff,ssize_t num){
    ssize_t nBytes= read(v->file,buff,num);
    if (num != nBytes){
      cout <<"Error";
      return 1;
    }
    return nBytes;
    v->cursor= v->header.offsetdata;
  }

  

  int readMBR(VDIFile *f, BootRecord&  b){
    off_t offset= VDISeek(f,f->header.offsetdata,SEEK_SET);
    if (offset< 0) cout << "Error"<<endl;
    int mbr = VDIread(f,&b,sizeof(b));
    return mbr;
  }

  int readSuperblock(VDIFile *f,int loc, Superblock& s){
    off_t offset= VDISeek(f,f->header.offsetdata+loc,SEEK_SET);
    if (offset<0) cout << "Error"<< endl;
    int super= VDIread(f,&s, sizeof(s));

  }

  int readGroupDescriptor(VDIFile *f,  unsigned int blockSize, group_descriptor groupDescriptor[], unsigned int groupCount){
      lseek(f->file,f->header.offsetdata+blockSize,SEEK_SET);
      read(f->file,groupDescriptor, sizeof(group_descriptor) * groupCount);


    }


    uint8_t* fetchBlock(VDIFile *f, unsigned int blockNum, uint8_t* buff, int location, unsigned int blockSize){

      //uint8_t* buff = new uint8_t[blockSize];

      int num = (blockNum * blockSize) + f->header.offsetdata+location;
      cout << "Num"<< num << endl;
      off_t offset=lseek(f->file, num , SEEK_SET);
      int block=read(f->file, buff, blockSize);

      return buff;


    }

   Inode fetchInode(VDIFile *f,int inodeNumber,Superblock super, group_descriptor group[],unsigned int blockSize,int filesystemstart){
   Inode inode;
   //uint8_t* buff = (uint8_t*)malloc(blockSize);
   uint8_t* buff = new uint8_t[blockSize];

   inodeNumber--;
   unsigned int blockgroup      = inodeNumber/super.s_inodes_per_group;
   unsigned int blockGroupNum   = inodeNumber % super.s_inodes_per_group;
   unsigned int inodesPerBlock  = blockSize/sizeof(Inode);
   unsigned int blockNum           = inodeNumber/inodesPerBlock;
   unsigned int inodeGroupNumber= inodeNumber% inodesPerBlock;
   fetchBlock(f,group[blockgroup].inode_table+blockNum,buff,filesystemstart+(inodeGroupNumber*sizeof(Inode)),blockSize);
   Inode  *ibuff =(Inode *)malloc(blockSize);
   ibuff=(Inode*) &buff;
   return ibuff[inodeGroupNumber];

 }

 void printSuperBlock(Superblock &super){
    printf("\nSuperblock from block group %i\n", super.s_block_group_nr);
    printf("Inodes count:          %15u\n"
           "Blocks count:          %15u\n"
           "Reserved blocks count: %15u\n"
           "Free blocks count:     %15u\n"
           "Free inodes count:     %15u\n"
           "State:                 %15u\n"
           "Block per group:       %15u\n"
           "Magic number:          %15x\n\n",
           super.s_inodes_count,
           super.s_blocks_count,
           super.s_r_blocks_count,
           super.s_free_blocks_count,
           super.s_free_inodes_count,
           super.s_state,
           super.s_blocks_per_group,
           super.s_magic);
  }

  void printBGDT(group_descriptor groupDescriptor[], unsigned int totalBlockGroups) {
    printf("Group    Block     Inode      Inode    Free      Free        Used\n"
           "         Bitmap    Bitmap     Table    Blocks    Inodes      Dirs\n"
           "-----------------------------------------------------------------\n");
    for (int i = 0; i < totalBlockGroups; i++) {
      printf("%5d %9u %9u %9u %9u %9u %9u\n",
             i,
             groupDescriptor[i].block_bitmap,
             groupDescriptor[i].inode_bitmap,
             groupDescriptor[i].inode_table,
             groupDescriptor[i].free_blocks_count,
             groupDescriptor[i].free_inodes_count,
             groupDescriptor[i].used_dirs_count);
    }
    printf("\n");
  }



#endif
