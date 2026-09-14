#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data, int len);

int
main(int argc, char *argv[])
{
  if (argc == 1) {
    printf("Example 1:\n");
    int a[2] = {61810, 2026};
    memdump("ii", (char *)a, sizeof(a));

    printf("Example 2:\n");
    memdump("S", "a string", sizeof("a string"));

    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *)&s, sizeof(s));

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;

    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "xyzzy");

    printf("Example 4:\n");
    memdump("pihcS", (char *)&example, sizeof(example));

    printf("Example 5:\n");
    memdump("sccccc", (char *)&example, sizeof(example));
  } else if (argc == 2) {
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while (n < sizeof(data)) {
      int nn = read(0, data + n, sizeof(data) - n);
      if (nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data, n);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data, int len)
{ int pos = 0;
  char *f;

  for(f = fmt; *f; f++){
    char spec = *f;

    if(spec == 'S'){
      int i;
      for(i = pos; i < len && data[i] != '\0'; i++)
        printf("%c", data[i]);
      printf("\n");
      pos = len;
      continue;
    }

    int need = 0;
    if(spec == 'i') need = 4;
    else if(spec == 'p') need = 8;
    else if(spec == 'h') need = 2;
    else if(spec == 'c') need = 1;
    else if(spec == 's') need = 8;
if(pos + need > len){
      printf("memdump: not enough data for '%c'\n", spec);
      return;
    }

    if(spec == 'i'){
      int v;
      memmove(&v, data + pos, 4);
      printf("%d\n", v);
    } else if(spec == 'p'){
      uint64 v;
      memmove(&v, data + pos, 8);
      int i;
      for(i = 60; i >= 0; i -= 4)
        printf("%x", (unsigned int)((v >> i) & 0xf));
      printf("\n");
    } else if(spec == 'h'){
      short v;
      memmove(&v, data + pos, 2);
      printf("%d\n", v);
    } else if(spec == 'c'){
      printf("%c\n", data[pos]);
    } else if(spec == 's'){
      char *strp;
      memmove(&strp, data + pos, 8);
      printf("%s\n", strp);
    }

    pos += need;
  }
 
}
