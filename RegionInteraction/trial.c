	/*
	** Reads a greyscale image from the comand line, and creates
	** a segmentation of regions based upon similar greyscale areas.
	** This version demonstrates how to use a paint-fill technique
	** along with region criteria to do segmentation.
	**
	** Good demonstration on targets.ppm.
	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SQR(x) ((x)*(x))

int main(int argc, char *argv[])

{
unsigned char	*image,*labels;
FILE		*fpt;
char		header[80];
int		ROWS,COLS,BYTES,r,c,r2,c2;
int		*indices,i;
int		RegionSize,*RegionPixels,TotalRegions;
double		avg,var;
void		RegionGrow();

if (argc != 2)
  {
  printf("Usage:  reg-grow [image]\n");
  exit(0);
  }

	/* Allocate memory for images.  Read image (raw grey). */
printf("Reading:  %s\n",argv[1]);
if ((fpt=fopen(argv[1],"rb")) == NULL)
  {
  printf("Unable to open %s for reading\n",argv[1]);
  exit(0);
  }
fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
if (strcmp(header,"P5") != 0  ||  BYTES != 255)
  {
  printf("Must be a greyscale PPM 8-bit image\n");
  exit(0);
  }
image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
	/* segmentation image = labels; calloc initializes all labels to 0 */
labels=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
	/* used to quickly erase small grown regions */
indices=(int *)calloc(ROWS*COLS,sizeof(int));
header[0]=fgetc(fpt);	/* whitespace character after header */
fread(image,1,ROWS*COLS,fpt);
fclose(fpt);

TotalRegions=0;
for (r=1; r<ROWS-1; r++)
  {
  for (c=1; c<COLS-1; c++)
    {
    if (labels[r*COLS+c] != 0)
      continue;
    avg=var=0.0;	/* compute average and variance in 7x7 window */
    for (r2=-1; r2<=1; r2++)
      for (c2=-1; c2<=1; c2++)
         avg+=(double)(image[(r+r2)*COLS+(c+c2)]);
    avg/=9.0;
    for (r2=-1; r2<=1; r2++)
      for (c2=-1; c2<=1; c2++)
         var+=SQR(avg-(double)image[(r+r2)*COLS+(c+c2)]);
    var=sqrt(var)/9.0;
    //printf("index=%d,\tvar=%f\n",r*COLS+c,var);
    if (var < 1.0)	/* here condition for seeding a new region is low var, so put your code for 2 predicates in place of the avg and the var*/
      {
      //printf("%d,%d avg=%lf var=%lf\n",r,c,avg,var);
      TotalRegions++;
	 RegionGrow(image,labels,ROWS,COLS,r,c,0,TotalRegions,indices,&RegionSize);
      if (RegionSize < 10)
        {	/* erase region (relabel pixels back to 0) */
        for (i=0; i<RegionSize; i++)
          labels[indices[i]]=0;
        TotalRegions--;
        }
      else
        printf("Region labeled %d is %d in size\n",TotalRegions,RegionSize);
      }
    }
  if (c < COLS-3)
    break;	/* ran out of labels -- break both loops */
  }
printf("%d total regions were found\n",TotalRegions);

if ((fpt=fopen("seg.ppm","wb")) == NULL)
  {
  printf("Unable to open file for writing\n");
  exit(0);
  }
fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
fwrite(labels,1,ROWS*COLS,fpt);
fclose(fpt);
}



#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */

void RegionGrow(unsigned char *image,	/* image data */
		unsigned char *labels,	/* segmentation labels */
		int ROWS,int COLS,	/* size of image */
		int r,int c,		/* pixel to paint from */
		int paint_over_label,	/* image label to paint over */
		int new_label,		/* image label for painting */
		int *indices,		/* output:  indices of pixels painted */
		int *count)		/* output:  count of pixels painted */
{
int	r2,c2;
int	queue[MAX_QUEUE],qh,qt;
int	average,total;	/* average and total intensity in growing region */

printf("ROWS=%d\tCOLS=%d\tr=%d\tc=%d\n",ROWS,COLS,r,c);
printf("paint_over_label=%d\tnew_label=%d\n",paint_over_label,new_label);

*count=0;
if (labels[r*COLS+c] != paint_over_label)
  return;
labels[r*COLS+c]=new_label;
//printf("labels[%d]=%d\n",r*COLS+c,new_label);
average=total=(int)image[r*COLS+c];
//printf("average=%d\n",average);
if (indices != NULL)
  indices[0]=r*COLS+c;
queue[0]=r*COLS+c;
qh=1;	/* queue head */
qt=0;	/* queue tail */
(*count)=1;
while (qt != qh)
  {
   if ((*count)%50 == 0)	/* recalculate average after each 50 pixels join */
    {
    average=total/(*count);
    // printf("new avg=%d\n",average);
    }
   for (r2=-1; r2<=1; r2++)
    for (c2=-1; c2<=1; c2++)
      {
      if (r2 == 0  &&  c2 == 0)
        continue;
      if ((queue[qt]/COLS+r2) < 0  ||  (queue[qt]/COLS+r2) >= ROWS  ||  (queue[qt]%COLS+c2) < 0  ||  (queue[qt]%COLS+c2) >= COLS)
        {printf("haha\n");continue;}
      if (labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]!=paint_over_label)
        {//printf("yes\t%d\t%d\n",r,c);
	continue;}
		/* test criteria to join region */
      if (abs((int)(image[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2])-average) > 10)
        continue;
      labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]=new_label;
	//printf("labels[%d]=%d\n",(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2,labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]);
      

      if (indices != NULL)
        indices[*count]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
      total+=image[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2];
      (*count)++;
      queue[qh]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
      qh=(qh+1)%MAX_QUEUE;
      if (qh == qt)
        {
        printf("Max queue size exceeded\n");
        exit(0);
        }
      }
  qt=(qt+1)%MAX_QUEUE;
	//printf("qh=%d\tqt=%d\n",qh,qt);
  }
}

