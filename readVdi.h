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

  ssize_t VDIread(VDIFile *v,void *buff,ssize_t num){
    ssize_t nBytes= read(v->file,buff,num);
    if (num != nBytes){
      cout <<"Error";
      return 1;
    }
    return nBytes;
    v->cursor= v->header.offsetdata;
  }
  /*  int readMap(VDIFile *v, int headerMap[]){
  off_t offset= lseek (v->file,v->header.offsetblock, SEEK_END);
  cout << dec<<v->header.offsetblock<< endl;
  int map= read(v->file,headerMap,4*(v->header.blocks));
  cout << dec << "vdi" << map<< endl;
  return 0;
  }*/

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


  unsigned char* fetchBlock(VDIFile *f, unsigned int blockNum, int location, unsigned int blockSize){

    unsigned char* buff = (unsigned char*)malloc(blockSize);

    int num = (blockNum * blockSize) + f->header.offsetdata+location;
    off_t offset=lseek(f->file, num , SEEK_SET);
    int block=read(f->file, buff, blockSize);
    return buff;
  }

   Inode fetchInode(VDIFile *f,int inodeNumber,Superblock super, group_descriptor group[],unsigned int blockSize){
   Inode inode;
   unsigned char* buff = (unsigned char*)malloc(blockSize);
   inodeNumber--;
   unsigned int blockgroup      = inodeNumber/super.s_inodes_per_group;
   unsigned int blockGroupNum   = inodeNumber % super.s_inodes_per_group;
   unsigned int inodesPerBlock  = blockSize/sizeof(Inode);
   unsigned int blockNum           = inodeNumber/inodesPerBlock;
   unsigned int inodeGroupNumber= inodeNumber% inodesPerBlock;
   fetchBlock(f,blockNum,buff,group[blockgroup].inode_table+blockNum,blockSize);
   Inode  *ibuff =(Inode *)malloc(blockSize);
   ibuff=(Inode*) &buff;
   return ibuff[inodeGroupNumber];

 }



#endif
