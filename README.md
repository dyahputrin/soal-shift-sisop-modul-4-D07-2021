# soal-shift-sisop-modul-4-D07-2021

----------------------------------------

### Soal 1
Pada soal nomor 1, kita diminta untuk melakukan encoding nama dari directory dan files dengan menggunakan Atbash cipher (mirror). Mount source (root) filesystem adalah directory /home/[user]/Downloads.
Untuk mengerjakan soal 1, perlu dibuat sebuah fungsi untuk melakukan encoding.
```
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
```
Pada potongan kode diatas, terdapat function ```encryptAtoz```, pada awal dari function ini kita perlu memisahkan nama sebuah file dari extensionnya. Oleh karena itu digunakan ```strtok``` dengan delimiter ```.```. Setelah nama file diambil, kita dapat melakukan for looping pada setiap character nya untuk di-encrypt.

### A : Jika sebuah direktori dibuat dengan awalan "AtoZ_", maka direktori tersebut akan menjadi sebuah direktori ter-encode
```
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

    char desc[1500];
    sprintf(desc, "MKDIR::%s", fpath);
    logInfo(desc);


    return 0;
}

```
Dengan asumsi apabila sebuah directory dibuat dengan awalan "AtoZ_", maka directory tersebut menjadi directory yang telah ter-encode. Sehingga, apabila terdapat folder atau file di dalamnya, folder atau file tersebut akan ter-decode ketika kita melakukan renaming (akan dibahas pada sectio 1B). Pada function xmp_mkdir, kita juga melakukan checking apabila sebuah directory dibuat menggunakan awalan "AtoZ_" dengan ```strstr```, maka kita akan memanggil fungsi ```logFileAtozMkdir``` untuk menyimpan log.

### B : Jika sebuah direktori di-rename dengan awalan "AtoZ_", maka direktori tersebut akan menjadi direktori ter-encode
```
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

    char desc[2500];
    sprintf(desc, "RENAME::%s::%s", fromPath, toPath);
    logInfo(desc);


    return 0;
}

```
Pada ```xmp_rename``` kita perlu melakukan checking apabila sebuah direktori mengandung "AtoZ_" dengan ```strstr```. Apabila sebuah direktori mengandung "AtoZ_", maka kita perlu memanggil function recursiveRename2.

### C : Apabila direktori yang terenkripsi di re-name menjadi tidak ter-encode, maka isi direktori tersebut akan ter-decode
Pada function xmp_rename, apabila sebuah direktori di-rename tanpa mengandung "AtoZ_" maka direktori tersebut akan ter-decode. Proses decoding nya dapat dilakukan pula dengan memanggil function recursiveRename2.

### D : Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log
```
void logFileAtozRename(char *path1, char *path2) {
    FILE *f;
    f = fopen("atoz.log", "a+");
    fprintf(f, "%s-->%s\n", path1, path2);
}
```
```
void logFileAtozMkdir(char *path) {
    FILE *f;
    f = fopen("atoz.log", "a+");
    //format nge-log yang mkdir tidak ditentukan
    fprintf(f, "mkdir: %s\n", path);
}
```
Untuk mencatat log, dibuat fungsi terlebih dahulu bernama ```logFileAtozRename``` dan ```logFileAtozMkdir``` untuk diimplementasikan pada ```xmp_rename``` dan ```xmp_mkdir```.

### E : Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)
```
void recursiveRename(char *fromPath, int type) {
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
            recursiveRename(beforePath, type);
        }
        //printf("de rename: %s\n", de->d_name);
        strcpy(file, de->d_name);
        strcpy(fileExt, file);
        sprintf(beforePath, "%s/%s", fromPath, file);
        //printf("ini file : %s\n", file);
        char *ext = strrchr(fileExt, '.');
	puts(file);
	
	if(type == 1)
		encryptAtoz(file);
	else if ( type == 2)
		encryptrot13(file);
	else if ( type == 3)
		vignereCipher(file);
        //printf("ini ext: %s\n", ext);
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
```
Pada function recursiveRename, untuk melakukan recursive pada direktori-direktori di dalamnya dapat dilakukan dengan while loop selama directory != NULL. Proses tersebut berlangsung secara recursive terus menerus dengan cara memanggil function recursiveRename.

### Kendala
Pada saat melakukan renaming file, terdapat sedikit kendala pada recursive directory nya.

---------------------------------------------

### Soal 4
Pada soal nomor 4, kita perlu mencatat kegiatan pada filesystem pada file SinSeiFS.log pada direktori home user. Log ini dibuat menjadi dua level yaitu, WARNING untuk mencatat syscall rmdir dan unlink dan INFO untuk mencatat syscall sisanya.
```
void logInfo(char *desc) {
    time_t waktu = time(NULL);
    struct wt wt = *localtime(&waktu);
    FILE* fLog = fopen(logpath, "a");
    fprintf(fLog, "INFO::%02d%02d20%02d-%02d:%02d:%02d::%s\n", wt.wt_mday, wt.wt_mon, (wt.wt_year + 1900) % 100, wt.wt_hour, wt.wt_min, wt.wt_sec, desc);
    fclose(fLog);
}

void logWarning(char *desc) {
    time_t waktu = time(NULL);
    struct wt wt = *localtime(&waktu);
    FILE* fLog = fopen(logpath, "a");
    fprintf(fLog, "WARNING::%02d%02d20%02d-%02d:%02d:%02d::%s\n", wt.wt_mday, wt.wt_mon, (wt.wt_year + 1900) % 100, wt.wt_hour, wt.wt_min, wt.wt_sec, desc);
    fclose(fLog);
}
```
Untuk melakukan pencatatan log, kita membuat dua function yaitu ```logInfo``` dan ```logWarning```. Kedua function memiliki cara kerja yang sama dimana kita menggunakan variabel waktu dengan type time_t dan struct wt wt untuk mengambil localtime. Setelah itu, melakukan ```fprintf``` sesuai format yang telah disediakan soal.
