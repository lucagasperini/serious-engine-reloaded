#include "stdio.h"
#include "stdlib.h"
#include "math.h"

/*
   The following is rather crude demonstration code to read 
   uncompressed and compressed TGA files of 16, 24, or 32 bit 
   TGA. 
   Hope it is helpful.
*/

typedef signed long int SLONG;
typedef signed short int SWORD;
typedef signed char  SBYTE;
typedef signed int SINT;

typedef unsigned long int ULONG;
typedef unsigned short int UWORD;
typedef unsigned char  UBYTE;
typedef unsigned int UINT;


#define TARGA_HEADER_BYTE_SIZE 18

struct HEADER {
   UBYTE idlength;
   UBYTE colourmaptype;
   UBYTE datatypecode;
   UWORD colourmaporigin;
   UWORD colourmaplength;
   UBYTE colourmapdepth;
   UWORD x_origin;
   UWORD y_origin;
   UWORD width;
   UWORD height;
   UBYTE bitsperpixel;
   UBYTE imagedescriptor;
} __attribute__((packed));

int main(int argc,char **argv)
{
   int n=0,i,j;
   int bytes2read,skipover = 0;
   unsigned char p[5];
   FILE* fptr;
   HEADER* header;

   if (argc < 2) {
      fprintf(stderr,"Usage: %s tgafile\n",argv[0]);
      exit(-1);
   }

   /* Open the file */
   if ((fptr = fopen(argv[1],"r")) == NULL) {
      fprintf(stderr,"File open failed\n");
      exit(-1);
   }
/*
   ULONG lCurrentPos = ftell(fptr);
   fseek(fptr, 0, SEEK_END);
   ULONG size = ftell(fptr);
   fseek(fptr, lCurrentPos, SEEK_SET);

   UBYTE* buffer = (UBYTE*) malloc(size);

   fread(buffer, size, 1, fptr);

   header = (HEADER*)buffer;
*/
   ULONG size = 18; 
   UBYTE* buffer = (UBYTE*) malloc(TARGA_HEADER_BYTE_SIZE);
   fread(buffer, TARGA_HEADER_BYTE_SIZE, 1, fptr);

   header = (HEADER*)buffer;

   fprintf(stderr,"ID length:         %d\n",header->idlength);
   fprintf(stderr,"Colourmap type:    %d\n",header->colourmaptype);
   fprintf(stderr,"Image type:        %d\n",header->datatypecode);
   fprintf(stderr,"Colour map offset: %d\n",header->colourmaporigin);
   fprintf(stderr,"Colour map length: %d\n",header->colourmaplength); 
   fprintf(stderr,"Colour map depth:  %d\n",header->colourmapdepth);
   fprintf(stderr,"X origin:          %d\n",header->x_origin);
   fprintf(stderr,"Y origin:          %d\n",header->y_origin);
   fprintf(stderr,"Width:             %d\n",header->width);
   fprintf(stderr,"Height:            %d\n",header->height);
   fprintf(stderr,"Bits per pixel:    %d\n",header->bitsperpixel);
   fprintf(stderr,"Descriptor:        %d\n",header->imagedescriptor);

   return 0;
}
