/*float max,min;
max=g[0];
for(i=1;i<ROWS*COLS;i++)
{
  if(g[i]>max)
  max=g[i];
}
min= g[0];
for(i=1;i<ROWS*COLS;i++)
{
  if(g[i]<min)
  min=g[i];
}
printf ("%f %f\n", max, min);
double *g2;
g2= (double *)calloc(ROWS*COLS,sizeof(double));
double val;
for(i=0;i<ROWS*COLS;i++)
{
  val=255*((g[i]-min)/(max-min));
  g2[i]=val;
}


double *G;
G= (double *)calloc(ROWS*COLS,sizeof(double));
for(i=0;i<ROWS*COLS;i++)
{
 G[i]=sqrt((int)g[i]);
}*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
main()
/* reading Hawk Image*/ {
FILE *fpt1; FILE *fpt; FILE *fpt2;

unsigned char *image1;

char header1[320];
int ROWS,COLS,BYTES; int r,c,r2,c2,sum ;
if ((fpt1=fopen("hawk.ppm","rb")) == NULL) {
printf("no image\n"); exit(0);
}
fscanf(fpt1,"%s %d %d %d\n",header1,&COLS,&ROWS,&BYTES); if (strcmp(header1,"P5") != 0 || BYTES != 255)
{
printf("Not a greyscale 8-bit PPM image\n"); exit(0);
}
image1=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
header1[0]=fgetc(fpt1); /* read white-space character that separates header */ fread(image1,1,COLS*ROWS,fpt1);
fclose(fpt1);

/* Applying Sobel Filter */
int *horizontal;
int *vertical;
horizontal= (int*)calloc(ROWS*COLS,sizeof(int));
vertical = (int*)calloc(ROWS*COLS,sizeof(int));

int sobel_gx[9]={-1,0,1,-2,0,2,-1,0,1};
int sobel_gy[9]={-1,-2,-1,0,0,0,1,2,1};

int TCOLS=3; int TROWS=3;
for(r=0;r<ROWS-2;r++){ for(c=0;c<COLS-2;c++){sum=0;
for(r2=-1;r2<=1;r2++){ for(c2=-1;c2<=1;c2++){
sum=sum+image1[(r+1+r2)*COLS+(c+1+c2)]*sobel_gx[(r2+1)*TCOLS+(c2+1)] ; }
}

horizontal[(r+1)*COLS+(c+1)]=sum; }
}

for(r=0;r<ROWS-2;r++){ for(c=0;c<COLS-2;c++){
sum=0;
for(r2=-1;r2<=1;r2++){ for(c2=-1;c2<=1;c2++){
sum=sum+image1[(r+1+r2)*COLS+(c+1+c2)]*sobel_gy[(r2+1)*TCOLS+(c2+1)] ; }
}

vertical[(r+1)*COLS+(c+1)]=sum; }
}
float *grad_magnitude;
grad_magnitude= (float*)calloc(ROWS*COLS,sizeof(float));

int i;

for(i=0;i<ROWS*COLS;i++){
grad_magnitude[i] = (((horizontal[i]*horizontal[i]) + (vertical[i]*vertical[i]))); }

/* normalizing square of grad magnitude */ float maximum,minimum;
maximum= grad_magnitude[0]; for(r=1;r<ROWS*COLS;r++){ if(grad_magnitude[r]>maximum){ maximum=grad_magnitude[r];
} }
minimum = grad_magnitude[0]; for(r=1;r<ROWS*COLS;r++){ if(grad_magnitude[r]<minimum){ minimum=grad_magnitude[r];
}


}

printf(" maximun and minimum are %f and %f\n", maximum,minimum); float *normgrad_magnitude;
normgrad_magnitude= (float*)calloc(ROWS*COLS,sizeof(float));

float value;
for(r=0;r<ROWS*COLS;r++){
value= ((grad_magnitude[r]-minimum))/(maximum-minimum) ;
 normgrad_magnitude[r]=value;

}
fpt2=fopen("sobelhawk.ppm","w");
  fprintf(fpt2,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(normgrad_magnitude,COLS*ROWS,1,fpt2);
  fclose(fpt2);
}
