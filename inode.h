#ifndef INODE
#define INODE

struct __attribute__ ((packed)) Inode {

	unsigned short i_mode; 		/*	File mode */
	unsigned short i_uid;		/* low 16 bits of owner Uid */
	unsigned int i_size;		/*  Size in bytes */
	unsigned int i_atime;		/* Access time */
	unsigned int ctime;    /* Creation time*/
	unsigned int i_mtime;		/* Creation time */
	unsigned int i_dtime;		/* Modification time */
	unsigned short i_gid;			/* Low 16 bits of Group id */
	unsigned short i_links_count;	/* links count */
	unsigned int i_blocks;		/* Blocks count */
	unsigned int i_flags;		/* File flags */

	union __attribute__ ((packed)){
		struct __attribute__ ((packed))
		{
			unsigned int l_i_reserved1;
		} linux1;
		struct __attribute__ ((packed))
		{
			unsigned int h_i_translator;
		} hurd1;
		struct __attribute__ ((packed))
		{
			unsigned int m_i_reserved1;
		} masix1;
	} osd1;

	unsigned int i_block[15]; 	/* Pointers to the blocks */
	unsigned int l_version;					/* The file version	*/
	unsigned int i_file_acl;				/* File ACL */
	unsigned int i_dir_acl;					/* Directory ACL */
	unsigned int i_faddr;					/* Fragment Address */

	union  __attribute__ ((packed)){
		struct __attribute__ ((packed))
		{
			unsigned char l_i_frag;			/* Fragment number */
			unsigned char l_i_fsize;		/* Fragment size */
			unsigned short i_pad1;
			unsigned short l_i_uid_high;	/* high 16 bit user id*/
			unsigned short l_i_gid_high;	/* high 16 bit of group id */
			unsigned int l_i_reversed2;
		} linux2;
		struct __attribute__ ((packed))
		{
			unsigned char h_i_frag;			/* 8 bit fragment number */
			unsigned char h_i_fsize;		/*  8 bit fragment size */
			unsigned short h_i_mode_high;
			unsigned short h_i_uid_high;
			unsigned short h_i_gid_high;
			unsigned int h_i_author;
		}	hurd2;
		struct __attribute__ ((packed))
		{
			unsigned char m_i_frag;
			unsigned char m_i_fsize;
			unsigned short m_pad1;
			unsigned int m_i_reserved2[2];
		}	masix2;
	} osd2;
};

#endif
