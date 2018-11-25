//#define FUSE_USE_VERSION 30
#include "gtfs.h"
/*-----------------------------------------------------------------------------------------------------------------------------*/
//DO THIS ALL ATTRIBUTES NOT DONE
void gtfs_super_block_init(gtfs_super_block *s, char* s_first_data_block, u32	s_block_size, u32	s_inodes_count)
{
	// pass that - arr[0],arr[4],4*1024
	s->s_inodes_count = s_inodes_count;
	s->s_blocks_count = gtfs_NDIR_BLOCKS;
	s->s_free_blocks_count = gtfs_NDIR_BLOCKS;
	s->s_free_inodes_count = s_inodes_count;
	s->s_first_data_block = s_first_data_block;
	s->s_block_size = s_block_size;
	//printf("--%d--\n",s);
}

/*-----------------------------------------------------------------------------------------------------------------------------*/


void gtfs_ibmap_init(int* starting_addr_ibmap)
{
	for(int i=0;i<gtfs_INODE_BLOCKS;i++)
	{
		starting_addr_ibmap[i]=0;
	}
}

/*-----------------------------------------------------------------------------------------------------------------------------*/

void gtfs_dbmap_init(int* starting_addr_dbmap)
{
	for(int i=0;i<gtfs_NDIR_BLOCKS;i++)
	{
		starting_addr_dbmap[i]=0;
	}
}

/*-----------------------------------------------------------------------------------------------------------------------------*/

void inode_init(gtfs_inode* i, u32 i_no)
{
			//printf("%d \n",i);
      i->name[0] = '\0';
	  i->type = -1;
	  i->i_mode = -1;
	  i->i_offset = 0;
      i->i_no = i_no;
      i->i_uid = 0;//getuid();
      i->i_size = 1;
      for(int j=0;j<30;j++)
     {
     		i->i_atime[j] = '\0';
				i->i_ctime[j] = '\0';
      	i->i_mtime[j] = '\0';
      	i->i_dtime[j] = '\0';
  		}
      i->i_gid = 0;//getgid();
      i->i_links_count = 0;
      i->i_blocks = 0 ;
      for(int j=0;j<gtfs_NDIR_BLOCKS;j++)
			{
				i->i_block[j] = -1;
			}
}

/*-----------------------------------------------------------------------------------------------------------------------------*/

gtfs_inode* getinode(void* a[5], int ino) //* or **?
{
	int n = (4*1024/sizeof(gtfs_inode)); // number of inodes in each block
	int block = ino/n; // number of blocks before the ino
	int rem = ino-block*n; // ino pos in its block
	return (gtfs_inode*)(a[3]+block*4*1024+rem*sizeof(gtfs_inode));
}
/*-----------------------------------------------------------------------------------------------------------------------------*/


void gtfs_create_disk(void*arr[5])
{
	//void* arr[5];
	arr[0] = malloc(1*4*1024); //1 block for super block
	arr[1] = malloc(1*4*1024); //1 block i-bmap block
	arr[2] = malloc(1*4*1024); //1 block d-bmap block
	arr[3] = malloc(gtfs_INODE_BLOCKS*4*1024); //gtfs_INODE_BLOCKS block inode block
	arr[4] = malloc(gtfs_NDIR_BLOCKS*4*1024); //gtfs_NDIR_BLOCKS blocks data block

	printf("Size of iNODE- %d\n",sizeof(gtfs_inode));
	int n = 4*1024/sizeof(gtfs_inode);
	printf("Number of iNODEs in one block - %d\n",n);
	printf("Number of iNODEs totally - %d\n\n",n*gtfs_INODE_BLOCKS);

	gtfs_super_block_init((gtfs_super_block*)(arr[0]),(char*) arr[4],4*1024,n*gtfs_INODE_BLOCKS);

	gtfs_ibmap_init((int*)arr[1]);
	gtfs_dbmap_init((int*)arr[2]);

	int ino;
	for (int k = 0; k < gtfs_INODE_BLOCKS; k++)
	{
		for (int j = 0; j < n; j++)
		{
			ino = k*n+j;
			inode_init(getinode(arr,ino),ino);
			if(ino<=2)
			{
				((int*)arr[1])[ino] = 1;
				if(ino==2)
				{
					gtfs_inode* root = getinode(arr,ino);
					root->i_block[1]= root->i_no;
					root->i_blocks =2;
					strcpy(root->name,"ROOT\0");
				}


			}

			//printf("iNode number init- %d\n",getinode(arr,ino)->i_no);
		}
	}

}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
int gtfs_stat(gtfs_inode* i)
{
	//gtfs_inode* i = (gtfs_inode*)j;
	printf("Name : %s \nInode number: %d \nType : %d \nMode: %d  \nOffset : %d \nUid : %d \nSize : %d \nAccess time: %s \nCreation time : %s \nModification time: %s \nDeletion time : %s \nGroup id: %d \nLinks count : %d \nBlocks: %d \n", i->name, i->i_no , i->type, i->i_mode,i->i_offset, i->i_uid, i->i_size, i->i_atime, i->i_ctime, i->i_mtime, i->i_dtime,i->i_gid, i->i_links_count, i->i_blocks);

return 0;
}

gtfs_inode* gtfs_open(char *filename , u32 mode, gtfs_inode *dir, void* arr[5])
{

	/*time_t cur;
  	struct tm * time_str;
	time ( &cur );
    time_str = localtime ( &cur );*/

    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';

	int flag=0;
	int check=0;
	int* bitmap = (int*)arr[1];
  int* dbitmap = (int*)arr[2];

  gtfs_inode **f;
	gtfs_inode *i ;

	gtfs_super_block* sblock = (gtfs_super_block*)arr[0];
	int ino_count = sblock->s_inodes_count;
	int dblock_count = sblock->s_blocks_count;
	//if file is already present
	//gtfs_inode* t = (gtfs_inode*)(arr[3]);
	//gtfs_inode* root = &t[2];

	for(int j=2;j<dir->i_blocks;j++)
	{

			i = getinode(arr,dir->i_block[j]);

			if(strcmp(filename,i->name)==0 && i->type==0)
				{

					i->i_mode = mode;
					strcpy(i->i_atime , time_str);
					flag = 1;
					return i;
				}

	}
	//if file is not present and mode is write or readandwrite
	if((mode&2) == 2 && flag==0)
	{

		if(sblock->s_free_inodes_count<=0 || sblock->s_free_blocks_count<=0)
		{
			printf("No available space.\n");
			return NULL;
		}
		for(int k=0;k<ino_count;k++)
		{
			if(bitmap[k]==0)
			{///arr[1][k] map to inode number
				bitmap[k]=1;
				for(int x=0;x<dblock_count;x++)
				{
					if(dbitmap[x]==0)
					{
			            dbitmap[x]=1;

						i = getinode(arr,k);
						inode_init(i,k);

						strcpy(i->name,filename);
						printf("%s created. \n",filename);
						char* str;
						str=(char*)(arr[4]+x*4096);
						//set datablock to \0
						for(int l=0;l<4096;l++)
							str[l]='\0';

						i->i_block[0]=x;

						i->i_blocks = 1;
						i->type =0;
						i->i_mode = mode;
						/*strcpy(i->i_atime , asctime (time_str));
						strcpy(i->i_mtime , asctime (time_str));
						strcpy(i->i_ctime , asctime (time_str));*/

						strcpy(i->i_atime ,  time_str);
						strcpy(i->i_mtime ,  time_str);
						strcpy(i->i_ctime ,  time_str);



						sblock->s_free_inodes_count--;
						sblock->s_free_blocks_count--;

						//return i;
						check=1;
						break;
					}


				}

				if(check)
					break;

			}
		}

		int a = 0;

		dir->i_block[dir->i_blocks]=i->i_no;
		dir->i_blocks+=1;

		return i;

	}
	else
	{
		return NULL;
	}

}



int gtfs_rm(void* arr[5],char*filename,gtfs_inode* root)
{
	/*time_t cur;
  struct tm * time_str;
	time ( &cur );
  time_str = localtime ( &cur );*/
	time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';

	int* bitmap = (int*)(arr[1]);
	int* dbitmap = (int*)arr[2];

	gtfs_super_block* sblock = (gtfs_super_block*)arr[0];
	int ino_count = sblock->s_inodes_count;

	for (int k = 2; k < root->i_blocks; k++)
	{

			gtfs_inode* t = getinode(arr,root->i_block[k]);
			if(strcmp(t->name,filename)==0 && t->type==0)
			{
				printf("%s removed.\n",filename);
				bitmap[t->i_no]=0;
				dbitmap[t->i_block[0]]=0;
				strcpy(t->i_dtime, time_str);
				sblock->s_free_blocks_count+=t->i_blocks;
				sblock->s_free_inodes_count++;
				for(int j=k+1; j < root->i_blocks; j++)
				{
					root->i_block[j-1]=root->i_block[j];
				}
				root->i_blocks-=1;
				inode_init(t,k);
				return 1;
			}
		//printf("%d %d ",bitmap[k],k);
	}
	printf("File does not exist.\n");
	return -1;
}

int gtfs_rename(void*arr[5], gtfs_inode* fp, char* newname)
{

		strcpy(fp->name,newname);
		return 1;

}

int gtfs_read(gtfs_inode* inode, char* data, void* arr[5])
{
	for (int i = 0; i < 100; ++i)
	{
		data[i] = '\0';
	}

  if(inode->i_mode&1 == 1)
  {
    int index=0;
    char* temp;
    for(int i=0;i<inode->i_blocks;i++) //set size
    {
      temp = (char*)(arr[4]+4096*inode->i_block[i]);
      for(int j=0;j<4*1024;j++) //set blocksize
      {
      	if(temp[j]=='\0')
      	{
      		return 1;
        }
        inode->i_offset++;
        data[index++] = temp[j];
      }
    }
    return 1;
  }
  return 0;
}

int gtfs_write(gtfs_inode* inode, char* data, void* arr[5], int flag)
{
	/*time_t cur;
  	struct tm * time_str;
	time ( &cur );
    time_str = localtime ( &cur );*/
    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';

  if(inode->i_mode == 2 || inode->i_mode == 3)
  {
    int index=0;
    char* temp;
    void* b = arr[4]+4096*inode->i_block[0];
		if(flag==1)
		{
			b+= inode->i_size -1;
		}
    temp = (char*)b;
		inode->i_size = strlen(data)+1;
    strcpy(temp,data);
    temp[strlen(data)]='\0';
    strcpy(inode->i_mtime , time_str);
    return 1;
  }
  return 0;
}

void gtfs_close(gtfs_inode* fp)
{
	free(fp);
}

gtfs_inode* gtfs_mkdir(char *dirname ,u32 mode,void* arr[5],gtfs_inode* root)
{
	/*time_t cur;
  	struct tm * time_str;
	time ( &cur );
    time_str = localtime ( &cur );*/
    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';

	int flag=0;
	int check =0;
	int* bitmap = (int*)arr[1];
    int* dbitmap = (int*)arr[2];
    gtfs_inode *i;

	gtfs_super_block* sblock = (gtfs_super_block*)arr[0];
	int ino_count = sblock->s_inodes_count;
	int dblock_count = sblock->s_blocks_count;

	if(sblock->s_free_inodes_count<=0 || sblock->s_free_blocks_count<=0)
		{
			printf("no available space\n");
			return NULL;
		}

	if(mode==5)
	{
		for(int k=0;k<ino_count;k++)
		{
			if(bitmap[k]==0)
			{
				bitmap[k]=1;

						i = getinode(arr,k);
						inode_init(i,k);

						strcpy(i->name,dirname);
						printf("%s created. \n",dirname);

						//gtfs_inode* root = (gtfs_inode*)(arr[3]+2*(sizeof(gtfs_inode)));
						i->i_block[0] = root->i_no;
						i->i_block[1] = i->i_no;

						i->i_blocks = 2;
						i->type =1;
						i->i_mode = mode;
						strcpy(i->i_atime , time_str);
						strcpy(i->i_mtime , time_str);
						strcpy(i->i_ctime , time_str);

						sblock->s_free_inodes_count--;

						root->i_block[root->i_blocks]=i->i_no;
						root->i_blocks+=1;
						return i;


					}

				}


			}
}

gtfs_inode* gtfs_readdir(gtfs_inode* inode,char* dirname,void* arr[5])
{
	int *t = inode->i_block;
	//gtfs_inode** filepointers = (gtfs_inode**)inode->i_block
	//printf("The following files under %s are: ",inode->name);

    for(int i=2;i<inode->i_blocks;i++)
    {
    	gtfs_inode *filepointer = getinode(arr,t[i]);
    	if(strcmp(filepointer->name,dirname)==0 && filepointer->type==1)
    		return filepointer;

    }
    return NULL;

}



void gtfs_ls(gtfs_inode* inode, void* arr[5])
{
	int *t = inode->i_block;
	//gtfs_inode** filepointers = (gtfs_inode**)inode->i_block;
	//printf("The following files under %s ",inode->name);

    for(int i=2;i<inode->i_blocks;i++)
    {
    	gtfs_inode *filepointer = getinode(arr,t[i]);
    	printf("%s ",filepointer->name);

    }

   printf("\n");
}

int gtfs_rmdir(void* arr[5],char* dirname,gtfs_inode *root)

{

	/*time_t cur;
  	struct tm * time_str;
	time ( &cur );
    time_str = localtime ( &cur );*/

    time_t mytime = time(NULL);
    char * time_str = ctime(&mytime);
    time_str[strlen(time_str)-1] = '\0';

	int* bitmap = (int*)(arr[1]);

	gtfs_super_block* sblock = (gtfs_super_block*)arr[0];
	int ino_count = sblock->s_inodes_count;

	for (int k = 2; k < root->i_blocks; k++)
	{
			gtfs_inode* t = getinode(arr,root->i_block[k]);

			if(strcmp(t->name,dirname)==0 && t->type==1)
			{
				if(t->i_blocks!=2)
				{
					printf("The directory must be empty.\n")	;
					return -1;
				}

				bitmap[t->i_no]=0;
				printf("%s removed.\n",t->name);
				strcpy(t->i_dtime , time_str);
				sblock->s_free_blocks_count+=t->i_blocks;
				sblock->s_free_inodes_count++;
				inode_init(t,t->i_no);

				for(int j=k+1; j < root->i_blocks; j++)
				{
					root->i_block[j-1]=root->i_block[j];
				}
				root->i_blocks-=1;
				return 1;
			}


	}
	printf("The directory does not exist.\n");
	return -1;
}
/*-----------------------------------------------------------------------------------------------------------------------------*/
void memcopy(void* arr[5])
{
	char*t1 = (char*)arr[0];
	char*t2 = (char*)arr[1];
	char*t3 = (char*)arr[2];
	char*t4 = (char*)arr[3];
	char*t5 = (char*)arr[4];

	arr[0] = malloc(1*4*1024); //1 block for super block
	arr[1] = malloc(1*4*1024); //1 block i-bmap block
	arr[2] = malloc(1*4*1024); //1 block d-bmap block
	arr[3] = malloc(gtfs_INODE_BLOCKS*4*1024); //gtfs_INODE_BLOCKS block inode block
	arr[4] = malloc(gtfs_NDIR_BLOCKS*4*1024); //gtfs_NDIR_BLOCKS blocks data block


	FILE* fp1 = fopen("data1.dat","w");

	for (int i = 0; i < 1*4*1024; ++i)
	{
		fputc(t1[i],fp1);
	}

	FILE* fp2 = fopen("data2.dat","w");

	for (int i = 0; i < 1*4*1024; ++i)
	{
		fputc(t2[i],fp2);
	}

	FILE* fp3 = fopen("data3.dat","w");

	for (int i = 0; i < 4*1024; ++i)
	{
		fputc(t3[i],fp3);
	}

	FILE* fp4 = fopen("data4.dat","w");

	for (int i = 0; i < gtfs_INODE_BLOCKS*4*1024; ++i)
	{
		fputc(t4[i],fp4);
	}

	FILE* fp5 = fopen("data5.dat","w");

	for (int i = 0; i < gtfs_NDIR_BLOCKS*4*1024; ++i)
	{
		fputc(t5[i],fp5);
	}

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	fclose(fp4);
	fclose(fp5);

}

void memaloc(void* arr[5])
{
	char* t1 = malloc(1*4*1024); //1 block for super block
	char* t2 = malloc(1*4*1024); //1 block i-bmap block
	char* t3 = malloc(1*4*1024); //1 block d-bmap block
	char* t4 = malloc(gtfs_INODE_BLOCKS*4*1024); //gtfs_INODE_BLOCKS block inode block
	char* t5 = malloc(gtfs_NDIR_BLOCKS*4*1024); //gtfs_NDIR_BLOCKS blocks data block

	FILE* fp1 = fopen("data1.dat","r");

	for (int i = 0; i < 1*4*1024; ++i)
	{
		t1[i] = fgetc(fp1);
	}

	FILE* fp2 = fopen("data2.dat","r");

	for (int i = 0; i < 1*4*1024; ++i)
	{
		t2[i] = fgetc(fp2);
	}

	FILE* fp3 = fopen("data3.dat","r");

	for (int i = 0; i < 4*1024; ++i)
	{
		t3[i] = fgetc(fp3);
	}

	FILE* fp4 = fopen("data4.dat","r");

	for (int i = 0; i < gtfs_INODE_BLOCKS*4*1024; ++i)
	{
		t4[i] = fgetc(fp4);
	}

	FILE* fp5 = fopen("data5.dat","r");

	for (int i = 0; i < gtfs_NDIR_BLOCKS*4*1024; ++i)
	{
		t5[i] = fgetc(fp5);
	}

	fclose(fp1);
	fclose(fp2);
	fclose(fp3);
	fclose(fp4);
	fclose(fp5);


	arr[0]= (void*)t1;
	arr[1]= (void*)t2;
	arr[2]= (void*)t3;
	arr[3]= (void*)t4;
	arr[4]= (void*)t5;


}


/*-----------------------------------------------------------------------------------------------------------------------------*/

int main()
{
	void*disk[5];
	FILE* fp1;
	if((fp1 = fopen("data1.dat","r"))==NULL)
	{
		gtfs_create_disk(disk);
	}
	else
	{
		memaloc(disk);
	}
	gtfs_inode* root = (gtfs_inode*)(disk[3]+2*(sizeof(gtfs_inode)));

	char dir[1000];
	int dirin = 0;

	dir[dirin++] = '\\';
	for(int i=0;i<strlen(root->name);i++)
	{
		dir[dirin++] = root->name[i];
	}
	dir[dirin]='\0';
	while(1)
	{
		char s[100];
		char op[4096];
		printf("\nGTFS:%s> ",dir);
		scanf("%s",s);
		if(strcmp("cd",s)==0)
		{
			scanf("%s",s);
			if(strcmp("..",s)==0)
			{
				if(root->i_no == 2) continue; //max back you can go is ROOT
				gtfs_inode* troot = getinode(disk,root->i_block[0]);
				if(troot!=NULL)
				{
					int len=strlen(root->name);
					for(int i=dirin-len-1;i<dirin;i++)
					{
						dir[i]='\0';
					}
					dirin-=len+1;
					root=troot;
				}
			}
			else
			{
				gtfs_inode* troot = gtfs_readdir(root,s,disk);
				if(troot!=NULL)
				{
					root = troot;
					dir[dirin++] = '\\';
					for(int i=0;i<strlen(root->name);i++)
					{
						dir[dirin++] = root->name[i];
					}
					dir[dirin]='\0';
				}
				else
				{
			    printf("No directory found with the given name.\n");
				}
			}
		}
		else if(strcmp("ls",s)==0)
		{
			gtfs_ls(root,disk);
		}
		else if(strcmp("cat",s)==0)
		{
			scanf("%s",s);
			gtfs_inode* fp = gtfs_open(s,1,root,disk);
			if(fp!=NULL)
			{
				gtfs_read(fp,op,disk);
				printf("%s\n",op);
			}
			else
			{
				printf("The file does not exist.\n");
			}
		}
		else if(strcmp("cp",s)==0)
		{
			scanf("%s",s);
			char temp[100];
			char op1[4096];
			gtfs_inode* fp = gtfs_open(s,1,root,disk);
			if(fp!=NULL)
			{
				gtfs_read(fp,op,disk);
				scanf("%s",s);
				gtfs_inode* fp1 = gtfs_open(s,2,root,disk);
				if(fp1!=NULL)
				{
					gtfs_write(fp1,op,disk,0);
					printf("Copied.\n");
				}
			}
			else
			{
				printf("The file does not exist.\n");
			}
		}
		else if(strcmp("touch",s)==0)
		{
			scanf("%s",s);
			gtfs_inode* fp = gtfs_open(s,2,root,disk);
		}
		else if(strcmp("write",s)==0)
		{
			scanf("%s",s);
			gtfs_inode* fp = gtfs_open(s,2,root,disk);
			printf("> ");
			scanf(" %[^\n]s",op);
			gtfs_write(fp,op,disk,0);
		}
		else if(strcmp("append",s)==0)
		{
			scanf("%s",s);
			gtfs_inode* fp = gtfs_open(s,2,root,disk);
			printf("> ");
			scanf(" %[^\n]s",op);
			gtfs_write(fp,op,disk,1);
		}
		else if(strcmp("mkdir",s)==0)
		{
			scanf("%s",s);
			gtfs_inode* dp = gtfs_readdir(root,s,disk);
			if(dp==NULL)
			{
				gtfs_mkdir(s,5,disk,root);
			}
			else
			{
				printf("File already exists.\n");
			}
		}
		else if(strcmp("rmdir",s)==0)
		{
			scanf("%s",s);
			gtfs_rmdir(disk,s,root);
		}
		else if(strcmp("rm",s)==0)
		{
			scanf("%s",s);
			gtfs_rm(disk,s,root);
		}
		else if(strcmp("stat",s)==0)
		{
			scanf("%s",s);
			gtfs_inode* fp = gtfs_open(s,1,root,disk);
			if(fp==NULL)
			{
				fp = gtfs_readdir(root,s,disk);
				if(fp==NULL)
				{
					printf("No such file/directory found.\n");
					continue;
				}
			}
			gtfs_stat(fp);
		}

		else if(strcmp("rename",s)==0)
		{
			scanf("%s",s);
			gtfs_inode* fp = gtfs_open(s,1,root,disk);
			if(fp==NULL)
			{
				fp = gtfs_readdir(root,s,disk);
				if(fp==NULL)
				{
					printf("No such file/directory found.\n");
					continue;
				}
			}
			scanf("%s",s);
			gtfs_rename(disk, fp, s);
		}
		else if(strcmp("closefs",s)==0)
		{
			break;
		}
		else
		{
			printf("Command not found.\n");
		}
	}
	memcopy(disk);
	return 0;
}
