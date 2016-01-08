#include <fcntl.h> 
#include <stdio.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <unistd.h> 
#include <string>

using namespace std;

int main (int argc, char* argv[]) 
{
  printf ("Hola");
  unsigned char buffer[16]; 
  size_t offset = 0; 
  size_t bytes_read; 
  int i; 
 
  /* Open the file for reading.  */ 
  int fd = open ("/dev/cu.usbmodem1411", O_RDONLY && F_SETFL); 
  printf ("Adeu");
  /* Read from the file, one chunk at a time. Continue until read 
     "comes up short", that is, reads less than we asked for. 
     This indicates that we've hit the end of the file.  */ 
  while(true){
  do {
    /* Read the next line's worth of bytes.  */ 
    //bytes_read = read (fd, buffer, sizeof (buffer)); 
    /* Print the offset in the file, followed by the bytes themselves.  */ 
    //printf ("0x%06x : ", offset); 
//dona'm valors
    int pos = 0;

    while( pos < 99 ) {
      read(fd, buffer+pos, 1); 
      printf("%c",buffer[pos]);         // Note you should be checking the result
      if( buffer[pos] == '$' ){
        printf("\n");
      }
      pos++;
    }
    //printf("%s",buffer); 

    /* Keep count of our position in the file.  */ 
    offset += bytes_read; 
  } 
  while (bytes_read == sizeof (buffer)); 
 }
  /* All done.  */ 
  close (fd); 
  return 0; 
} 