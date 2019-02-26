#ifdef VDIFILE
#define VDIFILE
#include "vdi.h"

struct VDIFile{
  int file;
  vdi header;
  int filesize;
  int cursor;
}
#endif
