/*
Lab 5
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

struct groundTruth {
  char letter;
  int colNo;
  int rowNo;
}

main()

{
FILE		*fpt;
FILE    *txtFile;
unsigned char	*image;
float	*sobel_image;
unsigned char	*normalized_sobel_image;
unsigned char	*initial_image;
unsigned char	*buffer_image;

unsigned int *E_ext;
unsigned int *E_internal2;
unsigned int *E_internal;
unsigned int *E_vi;


#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

char    buf[1024];
char		header[320];
int		ROWS,COLS,BYTES;
int		r,c,r1,c1,n,i,r2,c2,x,y,l,rc,cc,r0,c0,j,rc_min,cc_min,minimum,location,k,m,o,r9,c9,sum_dist;
float max,min;
double   sum1, sum2,sum;
int colNo_1[42];
int rowNo_1[42];
int colNo_[43];
int rowNo_[43];
int c_new[42];
int r_new[42];
int c_old[42];
int r_old[42];
int dist[42];
int avg_dist[42];
struct groundTruth textData;

/* read input image */
if ((fpt=fopen("hawk.ppm","rb")) == NULL)
    {
    printf("Unable to open hawk.ppm for reading\n");
    exit(0);
    }
fscanf(fpt,"%s %d %d %d\n",header,&COLS,&ROWS,&BYTES);
if (strcmp(header,"P5") != 0  ||  BYTES != 255)
    {
    printf("Not a greyscale 8-bit PPM image\n");
    exit(0);
    }

image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

if (image == NULL)
  {
  printf("Unable to allocate %d x %d memory\n",COLS,ROWS);
  exit(0);
  }

fread(image,1,COLS*ROWS,fpt);
fclose(fpt);


/* read txt file*/
txtFile = fopen("hawk_init.txt", "r");
char col_no[20] = "", row_no[20] ="";

i=0;
while ( !feof(txtFile)){
    fscanf(txtFile,"%s %s\n", col_no, row_no);
    textData.colNo = atoi(col_no);
    textData.rowNo = atoi(row_no);

    //letter_[i] = textData.letter;
    colNo_1[i] = textData.colNo;
    rowNo_1[i] = textData.rowNo;
    i++;

    //printf("%c %d %d\n",  textData.letter, textData.colNo, textData.rowNo);
 }
fclose(txtFile);

n = NELEMS(colNo_1);
//printf("%d\n", n);
for(i=0;i<n;i++){
  colNo_[i] = colNo_1[i];
  rowNo_[i] = rowNo_1[i];
  //printf("%d %d\n",colNo_[i],rowNo_[i]);
}
colNo_[42] = colNo_1[0];
rowNo_[42] = rowNo_1[0];
//printf("%d %d\n",colNo_[42],rowNo_[42]);

sobel_image=(float *)calloc(ROWS*COLS,sizeof(float));

// sobel image
int sobel_matrix_f1[3][3] = {{-1, 0, 1},
                             {-2, 0, 2},
                             {-1, 0, 1}};
int sobel_matrix_f2[3][3] = {{-1, -2, -1},
                             { 0,  0,  0},
                             { 1,  2,  1}};
for (r=1; r<ROWS-1; r++){
  for (c=1; c<COLS-1; c++)
    {
      sum1=0;
      for (r1=-1; r1<=1; r1++){
        for (c1=-1; c1<=1; c1++){

          sum1 +=sobel_matrix_f1[r1+1][c1+1]*(int)image[(r+r1)*COLS + (c+c1)];
        }
      }
      sum2 = 0;
      for (r2=-1; r2<=1; r2++){
        for (c2=-1; c2<=1; c2++){

          sum2 +=sobel_matrix_f2[r2+1][c2+1]*(int)image[(r+r2)*COLS + (c+c2)];
        }
      }

    sum = sqrt(pow(sum1,2) + pow(sum2,2));
    //if(sum>255) sum=255;
    //if(sum<0) sum =0;
    sobel_image[r*COLS+c] = sum;
    }
}

// Normalizing the sobel image

max = sobel_image[0];
min = sobel_image[0];
for (i=0; i<ROWS*COLS; i++){
  if (sobel_image[i] > max){
    max = sobel_image[i];
  }
  if (sobel_image[i] < min){
    min = sobel_image[i];
  }
}

normalized_sobel_image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

for (i=0; i<ROWS*COLS; i++){
  normalized_sobel_image[i]=(sobel_image[i]-min)*255/(max-min);
}


// Original contour

initial_image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));


for (r=1; r<ROWS-1; r++){
  for (c=1; c<COLS-1; c++){
    initial_image[r*COLS + c] = image[r*COLS + c];
  }
}

n = NELEMS(colNo_);
//printf("%d\n", n);

for(i=0;i<n-1;i++){
  r = rowNo_[i];
  c = colNo_[i];
  for(r1=-3; r1<=3; r1++){
      initial_image[(r+r1)*COLS + c] = 0;
  }
  for(c1=-3; c1<=3; c1++){
      initial_image[r*COLS + c+c1] = 0;
  }

  //printf("%d %d\n",r,c);
}

E_internal=(unsigned int *)calloc(7*7,sizeof(unsigned int));
E_ext=(unsigned int *)calloc(7*7,sizeof(unsigned int));
E_internal2=(unsigned int *)calloc(7*7,sizeof(unsigned int));
E_vi=(unsigned int *)calloc(7*7,sizeof(unsigned int));


//Active contouring

//avg distance
// for(i=1;i<n;i++){
//   r = rowNo_[i];
//   c = colNo_[i];
//   r0 = rowNo_[i-1];
//   c0 = colNo_[i-1];
//   dist[i-1] = sqrt(pow((r-r0),2)+pow((c-c0),2));
//   sum_dist +=dist[i-1];
// }

//avg_dist[1] = sum_dist/42;

//printf("%d %d\n",rowNo_[42],colNo_[42] );

for(m=0;m<n;m++){
  r_old[m] = rowNo_[m];
  c_old[m] = colNo_[m];
}

for(k=1;k<=30;k++){
  sum_dist=0;
  //avg distance
  for(o=1;o<n;o++){
    r9 = r_old[o];
    c9 = c_old[o];
    r0 = r_old[o-1];
    c0 = c_old[o-1];
    dist[o-1] = sqrt(pow((r9-r0),2)+pow((c9-c0),2));
    sum_dist += dist[o-1];
  }

  avg_dist[k] = sum_dist/42;

  for(i=0;i<n;i++){
    r = r_old[i];
    c = c_old[i];
    for(rc=-3; rc<=3; rc++){
      for(cc=-3; cc<=3; cc++){
        E_internal[(3+rc)*7+(3+cc)] = pow(((c+cc)-c_old[i+1]),2) + pow(((r+rc)-r_old[i+1]),2);
        E_ext[(3+rc)*7+(3+cc)] = - normalized_sobel_image[(r+rc)*COLS+(c+cc)];
        E_internal2[(3+rc)*7+(3+cc)] = pow((avg_dist[k] - sqrt(pow((r-r_old[i+1]),2) + pow((c-c_old[i+1]),2))),2);
        E_vi[(3+rc)*7+(3+cc)] = E_internal[(3+rc)*7+(3+cc)] + E_ext[(3+rc)*7+(3+cc)] + E_internal2[(3+rc)*7+(3+cc)];
      }
    }
    minimum = E_vi[0];
    for(j=1;j<49;j++){
      if(E_vi[j]<minimum){
        minimum = E_vi[j];
        location = j;
      }
    }
    //printf("%d\n", location);

    cc_min = (location)%7;
    rc_min = (location-cc)/7;
    r_new[i] = r + (-3+rc_min);
    c_new[i] = c + (-3+cc_min);
    //printf("%d %d\n", r_old[i],c_old[i] );

  }

  for(m=0;m<n;m++){
    r_old[m]=r_new[m];
    c_old[m]=c_new[m];
  }


}


buffer_image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));


for (r=1; r<ROWS-1; r++){
  for (c=1; c<COLS-1; c++){
    buffer_image[r*COLS + c] = image[r*COLS + c];
  }
}

for(i=0;i<n-1;i++){
  r = r_new[i];
  c = c_new[i];
  for(r1=-3; r1<=3; r1++){
      buffer_image[(r+r1)*COLS + c] = 0;
  }
  for(c1=-3; c1<=3; c1++){
      buffer_image[r*COLS + c+c1] = 0;
  }

  //printf("%d %d\n",r,c);
}
for (i=0;i<n-1;i++)
{
  printf("%d %d\n",r_new[i], c_new[i]);
}
fpt=fopen("buffer_image.ppm","w");
if (fpt == NULL)
  {
  printf("Unable to open brighter.ppm for writing\n");
  exit(0);
  }
fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
fwrite(buffer_image,1,ROWS*COLS,fpt);
fclose(fpt);

fpt=fopen("initial_image.ppm","w");
if (fpt == NULL)
  {
  printf("Unable to open brighter.ppm for writing\n");
  exit(0);
  }
fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
fwrite(initial_image,1,ROWS*COLS,fpt);
fclose(fpt);

fpt=fopen("sobel_image.ppm","w");
if (fpt == NULL)
  {
  printf("Unable to open brighter.ppm for writing\n");
  exit(0);
  }
fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
fwrite(normalized_sobel_image,1,ROWS*COLS,fpt);
fclose(fpt);

}
