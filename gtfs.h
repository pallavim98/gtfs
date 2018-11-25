#define u32 unsigned int
#define u16 unsigned short int
#define gtfs_NDIR_BLOCKS  56  /* number of direct blocks */
#define gtfs_INODE_BLOCKS  5  /* number of direct blocks */
#define max_filepointers 100

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>




/*-----------------------------------------------------------------------------------------------------------------------------*/

typedef struct gtfs_super_block {

	u32	s_inodes_count;		/* Inodes count */
	u32	s_blocks_count;		/* Blocks count */
	u32	s_free_blocks_count;	/* Free blocks count */
	u32	s_free_inodes_count;	/* Free inodes count */
	char* s_first_data_block;	/* First Data Block */
	u32	s_block_size;	/* Block size */

} gtfs_super_block;

/*-----------------------------------------------------------------------------------------------------------------------------*/

void gtfs_super_block_init(gtfs_super_block *s, char* s_first_data_block, u32	s_block_size, u32 s_inodes_count);

/*-----------------------------------------------------------------------------------------------------------------------------*/

//function to simulate disk

void gtfs_create_disk();

/*-----------------------------------------------------------------------------------------------------------------------------*/

//function to initialize inode bitmap

void gtfs_ibmap_init(int* starting_addr_ibmap);

/*-----------------------------------------------------------------------------------------------------------------------------*/

//function to initialize data bitmap

void gtfs_dbmap_init(int* starting_addr_dbmap);

/*-----------------------------------------------------------------------------------------------------------------------------*/


typedef struct gtfs_inode {
        char  name[50];           //file name
	u16   type;           /*0 : file, 1:dir */
        u16   i_mode;         /* File type and access rights */
        u32   i_offset;         //file offset
        u16   i_uid;          /* Low 16 bits of Owner Uid */
        u32   i_size;         /* Size in bytes */
       // time_t   i_atime;        /* Access time */
       // time_t   i_ctime;        /* Creation time */
       // time_t   i_mtime;        /* Modification time */
       // time_t   i_dtime;        /* Deletion Time */
        char i_atime[30];
        char i_ctime[30];
        char i_mtime[30];
        char i_dtime[30];
        u16   i_gid;          /* Low 16 bits of Group Id */
        u16   i_links_count;  /* Links count */
        u32   i_blocks;       /* Blocks count */
       // void*   i_block[gtfs_NDIR_BLOCKS];  /* Pointers to blocks */
        int i_block[gtfs_NDIR_BLOCKS];
        u32   i_no;           //inode no

}gtfs_inode;


/*-----------------------------------------------------------------------------------------------------------------------------*/

//function to initialize gtfs_inode structure

void inode_init(gtfs_inode* i, u32 i_no);

/*-----------------------------------------------------------------------------------------------------------------------------*/

//function to get inode number

gtfs_inode* getinode(void* a[5], int ino);

/*-----------------------------------------------------------------------------------------------------------------------------*/

//void inode_free(gtfs_inode* inode_ptr);

int gtfs_stat(gtfs_inode* i);

/*-----------------------------------------------------------------------------------------------------------------------------*/

gtfs_inode* gtfs_open(char *filename , u32 mode, gtfs_inode *dir, void* arr[5]);

/*-----------------------------------------------------------------------------------------------------------------------------*/

int gtfs_rm(void* arr[5],char*filename,gtfs_inode* root);

/*-----------------------------------------------------------------------------------------------------------------------------*/

int gtfs_rename(void*arr[5], gtfs_inode* fp, char* newname);

/*-----------------------------------------------------------------------------------------------------------------------------*/

int gtfs_read(gtfs_inode* inode, char* data, void* arr[5]);

/*-----------------------------------------------------------------------------------------------------------------------------*/

int gtfs_write(gtfs_inode* inode, char* data, void* arr[5],int flag);

/*-----------------------------------------------------------------------------------------------------------------------------*/

void gtfs_close(gtfs_inode* fp);

/*-----------------------------------------------------------------------------------------------------------------------------*/

gtfs_inode* gtfs_mkdir(char *dirname ,u32 mode,void* arr[5],gtfs_inode* root);

/*-----------------------------------------------------------------------------------------------------------------------------*/

gtfs_inode* gtfs_readdir(gtfs_inode* inode,char* dirname,void* arr[5]);

/*-----------------------------------------------------------------------------------------------------------------------------*/

void gtfs_ls(gtfs_inode* inode, void* arr[5]);

/*-----------------------------------------------------------------------------------------------------------------------------*/

int gtfs_rmdir(void* arr[5],char* dirname,gtfs_inode *root);

/*-----------------------------------------------------------------------------------------------------------------------------*/

void memcopy(void* arr[5]);

/*-----------------------------------------------------------------------------------------------------------------------------*/

void memaloc(void* arr[5]);

/*-----------------------------------------------------------------------------------------------------------------------------*/
