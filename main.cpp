#include "vdi.h"
#include "vdifile.h"
#include "readVdi.h"
#include <iostream>


 using namespace std;

  int main(int  argc,  char* argv[]){

    int vfile;
   VDIFile* f =new VDIFile();
   vfile=vdifileopen(f,argv[1]);

   ssize_t num;
   num = read(f,&(f->header),sizeof(f->header));
   int headerMap[f->header.blocks];
   int h;
   h=readHeader(f,headerMap);
cout << f->header.imgsignature;
   cout <<f->header.imgsignature<< endl;;

  }
