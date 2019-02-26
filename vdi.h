#ifndef VDIHEADER
#define VDIHEADER
 struct vdi
  {

    /*  0*/ unsigned int	hdr[4];
    /* 64*/ unsigned int	imgsignature;		/* VDI_HEADER_MAGIC		*/
    /* 68*/ unsigned int	verison;	/* VDH_HEADER_MAJOR/MINOR	*/
    /* len is calculated from here	*/
    /* 72*/ unsigned int	hsize;		/* VDI_HEADER_LEN_MIN to _MAX	*/
    /* 76*/ unsigned int		imgtype;		/* ?				*/
    /* 80*/ unsigned int  imgflags;		/* ?				*/
    /* 84*/ unsigned int	desc[64];	/* ?				*/
    /*340*/ unsigned int	block;		/* offset to block info		*/
    /*344*/ unsigned int	blockdata;		/* offset to data		*/
    /*348*/ unsigned int	cyl;		/* number of cylinders		*/
    /*352*/ unsigned int		head;		/* number of heads to emulate	*/
    /*356*/ unsigned int	sec;		/* number of sectors per track	*/
    /*360*/ unsigned int		secsize;	/* number of bytes in a sector	*/
    /*364*/ unsigned int		unk1;
    /*368*/ unsigned int disksize[2];	/* Total size of the disk, in byte	*/
    /*376*/ unsigned int locksize;	/* Size of a block: 1M		*/
    /*380*/ unsigned int		extradata;	/* unknown			*/
    /*384*/ unsigned int blocks;		/* Number of blocks in image	*/
    /*388*/ unsigned int blockuse;	/* Total number of blocks used	*/


    unsigned int uuid[4]; /*UUID of this VDI*/
    unsigned int uusnap[4];                      /*UUID of last snap*/
    unsigned int uulink[4];/*UUID link*/
     unsigned  int uupar[4];/*UUID parent*/

    /*456*/ unsigned int garbage[14];	/* no more in header len 0x190	*/
    /*512*/
  } ;
#endif
