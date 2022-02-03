#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"


void
find(char *path, char* target_file)
{
    int fd;
    struct dirent de;
    struct stat st;
    char buf[512];
    char *p;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      return;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    
	// read the name of each file/folder under the folder specified by fd, which is $path, name is de.name
    while(read(fd, &de, sizeof(de) == sizeof(de))){
        if(de.inum == 0)
          continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if(stat(buf, &st) < 0){
          printf("find: cannot stat %s\n", buf);
          continue;
        }
        switch(st.type){
            case T_FILE:
                if(strcmp(target_file, de.name) == 0){
                    printf("%s\n", buf);
                }
                break;
            case T_DIR:
                if(strcmp(de.name, ".") != 0 && strcmp(de.name, "..") !=0){
                    find(buf, target_file);
                }
        }
    }
    close(fd);
    return;
}

int
main(int argc, char *argv[])
{
    if (argc < 2){
        fprintf(2, "Usage: find [dir] [target_file...]");
        exit(1);
    }
    for(int i = 1; i < argc; i++){
        find(argv[0], argv[i]);
    }
    exit(0);
}
