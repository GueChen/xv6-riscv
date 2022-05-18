#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

const char* not_search_1 = ".";
const char* not_search_2 = "..";

//#define DEBUG

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), '\0', DIRSIZ-strlen(p));
  return buf;
}

void find(char* path, char* content)
{
    int fd;
    char buf[512], *p;
    struct dirent de; 
    struct stat st;
    
    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
#ifdef DEBUG
    printf("%d OPENED STAT WELL, TYPE %d INO %d SIZE %d\n", fd, st.type, st.ino, st.size);
#endif

    switch(st.type){
    case T_FILE:{
#ifdef DEBUG
        printf("NAME: %s TYPE: FILE\n", path);
#endif
        if(strcmp(fmtname(path), content) == 0){
          printf("%s\n", path);
        }
        break;
    }
    case T_DIR:{
#ifdef DEBUG
      printf("NAME: %s TYPE: DIR\n", path);
#endif
      if(strlen(path) + 1 + DIRSIZ + 1> sizeof buf){
        printf("find: path too long\n");
        break;
      }
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';
      while(read(fd, &de, sizeof(de)) == sizeof(de)){
        if(de.inum == 0)
          continue;
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        
        if(stat(buf, &st) < 0){
          printf("find: cannot stat %s\n", buf);
          continue;
        }

        switch (st.type)
        {
        case T_FILE:{
#ifdef DEBUG
          printf("NAME: %s TYPE: FILE\n", buf);
#endif
          if(strcmp(fmtname(buf), content) == 0){
            printf("%s\n", buf);
          }
          break;
        }        
        case T_DIR:{
#ifdef DEBUG
          printf("NAME: %s  TYPE: DIR\n", buf);
#endif
          if(strcmp(fmtname(buf), content) == 0){
            printf("%s\n", buf);
          }
          if(strcmp(fmtname(buf), not_search_1) != 0 && strcmp(fmtname(buf), not_search_2) != 0){
            //printf("ENTER %s FIND DEEPER\n", buf);
            find(buf, content);
          }
          break;
        }
        default:{
#ifdef DEBUG
          printf("NAME: %s UNKNOWN TYPE MAYBE CONSOLE\n", buf);
#endif
        }
        }
      }
    }}
    close(fd);
}

int main(int argc, char* argv[])
{
    if(argc < 3){
        printf("usage: find dirname<string> contentname<string>\n");
        exit(0);
    }
    find(argv[1], argv[2]);
    exit(0);
}