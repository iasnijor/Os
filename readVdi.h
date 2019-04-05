#ifndef READVDI
#define READVDI
#include "vdifile.h"
#include "vdi.h"
#include "mbr.h"
#include "superBlock.h"
#include "groupDescriptor.h"
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

  int VDISeek (VDIFile* v, off_t off, int s){
    int loc= lseek(v->file,off,s);
    return loc;
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
    off_t offset= lseek(f->file,f->header.offsetdata,SEEK_SET);
    if (offset< 0) cout << "Error"<<endl;
    int mbr = read(f->file,&b,sizeof(b));
    return mbr;
  }

  int readSuperblock(VDIFile *f,int loc, Superblock& s){
    off_t offset= lseek(f->file,f->header.offsetdata+loc,SEEK_SET);
    if (offset<0) cout << "Error"<< endl;
    int super= read(f->file,&s, sizeof(s));
    if(super==1024)
    cout <<"no" << endl;
  }

  int readGroupDescriptor(VDIFile *f,  unsigned int blockSize, group_descriptor groupDescriptor[], unsigned int groupCount){
      lseek(f->file,1024 + blockSize,SEEK_SET);
      read(f->file,groupDescriptor, sizeof(group_descriptor) * groupCount);

      // unsigned int location = 1024 + blockSize;
      // off_t offset = lseek(f->file,location,SEEK_SET);
      // cout<< "offset: " << offset <<endl;
      // if(offset < 0) cout << "can not seek" << endl;
      //
      // int descriptor = read(f->file, groupDescriptor, sizeof(group_descriptor) * blockGroupCount);
      // cout << "read: " << descriptor << endl;
      // if(descriptor != sizeof(group_descriptor) * blockGroupCount) cout << "can not read" << endl;

    }
#endif
