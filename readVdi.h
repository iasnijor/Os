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
#include "dir.h"
#include <string.h>
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
    off_t offset= VDISeek(f,loc,SEEK_SET);
    if (offset<0) cout << "Error"<< endl;
    int super= VDIread(f,&s, sizeof(s));

  }

  int readGroupDescriptor(VDIFile *f,  unsigned int blockSize, group_descriptor groupDescriptor[], unsigned int groupCount){
      off_t offset=VDISeek(f,blockSize,SEEK_SET);
      int group=VDIread(f,groupDescriptor, sizeof(group_descriptor) * groupCount);
    }


    uint8_t* fetchBlock(VDIFile *f, unsigned int blockNum, uint8_t* buff, int location, unsigned int blockSize){

      //uint8_t* buff = new uint8_t[blockSize];

      int num = (blockNum * blockSize) + location;
      off_t offset=VDISeek(f, num , SEEK_SET);
      int block=VDIread(f, buff, blockSize);
      return buff;


    }

   Inode fetchInode(VDIFile *f,int inodeNumber,Superblock super, group_descriptor group[],unsigned int blockSize,int filesystemstart){
   Inode inode;
   //uint8_t* buff = (uint8_t*)malloc(blockSize);

   Inode* ibuff = new Inode[blockSize];
   inodeNumber--;
   unsigned int blockgroup      = inodeNumber/super.s_inodes_per_group;
   unsigned int blockGroupNum   = inodeNumber % super.s_inodes_per_group;
   unsigned int inodesPerBlock  = blockSize/sizeof(Inode);
   unsigned int blockNumber     = blockGroupNum/inodesPerBlock;
   unsigned int inodeGroupNumber= blockGroupNum% inodesPerBlock;
    cout <<"B " <<blockgroup << " "<< blockGroupNum << " "<< inodesPerBlock<< " " << " " << blockNumber << " "<<  inodeGroupNumber<< endl;
  int  blockNum =group[blockgroup].inode_table+blockNumber;
  fetchBlock(f,blockNum,(uint8_t*)ibuff,filesystemstart,blockSize);
  return ibuff[inodeNumber];

 }
/*Inode readInode(VDIFile* f, unsigned int inodeCount,int filesystemstart,unsigned int blockSize,Superblock superBlock, group_descriptor groupDescriptor[]){
	Inode inode;
	inodeCount--;
	unsigned int groupCount = inodeCount / superBlock.s_inodes_per_group;
	unsigned int offset1 = inodeCount % superBlock.s_inodes_per_group;
	unsigned int inodesPerBlock = blockSize / sizeof(Inode);
	unsigned int blockNum = groupDescriptor[groupCount].inode_table + (offset1 / inodesPerBlock);
	unsigned int offset2 = inodeCount % inodesPerBlock;
	unsigned int val = (blockNum * blockSize) + (offset2 * sizeof(Inode));
	unsigned int loc =val;
	lseek(f->file,val+file, SEEK_SET);
	read(f->file, &inode, sizeof(Inode));
	return inode;
}*/


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


  void readDir( int inodeSize, uint8_t *buff){
      unsigned int cursor=400;

      dirEntry *entry=(dirEntry *)buff+cursor;
      /* first entry in the directory */
      //memcpy(entry,buff,sizeof(*entry));
      cout << "fucntionrec"<<entry->rec_len<<" "<< inodeSize<<  endl;
      while(cursor < inodeSize ) {
      char file_name[256];
      memcpy(file_name, entry->name, entry->name_len);
      file_name[entry->name_len] = '\0';              // append null char to the file name
      printf("Inode,%10u %s\n", entry->inode, file_name);
     entry  += entry->rec_len;      // move to the next entry
      cursor+= entry->rec_len;
    }
  //  free(entry)

  }

  void fetchBlockfromFile(VDIFile*f,Inode *i, int inodeBlockNum, uint8_t *buff,unsigned int blockSize, int filesystemstart ){
      unsigned int *list = new unsigned int[15];
      unsigned int ipb=blockSize/4;
      cout << "IPB "<<ipb<< " "<<inodeBlockNum<< " " << i->i_block<<endl;
     if (inodeBlockNum <12){
       list =i->i_block;
       goto Direct;
     }
     cout << "Here"<< endl;
     inodeBlockNum-=12;
     if (inodeBlockNum<ipb){
       list =i->i_block+12;
       goto Single;
     }
     inodeBlockNum-=ipb;
     if(inodeBlockNum <ipb*ipb){
       list=i->i_block+13;
       goto Double;
     }
     inodeBlockNum-=ipb*ipb;
     list=i->i_block+14;
      goto Triple;
     Triple :{
       fetchBlock(f,list[inodeBlockNum/(ipb*ipb*ipb)],buff,filesystemstart,blockSize);
       list= ((unsigned *)buff)+ inodeBlockNum/(ipb*ipb);
     }
     Double:
       {
         fetchBlock(f,list[inodeBlockNum/(ipb*ipb)],buff,filesystemstart,blockSize);
         list= ((unsigned *)buff)+ inodeBlockNum;
         inodeBlockNum= inodeBlockNum%(ipb*ipb);
       }
       Single :{
         fetchBlock(f,list[inodeBlockNum/(ipb)],buff,filesystemstart,blockSize);
         list= ((unsigned *)buff);
         inodeBlockNum= inodeBlockNum%ipb;
       }
       Direct:{
         fetchBlock(f,list[inodeBlockNum],buff,filesystemstart,blockSize);
                }
       }


#endif
