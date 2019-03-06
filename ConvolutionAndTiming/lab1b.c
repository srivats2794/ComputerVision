/* SEPERABLE FILTER */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

main()

{
FILE		*fpt;
unsigned char	*image;
unsigned char 	*smoothedrow;
double          *smoothedcol;
char		header[320];
int		ROWS,COLS,BYTES;
int		r,c,r2,c2,sum;

clock_t tic = clock();

	/* read image */
if ((fpt=fopen("bridge.ppm","rb")) == NULL)
  {
  printf("Unable to open bridge.ppm for reading\n");
  exit(0);
  }
fscanf(fpt,"%s %d %d %d\n",header,&COLS,&ROWS,&BYTES);
if (strcmp(header,"P5") != 0  ||  BYTES != 255)
  {
  printf("Not a greyscale 8-bit PPM image\n");
  exit(0);
  }
image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
header[0]=fgetc(fpt);	/* read white-space character that separates header */
fread(image,1,COLS*ROWS,fpt);
fclose(fpt);

	/* allocate memory for smoothed versions of image */
smoothedrow=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
smoothedcol=(double *)calloc(ROWS*COLS,sizeof(double));

/* smooth image column wise, skipping the border points */

for (c=0; c<=COLS; c++)
 for (r=3; r<=ROWS-4; r++)
  {      
    sum=0;
    for (r2=-3; r2<=3; r2++)
    {    
	sum+=image[(r+r2)*COLS+(c)];
    	smoothedcol[r*COLS+c]=sum;
    }
  }

/*smooth image row wise, skipping border points*/
for (r=3; r<=ROWS-4; r++)
 for (c=3; c<=COLS-4; c++)

 {
   sum=0;
   for (c2=-3; c2<=3; c2++)
   {
    sum+=smoothedcol[(r*COLS+(c+c2))];
    smoothedrow[r*ROWS+c]=sum/49;
   }
 }

clock_t toc = clock();

	/* time taken*/
printf("Smoothing time: %f seconds\n", (double)(toc-tic)/ CLOCKS_PER_SEC);


	/* write out smoothed image to see result */
fpt=fopen("smoothedrow.ppm","w");
fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
fwrite(smoothedrow,COLS*ROWS,1,fpt);
fclose(fpt);
}
