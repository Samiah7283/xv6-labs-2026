#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

char*
fmtname(char *path)
{
  char *p;

  // Find first character after last slash.
  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p;
}

void
run_exec(char **cmdargv, int cmdargc, char *file)
{
  int pid;

  if((pid = fork()) == 0){
    cmdargv[cmdargc] = file;
    cmdargv[cmdargc + 1] = 0;
    exec(cmdargv[0], cmdargv);
    fprintf(2, "find: exec %s failed\n", cmdargv[0]);
    exit(1);
  } else {
    wait(0);
  }
}

void
find(char *path, char *target, char **cmdargv, int cmdargc)
{
  char buf[512], *p;
  int fd;
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

  if(strcmp(fmtname(path), target) == 0){
    if(cmdargv)
      run_exec(cmdargv, cmdargc, path);
    else
      printf("%s\n", path);
  }

  switch(st.type){
  case T_FILE:
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      find(buf, target, cmdargv, cmdargc);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  char *cmdargv[MAXARG];
  int cmdargc = 0;
  int i;

  if(argc < 3){
    fprintf(2, "usage: find dir name [-exec cmd ...]\n");
    exit(1);
  }

  if(argc > 3){
    if(strcmp(argv[3], "-exec") != 0){
      fprintf(2, "usage: find dir name [-exec cmd ...]\n");
      exit(1);
    }
    for(i = 4; i < argc; i++){
      cmdargv[cmdargc++] = argv[i];
    }
    find(argv[1], argv[2], cmdargv, cmdargc);
  } else {
    find(argv[1], argv[2], 0, 0);
  }

  exit(0);
}
