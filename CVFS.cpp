# Custmised-Virtual-File-System

/*-----------------------  Declaration -------------------------*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<io.h>
#include<iostream>


#define MAXINODE 50

#define  READ 1
#define WRITE 2

#define MAXFILESIZE 1024

#define REGULAR 1
#define SPECIAL 2

#define START 0
#define CURRENT 1
#define END 2



typedef struct superblock
{
	int TotalInodes;
	int FreeInodes;
}SUPERBLOCK, *PSUPEERBLOCK;

typedef struct inode
{
	char FileName[50];
	int InodeNumber;
	int FileSize;
	int FileActualSize;
	int FileType;
	char *Buffer;
	int LinkCount;
	int ReferenceCount;
	int Permission;
	struct inode * next;
}INODE, *PINODE, **PPINODE;

typedef struct filetable
{
	int readoffset;
	int writeoffset;
	int count;
	int mode;
	PINODE ptrinode;
}FILETABLE, *PFILETABLE;

typedef struct ufdt
{
	PFILETABLE ptrfiletable;
}UFDT;

UFDT UFDTArr[50];
SUPERBLOCK SUPERBLOCKobj;
PINODE head = NULL;

/*-----------------------------Data Structure Initialization--------------------------*/

void createDILB()
{
	PINODE temp = head;
	PINODE newn = NULL;
	int icnt = 1;

	while (icnt <= MAXINODE)
	{
		newn = (PINODE)malloc(sizeof(INODE));
		
		newn->LinkCount = 0;
		newn->ReferenceCount = 0;
		newn->FileType = 0;
		newn->FileSize = 0;
		
		newn->Buffer = NULL;
		newn->next = NULL;
		
		newn->InodeNumber = icnt;

		if (head == NULL)
		{
			head = temp = newn;
		}
		else
		{
			temp->next = newn;
			temp = temp->next;
		}
		icnt++;
	}
	printf("DILB successfully created\n");
}

void InitializeSuperBlock()
{
	int i = 0;
	while (i < MAXINODE)
	{
		UFDTArr[i].ptrfiletable = NULL;
		i++;
	}
	SUPERBLOCKobj.FreeInodes = MAXINODE;
	SUPERBLOCKobj.TotalInodes = MAXINODE;
	printf("Super Block initialized successfully\n");
}

/*--------------------------- System Function -------------------------------*/

int GetFDFromName(char *name)
{
	int i = 0;

	while (i<50)
	{
		if (UFDTArr[i].ptrfiletable != NULL)
			if (strcmp((UFDTArr[i].ptrfiletable->ptrinode->FileName), name) == 0)
				break;
		i++;
	}
	if (i == 50)
		return -1;
	else
		return i;
}

PINODE Get_Inode(char *name)
{
	PINODE temp = head;
	int i = 0;

	if (name == NULL)
		return NULL;

	while (temp != NULL)
	{
		if (strcmp(name, temp->FileName) == 0)
			break;
		temp = temp->next;
	}
	return temp;
}

void man(char *name)      //man command
{
	if (name == NULL)
		return;
	else if (strcmp(name, "create") == 0)
	{
		printf("Description : Used to create new file\n");
		printf("Usages : create File_Name Permission\n");
	}
	else if (strcmp(name, "read") == 0)
	{
		printf("Description : Used to open file for reding\n");
		printf("Usages : read File_Name File_Type \n");
	}
	else if (strcmp(name, "write") == 0)
	{
		printf("Description : Used to open file for writing\n");
		printf("Usages : write File_Name \n");
	}
	else if (strcmp(name, "open") == 0)
	{
		printf("Description : Used to open file\n");
		printf("Usages : create File_Name Permission\n");
	}
	else if (strcmp(name, "ls") == 0)
	{
		printf("Description : Used to display list of files\n");
		printf("Usages : ls\n");
	}
	else if (strcmp(name, "stat") == 0)
	{
		printf("Description : Used to display statistics of file\n");
		printf("Usages : stat File_Name \n");
	}
	else if (strcmp(name, "fstat") == 0)
	{
		printf("Description : Used to display contents of file\n");
		printf("Usages : fstat File_Name \n");
	}
	else if (strcmp(name, "close") == 0)
	{
		printf("Description : Used to close file\n");
		printf("Usages : close File_Name \n");
	}
	else if (strcmp(name, "closeall") == 0)
	{
		printf("Description : Used to close all file\n");
		printf("Usages : closeall File_Name \n");
	}
	else if (strcmp(name, "rm") == 0)
	{
		printf("Description : Used to delete file\n");
		printf("Usages : rm File_Name \n");
	}
	else
	{
		printf("ERROR : No manual entry available\n");
	}
}


void DisplayHelp()   //help  command
{
	printf("ls : To list out all the files\n");
	printf("clear : To clear console\n");
	printf("open : To open file\n");
	printf("close : To close the file \n");
	printf("closeall : To close all open file\n");
	printf("read : To open the file for reading purpose\n");
	printf("write : To open file for writing purpose\n");
	printf("exit : To terminate the console\n");
	printf("stat : To display statistics of file\n");
	printf("fstst : To list out all information about file\n");
	printf("turncate : To shrink the size of file to desired size\n");
	printf("rm : To delete the file\n");
}
void ls_file()   //ls command
{
	int i = 0;
	PINODE temp = head;
	
	if (SUPERBLOCKobj.FreeInodes == MAXINODE)
	{
		printf("Error : There is no file");
		return;
	}
	
	printf("\nFile Name\t|Inode Number\t|File Size\t|Link Count\n");
	printf("--------------------------------------------------------------");
	while (temp != NULL)
	{
		if(temp->FileType != 0)
		{
			printf("\n%s\t\t|%d\t\t|%d\t\t|%d\n",temp->FileName,temp->InodeNumber,temp->FileActualSize,temp->LinkCount);
		}
		temp = temp->next;
	}
    printf("--------------------------------------------------------------");
}

int fstat_file(int fd)   //fstat command
{
	PINODE temp = head;
	int i = 0;
	
	if(fd < 0) 
	  return -1;
	
	if(UFDTArr[fd].ptrfiletable == NULL) 
	  return -2;
	
	temp = UFDTArr[fd].ptrfiletable->ptrinode;
	
	printf("\n--------------------Statistical Information about file------------------------\n");
	printf("\nFile Name:%s\n",temp->FileName);
	printf("\nInode Number:%d\n",temp->InodeNumber);
	printf("\nFile Size:%d\n",temp->FileSize);
	printf("\nActual File Size:%d\n",temp->FileActualSize);
	printf("\nLink Count:%d\n",temp->LinkCount);
	printf("\nReference Count:%d\n",temp->ReferenceCount);
	
	if(temp->Permission == 1)
	   printf("\nFile Permission : READ ONLY");
	else if(temp->Permission == 2)
	   printf("\nFile Permission : WRITE ONLY");
	if(temp->Permission == 3)
	   printf("\nFile Permission : READ AND WRITE");
	
	printf("\n-------------------------------------------------------------------------------\n");
	
	return 0;
}

int stat_file(char *name)   //stat command
{
	PINODE temp = head;
	int i = 0;
	
	if(name == NULL) 
	  return -1;
	  
	while(temp != NULL)
	{
		if(strcmp(name,temp->FileName) == 0)
		  break;
		temp = temp->next;
	}
	
	if(temp == NULL)
	  return -2;
	
	printf("\n--------------------Statistical Information about file------------------------\n");
	printf("\nFile Name:%s\n",temp->FileName);
	printf("\nInode Number:%d\n",temp->InodeNumber);
	printf("\nFile Size:%d\n",temp->FileSize);
	printf("\nActual File Size:%d\n",temp->FileActualSize);
	printf("\nLink Count:%d\n",temp->LinkCount);
	printf("\nReference Count:%d\n",temp->ReferenceCount);
	
	if(temp->Permission == 1)
	   printf("\nFile Permission : READ ONLY");
	else if(temp->Permission == 2)
	   printf("\nFile Permission : WRITE ONLY");
	if(temp->Permission == 3)
	   printf("\nFile Permission : READ AND WRITE");
	
	printf("\n-------------------------------------------------------------------------------\n");
	
	return 0;
}


void exit_x()            //exit terminal command
{
	PINODE temp = head;
	int i = 0;
	while (temp != NULL)
	{
		head = head->next;
		free(temp->Buffer);
		free(temp);
		temp = head;
	}
	while(UFDTArr[i].ptrfiletable != NULL)
	{
		free(UFDTArr[i].ptrfiletable);
		i++;
	}
}


int createFile(char *name, int permission)    //create file command
{
	int i = 0;
	PINODE temp = head;

	if (name == NULL || permission == 0 || permission > 3)
	{
		return -1;
	}

	if (SUPERBLOCKobj.FreeInodes == 0)
	{
		return -2;
	}

	if(Get_Inode(name) != NULL)
	   return -3;
	   
	while(temp != NULL)
	{
		if(temp->FileType == 0)
		  break;
		temp = temp->next;
	}
	
	while(i < 50)
	{
		if(UFDTArr[i].ptrfiletable == NULL)
		 break;
		i++;
	}
	
	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
	
    UFDTArr[i].ptrfiletable->count = 1;	
    UFDTArr[i].ptrfiletable->mode = permission;
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    
    UFDTArr[i].ptrfiletable->ptrinode = temp;
    
    strcpy(UFDTArr[i].ptrfiletable->ptrinode->FileName,name);
	UFDTArr[i].ptrfiletable->ptrinode->FileType = REGULAR;
	UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount = 1;
	UFDTArr[i].ptrfiletable->ptrinode->LinkCount = 1;
	UFDTArr[i].ptrfiletable->ptrinode->FileSize = MAXFILESIZE;
	UFDTArr[i].ptrfiletable->ptrinode->FileActualSize = 0;
	UFDTArr[i].ptrfiletable->ptrinode->Permission = permission;
	UFDTArr[i].ptrfiletable->ptrinode->Buffer = (char *)malloc(MAXFILESIZE);

	(SUPERBLOCKobj.FreeInodes)--;
	
		
	return i;	
}

int rm_File(char * name)   //rm command
{
	int fd = 0;
	
	fd = GetFDFromName(name);
	if(fd == -1)
	  return -1;
	  
	(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount)--;
	
	if(UFDTArr[fd].ptrfiletable->ptrinode->LinkCount == 0)
	{
		UFDTArr[fd].ptrfiletable->ptrinode->FileType = 0;
		free(UFDTArr[fd].ptrfiletable->ptrinode->Buffer);
		free(UFDTArr[fd].ptrfiletable);
	}
	
	UFDTArr[fd].ptrfiletable = NULL;
	(SUPERBLOCKobj.FreeInodes)++;
}

int ReadFile(int fd, char *arr, int isize) //Read File Command
{
	int read_size = 0;
	
	if(UFDTArr[fd].ptrfiletable == NULL)
	 return -1;
	if(UFDTArr[fd].ptrfiletable->ptrinode->Permission != READ && UFDTArr[fd].ptrfiletable->ptrinode->Permission != READ+WRITE)
	return -2;
	if(UFDTArr[fd].ptrfiletable->mode != READ && UFDTArr[fd].ptrfiletable->mode != READ+WRITE)
	return -2;
	if(UFDTArr[fd].ptrfiletable->readoffset == UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)
	return -3;
	if(UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
	return -4;
	
	read_size = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) - (UFDTArr[fd].ptrfiletable->readoffset);
    
    if(read_size < isize)
    {
    	strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),read_size);
    	
    	(UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset)+read_size;
	}
	else
    {
    	strncpy(arr,(UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->readoffset),isize);
    	
    	(UFDTArr[fd].ptrfiletable->readoffset) = (UFDTArr[fd].ptrfiletable->readoffset)+isize;
	}
	return isize;
}

int WriteFile(int fd, char *arr, int isize)  //Write file command
{
	
	if(UFDTArr[fd].ptrfiletable->ptrinode->Permission != WRITE && UFDTArr[fd].ptrfiletable->ptrinode->Permission != READ+WRITE)
	 return -1;
	
	if(UFDTArr[fd].ptrfiletable->mode != WRITE && UFDTArr[fd].ptrfiletable->mode != READ+WRITE)
	 return -1;
	
	if(UFDTArr[fd].ptrfiletable->writeoffset == MAXFILESIZE)
	 return -2;
	
	if(UFDTArr[fd].ptrfiletable->ptrinode->FileType != REGULAR)
	 return -3;
	
    strncpy((UFDTArr[fd].ptrfiletable->ptrinode->Buffer)+(UFDTArr[fd].ptrfiletable->writeoffset),arr,isize);
    	
    (UFDTArr[fd].ptrfiletable->writeoffset) = (UFDTArr[fd].ptrfiletable->readoffset)+isize;

    (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize) = (UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize)+isize;
	
	return isize;
}

int OpenFile(char *name, int mode)
{
	int i = 0;
	PINODE temp = NULL;
	
	if(name == NULL || mode <= 0)
	  return -1;
	  
	temp = Get_Inode(name);
	if(temp == NULL)
	  return -2;
	  
	if(temp->Permission < mode)
	  return -3;
	  
	while(i < 50)
	{
		if(UFDTArr[i].ptrfiletable == NULL)
		  break;
		i++;
	}
	
	UFDTArr[i].ptrfiletable = (PFILETABLE)malloc(sizeof(FILETABLE));
	if(UFDTArr[i].ptrfiletable == NULL)  return -1;
	UFDTArr[i].ptrfiletable->count = 1;
	UFDTArr[i].ptrfiletable->mode = mode;
	
	if(mode == READ+WRITE)
	{
		UFDTArr[i].ptrfiletable->readoffset = 0;
		UFDTArr[i].ptrfiletable->writeoffset = 0;
	}
	else if(mode == READ)
	{
		UFDTArr[i].ptrfiletable->readoffset = 0;
	}
    else if(mode == WRITE)
	{
	    UFDTArr[i].ptrfiletable->writeoffset = 0;
	}
	
	UFDTArr[i].ptrfiletable->ptrinode = temp;
	(UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)++;
	
	return i;
}

void CloseFile(int fd)
{
    UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    (UFDTArr[fd].ptrfiletable->ptrinode->ReferenceCount)--;
}

int CloseFileByName(char *name)
{
	int i = 0;
	i = GetFDFromName(name);
	
	if(i == -1)
	  return -1;
	  
	
    UFDTArr[i].ptrfiletable->readoffset = 0;
    UFDTArr[i].ptrfiletable->writeoffset = 0;
    (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
    
    return 0;
}

void CloseAllFile()
{
	int i = 0;
	
	while(i<50)
    {
       if(UFDTArr[i].ptrfiletable != NULL)
       {
          UFDTArr[i].ptrfiletable->readoffset = 0;
          UFDTArr[i].ptrfiletable->writeoffset = 0;
          (UFDTArr[i].ptrfiletable->ptrinode->ReferenceCount)--;
        }
        i++;
        
    }
}

int truncate_File(char *name)
{
	int fd = GetFDFromName(name);
	
	if(fd == -1) return -1;
	
	memset(UFDTArr[fd].ptrfiletable->ptrinode->Buffer,0,1024);
	UFDTArr[fd].ptrfiletable->readoffset = 0;
    UFDTArr[fd].ptrfiletable->writeoffset = 0;
    UFDTArr[fd].ptrfiletable->ptrinode->FileActualSize = 0;
}

/*int LseekFile(int fd,int size,int from)
{
	if((fd<0) || (from>2)) return -1;
	if(UFDTArr[fd].ptrfiletable == NULL) return -1;
	
	if((UFDTArr[fd].ptrfiletable->mode == READ) || (UFDTArr[fd].ptrfiletable->mode == READ+WRITE))
	{
		if(from == CURRENT)
		{
			if((()))
		}
	}
}*/


/*-------------------------- Main Function----------------------------*/

int main()
{
	char * ptr = NULL;
	int ret = 0, fd = 0, cnt = 0;
	char command[4][80], str[80], arr[1024];

	InitializeSuperBlock();
	createDILB();

	while (1)
	{
		fflush(stdin);
		strcpy(str, "");

		printf("\nCDFileSystem #>");
		fgets(str, 80, stdin);

		cnt = sscanf(str, "%s %s %s %s", command[0], command[1], command[2], command[3]);

		if (cnt == 1)
		{
			if (strcmp(command[0], "ls") == 0)
			{
				ls_file();
			}
			else if (strcmp(command[0], "closeall") == 0)
			{
                CloseAllFile();
                printf("All files closed successfully");
                continue;
			}
			else if (strcmp(command[0], "clear") == 0)
			{
				system("cls");
				continue;
			}
			else if (strcmp(command[0], "help") == 0)
			{
				DisplayHelp();
				continue;
			}
			else if (strcmp(command[0], "exit") == 0)
			{
				printf("\nTerminating Customized Dynamic File System\n");
				exit_x();
				break;
			}
			else
			{
				printf("\nERROR : Command not found !!!\n");
				continue;
			}
		}
		else if (cnt == 2)
		{
			if (strcmp(command[0], "stat") == 0)
			{
				ret = stat_file(command[1]);
                if(ret == -1)
                   printf("ERROR  : Incorrect Parameters\n");
                if(ret == -2)
                   printf("ERROR  : There is no such a file\n");
                continue;
			}
			else if (strcmp(command[0], "fstat") == 0)
			{
                ret = fstat_file(atoi(command[1]));
                if(ret == -1)
                   printf("ERROR  : Incorrect Parameters\n");
                if(ret == -2)
                   printf("ERROR  : There is no such a file\n");
                continue;
			}
			else if (strcmp(command[0], "close") == 0)
			{
                 ret = CloseFileByName(command[1]);
                if(ret == -1)
                   printf("ERROR  : There is no such a file\n");
                continue;
			}
			else if (strcmp(command[0], "rm") == 0)
			{
                rm_File(command[1]);
			}
			else if (strcmp(command[0], "man") == 0)
			{
				man(command[1]);
			}
			else if (strcmp(command[0], "write") == 0)
			{
                fd = GetFDFromName(command[1]);
                if(fd == -1)
                {
                	printf("ERROR : Incorrect Parameter\n");
                	continue;
				}
				printf("Enter the Data : \n");
				scanf("%[^\n]s",arr);
				
				ret = strlen(arr);
				if(ret == 0)
				{
					printf("ERROR : Incorrect Parameter\n");
                	continue;
				}
				
				ret = WriteFile(fd,arr,ret);
				if(ret == -1)
				  printf("ERROR : Permission Denied\n");
				if(ret == -2)
				  printf("ERROR : There is no sufficient Memory\n");
				if(ret == -1)
				  printf("ERROR : It's not Regular file'\n");
			}
			else if (strcmp(command[0], "truncate") == 0)
			{
                ret = truncate_File(command[1]);
                if(ret == -1)
                   printf("ERROR  : There is no such a file\n");
                
			}
			else
			{
				printf("\nERROR : Command not found !!!\n");
				continue;
			}
		}
		else if (cnt == 3)
		{
			if (strcmp(command[0], "create") == 0)
			{
				ret = createFile(command[1], atoi(command[2]));
				if (ret >= 0)
					printf("File is successfully created with file descriptor : %d\n", ret);
				if (ret == -1)
					printf("ERROR : Incorrect Parameters\n");
				if (ret == -2)
					printf("ERROR : There no inode\n");
				if (ret == -3)
					printf("ERROR : Incorrect File already exists\n");
				if (ret == -4)
					printf("ERROR : Memory allocation failure\n");
				continue;
			}
			else if (strcmp(command[0], "open") == 0)
			{
               ret = OpenFile(command[1], atoi(command[2]));
				if (ret >= 0)
					printf("File is successfully created with file descriptor : %d\n", ret);
				if (ret == -1)
					printf("ERROR : Incorrect Parameters\n");
				if (ret == -2)
					printf("ERROR : File not present\n");
				if (ret == -3)
					printf("ERROR : Permission denied\n");
				continue;
			}
			else if (strcmp(command[0], "read") == 0)
			{
				fd = GetFDFromName(command[1]);
                if(fd == -1)
                {
                	printf("ERROR : Incorrect Parameter\n");
                	continue;
				}
				
				ptr = (char *)malloc(sizeof(atoi(command[2]))+1);
				if(ptr == NULL)
				{
					printf("ERROR : Memory allocation failure\n");
				    continue;
				}

                ret = ReadFile(fd,ptr,atoi(command[2]));
				if (ret == 0)
					printf("ERROR : File is empty\n");
				if (ret == -1)
					printf("ERROR : File not existing\n");
				if (ret == -2)
					printf("ERROR : permission denied\n");
				if (ret == -3)
					printf("ERROR : Reached at End of file\n");
				if (ret == -4)
					printf("ERROR : It's not regular file'\n");
				if(ret > 0)
				{
					write(2,ptr,ret);
				}
				
				continue;
			}
			else
			{
				printf("\nERROR : Command not found !!!\n");
				continue;
			}
		}
		else if (cnt == 4)
		{
			if (strcmp(command[0], "lseek") == 0)
			{

			}
			else
			{
				printf("\nERROR : Command not found !!!\n");
				continue;
			}
		}
		else
		{
			printf("\nERROR : Command not found !!!\n");
			continue;
		}
	}

	return 0;
}
