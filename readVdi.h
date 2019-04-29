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
#include <vector>
#include<sys/stat.h>

using namespace std;
//Mimics UNIX file Open
  int vdifileopen(VDIFile *v,  char* name){
    v->file = open(name,O_RDWR);
    return v->file;
  }
  //Mimics UNIX file close
  void vdiFileClose(VDIFile *v){
    if (!v)
    return;
    close(v->file);
  }

  //Mimics UNIX file seeking
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
  //    cout <<"Error here ";
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
    uint8_t* fetchBlock(VDIFile *f, unsigned int blockNum, uint8_t* buff, int location, unsigned int blockSize, std::vector<int> &Block){
      int num = (blockNum * blockSize) + location;
    cout << blockNum <<" "<< Block.size()<< endl;
      Block.push_back(blockNum);
      off_t offset=VDISeek(f, num , SEEK_SET);
      int block=VDIread(f, buff, blockSize);
      return buff;
    }

   // Function to fetch inode
   Inode fetchInode(VDIFile *f,int inodeNumber,Superblock super, group_descriptor group[],unsigned int blockSize,int filesystemstart,std::vector<int> &Block){
     Inode inode;
     Inode* ibuff = new Inode[blockSize];
     inodeNumber--;
     unsigned int blockgroup      = inodeNumber/super.s_inodes_per_group;
     unsigned int blockGroupNum   = inodeNumber % super.s_inodes_per_group;
     unsigned int inodesPerBlock  = blockSize/sizeof(Inode);
     unsigned int blockNumber     = blockGroupNum/inodesPerBlock;
     inodeNumber= blockGroupNum% inodesPerBlock;
     int  blockNum =group[blockgroup].inode_table+blockNumber;
     fetchBlock(f,blockNum,(uint8_t*)ibuff,filesystemstart,blockSize,Block);
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

  //FUnction to fetch block from inode
  void fetchBlockfromFile(VDIFile*f,Inode *i, int inodeBlockNum, uint8_t *buff,unsigned int blockSize, int filesystemstart,std::vector<int> &Block ){
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
       fetchBlock(f,list[inodeBlockNum/(ipb*ipb*ipb)],buff,filesystemstart,blockSize,Block);
       list= ((unsigned *)buff);
       inodeBlockNum %= ipb * ipb * ipb;

     Double:
       {
         fetchBlock(f,list[inodeBlockNum/(ipb*ipb)],buff,filesystemstart,blockSize,Block);
         list= ((unsigned *)buff);
         inodeBlockNum %= ipb * ipb;
       }
       Single :{
         fetchBlock(f,list[inodeBlockNum/(ipb)],buff,filesystemstart,blockSize,Block);
         list= ((unsigned *)buff);
         inodeBlockNum= inodeBlockNum%ipb;
       }
       Direct:{
         fetchBlock(f,list[inodeBlockNum],buff,filesystemstart,blockSize,Block);
                }
       }

       //FUnction to read  and traverse Directory
       int readDir(VDIFile *f,Superblock super, group_descriptor *groupDescriptor,int filesystemstart,
         int blockSize, int inodeSize, uint8_t *buff, std::vector<int> &in ,
         std::vector<string> &dir,std::vector<string> &fil,std::vector<int> &Block){
           dirEntry *entry;
           unsigned int cursor=24;
           int i=0,x=0;
           entry=(dirEntry *)(buff+cursor);
            int rec=entry->rec_len;
           while(cursor < inodeSize ) {
           char file_name[256];
           memcpy(file_name, entry->name, entry->name_len);
           file_name[entry->name_len] = '\0';
           int type=(int)entry->file_type;
           string name = (string)entry->name;


           if(type==1 ){fil.push_back(name);
          /*   std::vector<int> in3;
          //   in3= traverseinodes(f,super,groupDescriptor,filesystemstart,blockSize,dir,fil,entry->inode);
            Inode i= fetchInode(f,entry->inode,super,groupDescriptor,blockSize,filesystemstart,Block);
            if(S_ISREG(i.i_mode)){cout<<"Its a file."<<endl;
            for (int j = 0 ; j <15;j++){
              if(i.i_block[j]!=0){
                cout <<dec<< "Block is used "<<j<< endl;
              uint8_t* buf = new uint8_t[blockSize];
               fetchBlockfromFile(f,&i,j,buf,blockSize,filesystemstart,Block);
              x+=readDir(f,super,groupDescriptor,filesystemstart,blockSize,i.i_size,buf,in,dir,fil,Block);
                      }
                  }            // append null char to the file name
            }*/
          }


           if(type==2 ){
           dir.push_back(name);
           if(name!="lost+found"){
           std::vector<int> in3;
        //   in3= traverseinodes(f,super,groupDescriptor,filesystemstart,blockSize,dir,fil,entry->inode);
          Inode i= fetchInode(f,entry->inode,super,groupDescriptor,blockSize,filesystemstart,Block);
              if(S_ISREG(i.i_mode)){cout<<"Its a file."<<endl;
                    for (int j = 0 ; j <15;j++){
                              if(i.i_block[j]!=0){
                                cout <<dec<< "Block is used "<<j<< endl;
                              uint8_t* buf = new uint8_t[blockSize];
                               fetchBlockfromFile(f,&i,j,buf,blockSize,filesystemstart,Block);
                              x+=readDir(f,super,groupDescriptor,filesystemstart,blockSize,i.i_size,buf,in,dir,fil,Block);
                                      }
                                }
                              }
          if(S_ISDIR(i.i_mode)){
                      for (int j = 0 ; j <15;j++){
                        if(i.i_block[j]!=0){
                          cout <<dec<< "Block is used "<<j<< endl;
                        uint8_t* buf = new uint8_t[blockSize];
                         fetchBlockfromFile(f,&i,j,buf,blockSize,filesystemstart,Block);
                        x+=readDir(f,super,groupDescriptor,filesystemstart,blockSize,i.i_size,buf,in,dir,fil,Block);
                                        }
                                      }
                            }
                        }
                     }
          printf("Inode,%10u %s\n", entry->inode, file_name);
           in.push_back(entry->inode);
           i++;
           cursor+= entry->rec_len;
           entry=(dirEntry *)(buff+cursor);

       }
         return i+x;
       //  free(entry)

       }

//Function to compare and correct Superblock
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

 //Function to compare and correct groupDescriptor
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

//Checks if the number is power of 357
// I got this function from https://stackoverflow.com/questions/1804311/how-to-check-if-an-integer-is-a-power-of-3
// it chcks powers upto 3 ^19 , 5^17 and 7^14
  bool power357(unsigned int number){
        return ((number!=0 && 1162261467%number==0) || (number !=0 && 762939453125%number ==0 )|| (number!=0 && 678223072849%number ==0));
      }

// Print a vector
    void printinodeNumber(vector<int> &v){
      for (int i=0; i<v.size();i++){
        cout << v.at(i)<< endl;
      }
    }

//Print bitmaps
    void printbitmaps(int bitmap[], int totalBlocks){
      for (int i=0;i<254*16;i++){
        if (i%254==0)cout << " "<< endl;

        cout <<dec<< bitmap[i]<<" ";
      }
    }
    void printbitmaps2(string bitmap1[],string bitmap2[], int totalBlocks){
      for (int i=0;i<254*16;i++){
        cout <<dec<< bitmap1[i]<< " "<< bitmap2[i]<< endl;
        if (i%1024==0)cout << " "<< endl;
      }
    }

#endif
