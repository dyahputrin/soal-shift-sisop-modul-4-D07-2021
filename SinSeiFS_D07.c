#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static  const  char *dirpath = "/home/putri/Downloads";

// nmr 2 b		
void vigenereCipher(char* plainText){
	char *k = "SISOP";
	int i;
	char cipher;
	int cipherValue;
	int len = strlen(k);
	
	//Loop through the length of the plain text string
	for(i=0; i<strlen(plainText); i++){
		
		//if the character is lowercase, where range is [97 -122]
		if(islower(plainText[i]))
		{
			cipherValue = ( (int)plainText[i]-97 + (int)tolower(k[i % len])-97 ) % 26 +97;
			cipher = (char)cipherValue;
		}
		else // Else it's upper case, where letter range is [65 - 90]
		{
			cipherValue = ( (int)plainText[i]-65 + (int)toupper(k[i % len])-65 ) % 26 +65;
			cipher = (char)cipherValue;
		}
	}
	
	
}

//no 2a
void encryptrot13(char *string) {
	printf("encrypt");
	if(!strcmp(string, ".") || !strcmp(string, "..")) return;
	
	char *token = strtok(string, ".");

	int end = strlen(token);

        for(int i = 0; i < end; i++) {
                if(string[i] >= 65 && string[i] <= 90) {
                        string[i] = string[i] + 13;
                        if(string[i] > 90) {
                                string[i] = string[i] - 90 + 65 - 1;
                        }
                } else if(string[i] >= 97 && string[i] <= 122) {
                        int j = (int)string[i];
                        j = j + 13;
                        if(j > 122) {
                                j = j - 122 + 97 - 1;
                        }
                        string[i] = j;
                } else {
                        string[i] = string[i];
                }
	
        }
}

void encryptAtoz(char *path) {
    if(!strcmp(path, ".") || !strcmp(path, "..")) return;

    //char temp[100];
    char *token = strtok(path, ".");

    int end = strlen(token);

    for (int i = 0; i < end; i++) {
        
        while(path[i]!='\0') {
            if(!((path[i]>=0&&path[i]<65)||(path[i]>90&&path[i]<97)||(path[i]>122&&path[i]<=127))) {

                if(path[i]>='A'&&path[i]<='Z') {
                    path[i] = 'Z'+'A'-path[i];
                    //printf("%c", ecryptPath);
                }
               
               if(path[i]>='a'&&path[i]<='z') {
                    path[i]  = 'z'+'a'-path[i];
                    //printf("%c", ecryptPath);
                }
               
            } 
                
            if(((path[i]>=0&&path[i]<65)||(path[i]>90&&path[i]<97)||(path[i]>122&&path[i]<=127))) {
                path[i]  = path[i];
                //printf("%c", ecryptPath);
                   
            }
    
            i++;
        }

    }  

}


void recursiveRename(char *fromPath) {
    DIR *dp;
    struct dirent *de;
    char file[100];
    char fileExt[100];
    char beforePath[1100];
    char afterPath[1100];

    dp = opendir(fromPath);

    if (dp == NULL) return;

    while ((de = readdir(dp)) != NULL) {
        if(strcmp(de->d_name, "..") == 0|| strcmp(de->d_name, ".") == 0) continue;
        if(de->d_type == DT_DIR) {
            strcpy(file, de->d_name);
            sprintf(beforePath, "%s/%s", fromPath, file);
            recursiveRename(beforePath);
        }
        //printf("de rename: %s\n", de->d_name);
        strcpy(file, de->d_name);
        strcpy(fileExt, file);
        sprintf(beforePath, "%s/%s", fromPath, file);
        //printf("ini file : %s\n", file);
        char *ext = strrchr(fileExt, '.');

        encryptAtoz(file);
        //printf("ini ext: %s\n", ext);
        /*
        if(type == 1)
            encryptAtoz(file);
        else if ( type == 2)
            encryptrot13(file);
        else if ( type == 3)
            vignereCipher(file);
         */
        

        if(ext != NULL) {
            sprintf(afterPath, "%s/%s%s", fromPath, file, ext);
            //printf("ini afterpath: %s\n", afterPath);
            rename(beforePath, afterPath);
        }

        else if(ext == NULL) {
            sprintf(afterPath, "%s/%s", fromPath, file);
            //printf("ini afterpath: %s\n", afterPath);
            rename(beforePath, afterPath);
        }

    }

    closedir(dp);

}

void logFileAtozMkdir(char *path) {
    FILE *f;
    f = fopen("atoz.log", "a+");
    //format nge-log yang mkdir tidak ditentukan
    fprintf(f, "mkdir: %s\n", path);
}

void logFileRXMkdir(char *path) {
    FILE *f;
    f = fopen("RX.log", "a+");
    //format nge-log yang mkdir tidak ditentukan
    fprintf(f, "mkdir: %s\n", path);
}

void logFileAtozRename(char *path1, char *path2) {
    FILE *f;
    f = fopen("atoz.log", "a+");
    fprintf(f, "%s-->%s\n", path1, path2);
}

void logFileAtozMkdir(char *path) {
    FILE *f;
    f = fopen("atoz.log", "a+");
    //format nge-log yang mkdir tidak ditentukan
    fprintf(f, "mkdir: %s\n", path);
}

static  int  xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];

    sprintf(fpath,"%s%s",dirpath,path);

    res = lstat(fpath, stbuf);

    if (res == -1) return -errno;

    return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    int res;
    char fpath[1000];

    sprintf(fpath,"%s%s",dirpath,path);

    if(strstr(fpath, "AtoZ_")) {
        logFileAtozMkdir(fpath);
    }
    // log 2c
    /*
	else if(strstr(fpath, "RX_")) {
        logFileRXMkdir(fpath);
    }
    */
    
    res = mkdir(fpath, mode);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_rename(const char *from, const char *to)
{
    int res;
    char fromPath[1000];
    char toPath[1000];

    sprintf(fromPath,"%s%s",dirpath,from);
    sprintf(toPath,"%s%s",dirpath,to);
    
    // atoz jadi normal decode
    if(strstr(fromPath, "AtoZ_")) {
        if(!strstr(toPath, "AtoZ_")) recursiveRename(fromPath);
    }
    // normal jadi atoz encode
    if(!strstr(fromPath, "AtoZ_")) {
        if(strstr(toPath, "AtoZ_")) recursiveRename(fromPath);
        //printf("%s-->%s\n", fromPath, toPath);
        logFileAtozRename(fromPath, toPath);
    }
    
    /*
    //if nya kutambahin 1 atoz 2 rx 
    
    // atoz jadi normal decode
    
    if(strstr(fromPath, "AtoZ_")) {
        if(!strstr(toPath, "AtoZ_")) recursiveRename(fromPath,1);
    }else if(strstr(fromPath, "RX_")) {
        if(!strstr(toPath, "RX_")) recursiveRename(fromPath,2);
	}
    // normal jadi atoz encode
    if(!strstr(fromPath, "AtoZ_")) {
        if(strstr(toPath, "AtoZ_")) recursiveRename(fromPath,1);
        //printf("%s-->%s\n", fromPath, toPath);
        logFileAtozRename(fromPath, toPath);
    } else if(!strstr(fromPath, "RX_")) {
        if(strstr(toPath, "RX_")) recursiveRename(fromPath,2);
        //printf("%s-->%s\n", fromPath, toPath);
        logFileRXRename(fromPath, toPath);
    }
    */

    //recursiveRename(fromPath);
    
    res = rename(fromPath, toPath);
    if (res == -1)
        return -errno;


    return 0;
}



static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];

    if(strcmp(path,"/") == 0)
    {
        path=dirpath;
        sprintf(fpath,"%s",path);
    } else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;

    DIR *dp;
    struct dirent *de;
    (void) offset;
    (void) fi;

    dp = opendir(fpath);

    if (dp == NULL) return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;

        memset(&st, 0, sizeof(st));

        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        res = (filler(buf, de->d_name, &st, 0));

        if(res!=0) break;
    }

    closedir(dp);

    return 0;
}



static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
    if(strcmp(path,"/") == 0)
    {
        path=dirpath;

        sprintf(fpath,"%s",path);
    }
    else sprintf(fpath, "%s%s",dirpath,path);

    int res = 0;
    int fd = 0 ;

    (void) fi;

    fd = open(fpath, O_RDONLY);

    if (fd == -1) return -errno;

    res = pread(fd, buf, size, offset);

    if (res == -1) res = -errno;

    close(fd);

    return res;
}



static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .mkdir = xmp_mkdir,
    .rename = xmp_rename,
    .readdir = xmp_readdir,
    .read = xmp_read,
};



int  main(int  argc, char *argv[])
{
    umask(0);

    return fuse_main(argc, argv, &xmp_oper, NULL);
}
