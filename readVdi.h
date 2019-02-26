#ifndef READVDI
#define READVDI
#include "vdifile.h"
#include "vdi.h"
#include <cstdint>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;
int vdifileopen(VDIFile *v,  char* name){

    v->file = open(name,O_RDONLY);

    return v->file;
}
void vdiFileClose(VDIFile *v){
  if (!v)
  return;
   close(v->file);
  }
  ssize_t read(VDIFile *v,void *buff,ssize_t num){
    ssize_t nBytes= read(v->file,buff,num);
    if (num != nBytes){
      printf("Error");
      return 1;
          }
          return 0;
  }

  int readHeader(VDIFile *v, int headerMap[]){
    int  offset= lseek (v->file,v->header.block, SEEK_SET);

    int map= read(v->file,headerMap,4*(v->header.blocks));
    return 0;
  }
#endif
