#include <stdio.h>
#include <stdlib.h>

void main()
{
FILE *fpt;
unsigned char *image;
int i,j;

image=(unsigned char*)calloc(100,sizeof(unsigned char));
for(i=2;i<=7;i++)
 for(j=2;j<=7;j++)
  {image[i*10+j]=255;
   printf("%d\n",i*10+j);}

fpt=fopen("template.ppm","wb");
fprintf(fpt,"P5 %d %d 255\n",10,10);
fwrite(image,1,100,fpt);
fclose(fpt);



}
