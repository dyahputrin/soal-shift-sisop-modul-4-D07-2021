#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/putri/Downloads"

void checkDir(int num, char *pathDir) {
    int lengthPathDir = strlen(pathDir);

    for (int i = 0; i < lengthPathDir; i++) {

        if(lengthPathDir[i] == '/') return i+1;
    }
    return num;
}

void checkExt(char *pathDir) {
    int lengthPathDir = strlen(pathDir);

    for (int i = 0; i < lengthPathDir; i++) {
        
        if(lengthPathDir[i] == '.') return i;
    }
    return lengthPathDir;
}

void encryptAtoz(char *path) {
    if(!strcmp(path, ".") || !strcmp(path, "..")) return;

    int start = checkDir(path, 0);
    int end = checkExt(path);
    char ecryptPath;

    for (int i = start; i < end; i++) {
        
        while(path[i]!='\0') {
            if(!((path[i]>=0&&path[i]<65)||(path[i]>90&&path[i]<97)||(path[i]>122&&path[i]<=127))) {

                if(path[i]>='A'&&path[i]<='Z') {
                    ecryptPath = 'Z'+'A'-path[i];
                    //printf("%c", ecryptPath);
                }
               
               if(path[i]>='a'&&path[i]<='z') {
                    ecryptPath = 'z'+'a'-path[i];
                    //printf("%c", ecryptPath);
                }
               
            } 
                
            if(((path[i]>=0&&path[i]<65)||(path[i]>90&&path[i]<97)||(path[i]>122&&path[i]<=127))) {
                ecryptPath = path[i];
                //printf("%c", ecryptPath);
                   
            }
    
            i++;
        }

    }
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char fpath[1000];

    char *a = strstr(path, encv1);
    if(a != NULL) decrypt(a);
    if(strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        sprintf(fpath, "%s%s", dirpath, path);
    }
  
    res = lstat(fpath, stbuf);
  Finfo ("LS",fpath);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                       off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];
  
    char *a = strstr(path, encv1);
    if(a != NULL) decrypt(a);

    if(strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        sprintf(fpath, "%s%s", dirpath, path);
    }
    
    Finfo ("CD", fpath);
    int res = 0;
    DIR *dp;
    struct dirent *de;

    (void)offset;
    (void)fi;

    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;
    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if(de->d_type == DT_DIR){
            char temp1[1000];
            char *curr_sync = cekSync(de->d_name, temp1);
            
            if(curr_sync != NULL){
                char temp2[1000];
                sprintf(temp2, "%s/", fpath);
                char *first_path = strcat(temp2, de->d_name);

                permission_sync(curr_sync, first_path);
    
            }
        }

        if(a != NULL) encrypt(de->d_name);
        res = (filler(buf, de->d_name, &st, 0));

        if (res != 0) break;
    }

    closedir(dp);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
                    struct fuse_file_info *fi)
{
  
    char fpath[1000];
    char *a = strstr(path, encv1);
    if(a != NULL)
        decrypt(a);

    if(strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        sprintf(fpath, "%s%s", dirpath, path);
    }
 
    int res = 0;
    int fd = 0;

    (void)fi;

    fd = open(fpath, O_RDONLY);
      Finfo ("CAT", fpath);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    int res;
    char fpath[1000];
    char *a = strstr(path, encv1);

    if(a != NULL){
        decrypt(a);
    }

    if(strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        sprintf(fpath, "%s%s", dirpath, path);
    }
    char *temp = strrchr(fpath, '/');

    char *b = strstr(temp, syncron);

    if(b != NULL){
        char temp1[1000];
        char *first_path = cekSync(temp+1, temp1);

        if(first_path != NULL){
            char curr_sync[1000];
            sprintf(curr_sync, "%s", fpath);
            
            permission_sync(curr_sync, first_path);
        }
    }

    
    res = mkdir(fpath, mode);
Finfo ("MKDIR", fpath);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_rmdir(const char *path)
{
    int res;
    char fpath[1000];
    char *a = strstr(path, encv1);
    if(a != NULL)
        decrypt(a);

    if(strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else sprintf(fpath, "%s%s", dirpath, path);
    

    res = rmdir(fpath);
Fwarning ("RMDIR", fpath);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_rename(const char *from, const char *to)
{
    int res;
    char fpath[1000], path2[1000];
    char *a = strstr(to, encv1);
    if(a != NULL)
        decrypt(a);

    sprintf(fpath, "%s%s", dirpath, from);
    sprintf(path2, "%s%s", dirpath, to);

    

    res = rename(fpath, path2);

    if (res == -1)
        return -errno;
    Finfo ("RENAME", fpath);
    if (strstr(to, encv2) != NULL){
        encrypt2(path2);
    }

    if (strstr(from, encv2) != NULL && strstr(to, encv2) == NULL){
        decrypt2(path2);
    }

    return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
    int res;
    char fpath[1000];
    char *a = strstr(path, encv1);
    if (a != NULL)
        decrypt(a);

    if (strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        sprintf(fpath, "%s%s", dirpath, path);
    }

    res = truncate(fpath, size);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size,
                     off_t offset, struct fuse_file_info *fi)
{
    int fd;
    int res;
    char fpath[1000];
    char *a = strstr(path, encv1);
    if(a != NULL) decrypt(a);

    if(strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        sprintf(fpath, "%s%s", dirpath, path);
    }
    (void)fi;
    fd = open(fpath, O_WRONLY);
    if (fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);

    return res;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    (void)fi;

    int res;
    char fpath[1000];

    char *a = strstr(path, encv1);
    if(a != NULL)
        decrypt(a);

    if(strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        sprintf(fpath, "%s%s", dirpath, path);
    }
    
    res = creat(fpath, mode);
    Finfo ("CREATE", fpath);
    if (res == -1)
        return -errno;

    close(res);

    return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
    int res;
    char fpath[1000];

    char *a = strstr(path, encv1);
    if (a != NULL)
        decrypt(a);

    if (strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        sprintf(fpath, "%s%s", dirpath, path);
    }

    struct timeval tv[2];

    tv[0].tv_sec = ts[0].tv_sec;
    tv[0].tv_usec = ts[0].tv_nsec / 1000;
    tv[1].tv_sec = ts[1].tv_sec;
    tv[1].tv_usec = ts[1].tv_nsec / 1000;

    res = utimes(fpath, tv);
    if (res == -1)
        return -errno;

    return 0;
}

static int xmp_access(const char *path, int mask)
{
    int res;
    char fpath[1000];

    char *a = strstr(path, encv1);
    if(a != NULL)
        decrypt(a);

    if(strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        sprintf(fpath, "%s%s", dirpath, path);
    }

    res = access(fpath, mask);
    if (res == -1)
        return -errno;

    return 0;
}
static int xmp_open(const char *path, struct fuse_file_info *fi)
{
    int res;
    char fpath[1000];
    char *a = strstr(path, encv1);
    if(a != NULL)
        decrypt(a);

    if(strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        sprintf(fpath, "%s%s", dirpath, path);
    }

    res = open(fpath, fi->flags);
    if (res == -1)
        return -errno;

    close(res);
    return 0;
}


static int xmp_unlink(const char *path){
    int res;
    char fpath[1000];
    char *a = strstr(path, encv1);
    if(a != NULL)
        decrypt(a);

    if(strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    } else {
        sprintf(fpath, "%s%s", dirpath, path);
    }

  
    res = unlink(fpath);
  Fwarning ("REMOVE", fpath);
    if(res == -1){
        return -errno;
    }

    return 0;
}


static int xmp_readlink(const char *path, char *buf, size_t size)
{
    int res;
    char fpath[1000];

    char *a = strstr(path, encv1);
    if (a != NULL)
        decrypt(a);

    if (strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        sprintf(fpath, "%s%s", dirpath, path);
    }

    res = readlink(fpath, buf, size - 1);
    
    if (res == -1)
        return -errno;

    buf[res] = '\0';
    return 0;
}


static int xmp_mknod(const char *path, mode_t mode, dev_t rdev) {
    int res;
    char fpath[1000];

    char *a = strstr(path, encv1);
    if(a != NULL)
        decrypt(a);

    if(strcmp(path, "/") == 0){
        path = dirpath;
        sprintf(fpath, "%s", path);
    }else{
        sprintf(fpath, "%s%s", dirpath, path);
    }
    // 'mknod(path, mode, rdev)' more portable
    if (S_ISREG(mode)) {
        res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
        if (res >= 0)
            res = close(res);
    } else if (S_ISFIFO(mode))
        res = mkfifo(path, mode);
    else
        res = mknod(path, mode, rdev);

    if (res == -1)
        return -errno;

    return 0;
}

static  struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .access = xmp_access,
    .readlink = xmp_readlink,
    .readdir = xmp_readdir,
    .mknod = xmp_mknod,
    .mkdir = xmp_mkdir,
    .symlink = xmp_symlink,
    .unlink = xmp_unlink,
    .rmdir = xmp_rmdir,
    .rename = xmp_rename,
    .link = xmp_link,
    .chmod = xmp_chmod,
    .chown = xmp_chown,
    .truncate = xmp_truncate,
    .utimens = xmp_utimens,
    .open = xmp_open,
    .read = xmp_read,
    .write = xmp_write,
    .statfs = xmp_statfs,
    .create = xmp_create,
    .release = xmp_release,
    .fsync = xmp_fsync,
    .setxattr = xmp_setxattr,
    .getxattr = xmp_getxattr,
    .listxattr = xmp_listxattr,
    .removexattr = xmp_removexattr,
};

int main(int argc, char *argv[])
{
    umask(0);
    return fuse_main(argc, argv, &xmp_oper, NULL);
}