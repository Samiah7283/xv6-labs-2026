#include "kernel/types.h"
#include "user/user.h"

char *sep = " -\r\t\n./,";

int
is_sep(char c)
{
  char *s;
  for(s = sep; *s; s++){
    if(*s == c)
      return 1;
  }
  return 0;
}

void
sixfive(int fd)
{
  char c;
  int num = 0;
  int digit_count = 0;
  int valid = 1; 

  while(read(fd, &c, 1) == 1){
    if(is_sep(c)){
      if(digit_count > 0 && valid && (num % 5 == 0 || num % 6 == 0))
        printf("%d\n", num);
      num = 0;
      digit_count = 0;
      valid = 1;
    } else if(c >= '0' && c <= '9'){
      num = num * 10 + (c - '0');
      digit_count++;
    } else {
      valid = 0;
    }
  }

  if(digit_count > 0 && valid && (num % 5 == 0 || num % 6 == 0))
    printf("%d\n", num);
}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc <= 1){
    sixfive(0);
    exit(0);
  }

  for(i = 1; i < argc; i++){
    fd = open(argv[i], 0);
    if(fd < 0){
      fprintf(2, "sixfive: cannot open %s\n", argv[i]);
      continue;
    }
    sixfive(fd);
    close(fd);
  }

  exit(0);
}
