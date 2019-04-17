#ifndef DIR
#define DIR
struct __attribute__ ((packed)) dirEntry  {
	unsigned int	inode;			/* Inode number */
	unsigned short	rec_len;		/* Directory entry length */
	unsigned char	name_len;		/* Name length */
	unsigned char	file_type;
	char	name[4];	/* File name */
};
#endif
