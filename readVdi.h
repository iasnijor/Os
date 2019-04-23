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

  // Mimics UNIX read function
  ssize_t VDIread(VDIFile *v,void *buff,ssize_t num){
    ssize_t nBytes= read(v->file,buff,num);
    if (num != nBytes){
      cout <<"Error";
      return 1;
    }
    return nBytes;
    v->cursor= v->header.offsetdata;
  }


  // reads MBR
  int readMBR(VDIFile *f, BootRecord&  b){
    off_t offset= VDISeek(f,f->header.offsetdata,SEEK_SET);
    if (offset< 0) cout << "Error"<<endl;
    int mbr = VDIread(f,&b,sizeof(b));
    return mbr;
  }
  //Function to read Superblock
  int readSuperblock(VDIFile *f,int loc, Superblock& s){
    off_t offset= VDISeek(f,loc,SEEK_SET);
    if (offset<0) cout << "Error"<< endl;
    int super= VDIread(f,&s, sizeof(s));

  }
  //Function to read groupDescriptor
  int readGroupDescriptor(VDIFile *f,  unsigned int blockSize, group_descriptor groupDescriptor[], unsigned int groupCount){
      off_t offset=VDISeek(f,blockSize,SEEK_SET);
      int group=VDIread(f,groupDescriptor, sizeof(group_descriptor) * groupCount);
    }

    //Function to fetchBlock
    uint8_t* fetchBlock(VDIFile *f, unsigned int blockNum, uint8_t* buff, int location, unsigned int blockSize){
      int num = (blockNum * blockSize) + location;
      off_t offset=VDISeek(f, num , SEEK_SET);
      int block=VDIread(f, buff, blockSize);
      return buff;
    }

   // Function to fetch inode
   Inode fetchInode(VDIFile *f,int inodeNumber,Superblock super, group_descriptor group[],unsigned int blockSize,int filesystemstart){
   Inode inode;
   Inode* ibuff = new Inode[blockSize];
   inodeNumber--;
   unsigned int blockgroup      = inodeNumber/super.s_inodes_per_group;
   unsigned int blockGroupNum   = inodeNumber % super.s_inodes_per_group;
   unsigned int inodesPerBlock  = blockSize/sizeof(Inode);
   unsigned int blockNumber     = blockGroupNum/inodesPerBlock;
   inodeNumber= blockGroupNum% inodesPerBlock;
   int  blockNum =group[blockgroup].inode_table+blockNumber;
   fetchBlock(f,blockNum,(uint8_t*)ibuff,filesystemstart,blockSize);
   return ibuff[inodeNumber];
 }

//Print Entries of Superblock
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

  //Prints Block Group Descriptor Table
  void printBGDT(group_descriptor groupDescriptor[], unsigned int totalBlockGroups) {
    int usedDir=0;
    printf("Group    Block     Inode      Inode    Free      Free        Used\n"
           "         Bitmap    Bitmap     Table    Blocks    Inodes      Dirs\n"
           "-----------------------------------------------------------------\n");
    for (int i = 0; i < totalBlockGroups; i++) {
    usedDir += groupDescriptor[i].used_dirs_count;
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
    cout <<"Number of existing directories: "<<usedDir <<endl;
  }

//FUnction to read Directory
  void readDir( int inodeSize, uint8_t *buff){
      dirEntry *entry;
    //  List<int> inodes= new List<int>;

      unsigned int cursor=24;
      entry=(dirEntry *)(buff+cursor);
       int rec=entry->rec_len;
      while(cursor < inodeSize ) {
      char file_name[256];
      memcpy(file_name, entry->name, entry->name_len);
      file_name[entry->name_len] = '\0';              // append null char to the file name
      printf("Inode,%10u %s\n", entry->inode, file_name);
// entry  += entry->rec_len;      // move to the next entry
    //  Inode i= fetchInode(f,entry->inode,super,groupDescriptor,blockSize,filesystemstart);
    //    inodes.push(entry->inode);
      cursor+= entry->rec_len;
      entry=(dirEntry *)(buff+cursor);
    }
//    retutn inodes;
  //  free(entry)

  }

  void fetchBlockfromFile(VDIFile*f,Inode *i, int inodeBlockNum, uint8_t *buff,unsigned int blockSize, int filesystemstart ){
      unsigned int *list;
        unsigned int ipb=blockSize/4;
       if (inodeBlockNum <12){
       list =i->i_block;
       goto Direct;
     }
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
       fetchBlock(f,list[inodeBlockNum/(ipb*ipb*ipb)],buff,filesystemstart,blockSize);
       list= ((unsigned *)buff);
       inodeBlockNum %= ipb * ipb * ipb;

     Double:
       {
         fetchBlock(f,list[inodeBlockNum/(ipb*ipb)],buff,filesystemstart,blockSize);
         list= ((unsigned *)buff);
         inodeBlockNum %= ipb * ipb;
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
 void compareSuperblock(Superblock super, Superblock rhs){
   if (super.s_inodes_count!= rhs.s_inodes_count){cout << "Eroorin Superblock"<< endl;rhs.s_inodes_count=super.s_inodes_count;}
   if (super.s_blocks_count !=rhs.s_blocks_count){rhs.s_blocks_count=super.s_blocks_count;}
   if(super.s_r_blocks_count!=rhs.s_r_blocks_count){rhs.s_r_blocks_count=super.s_r_blocks_count;}
   if(super.s_free_blocks_count!=rhs.s_free_blocks_count){rhs.s_free_blocks_count=super.s_free_blocks_count;}
   if(super.s_free_inodes_count!=rhs.s_free_inodes_count){rhs.s_free_inodes_count=super.s_free_inodes_count;}
   if(super.s_first_data_block!=rhs.s_first_data_block){rhs.s_first_data_block=super.s_first_data_block;}
   if(super.s_log_block_size!=rhs.s_log_block_size){rhs.s_log_block_size=super.s_log_block_size;}
   if(super.s_log_frag_size!=rhs.s_log_frag_size){rhs.s_log_frag_size=super.s_log_frag_size;}
   if(super.s_blocks_per_group!=rhs.s_blocks_per_group){rhs.s_blocks_per_group=super.s_blocks_per_group;}
   if(super.s_frags_per_group!=rhs.s_frags_per_group){rhs.s_frags_per_group=super.s_frags_per_group;}
   if(super.s_inodes_per_group!=rhs.s_inodes_per_group){rhs.s_inodes_per_group=super.s_inodes_per_group;}
   if(super.s_mtime!=rhs.s_mtime){rhs.s_mtime=super.s_mtime;}
   if(super.s_wtime!=rhs.s_wtime){rhs.s_wtime=super.s_wtime;}
   if(super.s_mnt_count!=rhs.s_mnt_count){rhs.s_mnt_count=super.s_mnt_count;}
   if(super.s_max_mnt_count!=rhs.s_max_mnt_count){rhs.s_max_mnt_count=super.s_max_mnt_count;}
   if(super.s_magic!=rhs.s_magic){rhs.s_magic=super.s_magic;}
   if(super.s_state!=rhs.s_state){rhs.s_state=super.s_state;}
   if(super.s_errors=rhs.s_errors){rhs.s_errors=super.s_errors;}
   if(super.s_minor_rev_level!=rhs.s_minor_rev_level){rhs.s_minor_rev_level=super.s_minor_rev_level;}
   if(super.s_lastcheck!=rhs.s_lastcheck){rhs.s_lastcheck=super.s_lastcheck;}
   if(super.s_checkinterval!=rhs.s_checkinterval){rhs.s_checkinterval=super.s_checkinterval;}
   if(super.s_creator_os!=rhs.s_creator_os){rhs.s_creator_os=super.s_creator_os;}
   if(super.s_rev_level!=rhs.s_rev_level){rhs.s_rev_level=super.s_rev_level;}
   if(super.s_def_resgid!=rhs.s_def_resgid){rhs.s_def_resgid=super.s_def_resgid;}
   if(super.s_def_resuid!=rhs.s_def_resuid){rhs.s_def_resuid=super.s_def_resuid;}
   if(super.s_first_ino!=rhs.s_first_ino){rhs.s_first_ino=super.s_first_ino;}
   if(super.s_inode_size!=rhs.s_inode_size){rhs.s_inode_size=super.s_inode_size;}
   if(super.s_block_group_nr!=rhs.s_block_group_nr){rhs.s_block_group_nr=super.s_block_group_nr;}

 }
void compareGroupDes(group_descriptor g[],group_descriptor rhs[],unsigned int groupCount){
  for (int i =0 ; i < groupCount; i++ ){
    if(g[i].block_bitmap!= rhs[i].block_bitmap){cout <<"Error block_bitmap "<<i<< endl;rhs[i].block_bitmap=g[i].block_bitmap;}
    if(g[i].inode_bitmap!= rhs[i].inode_bitmap){cout <<"Error inode_bitmap "<<i<< endl;rhs[i].inode_bitmap=g[i].inode_bitmap;}
    if(g[i].inode_table!= rhs[i].inode_table){cout <<"Error inode_table "<<i<< endl;rhs[i].inode_table=g[i].inode_table;}
    if(g[i].free_blocks_count!=rhs[i].free_blocks_count){cout <<"Error free_blocks_count "<<i<< endl;rhs[i].free_blocks_count=g[i].free_blocks_count;}
    if(g[i].free_inodes_count!= rhs[i].free_inodes_count){cout <<"Error free_inodes_count "<<i<< endl;rhs[i].free_inodes_count=g[i].free_inodes_count;}
    if(g[i].used_dirs_count!= rhs[i].used_dirs_count){cout <<"Error used_dirs_count "<<i<< endl;rhs[i].used_dirs_count=g[i].used_dirs_count;}
    if(g[i].pad!= rhs[i].pad){cout << "Erro pad"<< endl;rhs[i].pad=g[i].pad;}
      }
    }
    int bin(unsigned n)
    {
    if (n > 1)
    bin(n>>1);
    return(n &1);
    }
    
  void bin2(unsigned n){
    if (n > 1)
    bin(n>>1);
    printf("%d\n", n & 1);
    }

  bool power357(unsigned int number){
        return ((number!=0 && 1162261467%number==0) || (number !=0 && 762939453125%number ==0 )|| (number!=0 && 678223072849%number ==0));
      }
#endif
