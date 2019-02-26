#ifdef VDIHEADER
#define VDIHEADER

typedef struct vdi
  {

    /*  0*/ unsigned char	hdr[64];
    /* 64*/ unsigned long	imgsignature;		/* VDI_HEADER_MAGIC		*/
    /* 68*/ int			maj, min;	/* VDH_HEADER_MAJOR/MINOR	*/
    /* len is calculated from here	*/
    /* 72*/ long		len;		/* VDI_HEADER_LEN_MIN to _MAX	*/
    /* 76*/ long		typ;		/* ?				*/
    /* 80*/ long		flags;		/* ?				*/
    /* 84*/ unsigned char	desc[256];	/* ?				*/
    /*340*/ unsigned long	block;		/* offset to block info		*/
    /*344*/ unsigned long	data;		/* offset to data		*/
    /*348*/ long		cyl;		/* number of cylinders		*/
    /*352*/ long		head;		/* number of heads to emulate	*/
    /*356*/ long		sec;		/* number of sectors per track	*/
    /*360*/ long		secsize;	/* number of bytes in a sector	*/
    /*364*/ long		unk1;
    /*368*/ int64_t		disksize;	/* Total size of the disk, in byte	*/
    /*376*/ long		blocksize;	/* Size of a block: 1M		*/
    /*380*/ long		extradata;	/* unknown			*/
    /*384*/ long		blocks;		/* Number of blocks in image	*/
    /*388*/ long		blockuse;	/* Total number of blocks used	*/

    /*392*/ UUID		uuid, uusnap, uulink, uupar;

    /*456*/ unsigned char	unk2[16];	/* no more in header len 0x180	*/
    /*480*/ unsigned char	unk3[40];	/* no more in header len 0x190	*/
    /*512*/
  } VDI;
