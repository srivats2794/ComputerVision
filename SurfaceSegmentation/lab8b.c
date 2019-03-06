#include <stdio.h>
#include <stdlib.h>
#include <math.h> 


#define SQR(x) ((x)*(x))

int main (int argc, char *argv[])
{
	
	FILE	*fpt;
	FILE	*fpt2;
	
	//int		ROWS,COLS,BYTES;
	double	cp[7];
	unsigned char	RangeImage[128*128];
	double	xangle,yangle,dist;
	double	ScanDirectionFlag,SlantCorrection;
	
	double		P[3][128*128];
	double		Va[3][1];
	double		Vb[3][1];
	double		Sn[3][128*128];
	int		pdist;
	int             ImageTypeFlag;
	char	Filename[160],Outfile[160];
	char	header[320];
	double	X,Y,Z;
	int		i,j,k,sum,sum2,index,num_ind,max_len;
	unsigned char	*image,*labels,*thresh_image;
	int		ct;
	
	int		ROWS,COLS,BYTES,r,c,r2,c2;
	int		rows,cols,bytes;
	int		*indices;
	int		RegionSize,*RegionPixels,TotalRegions;
	double		var;
	void		RegionGrow();
	
	double	avg[3][1];
	double	tot[3][1];	/* average and total intensity in growing region */
	double	nm_avg;
	double	nm_Sn;
	double	dp;
	double	tha;
	
	
	if ((fpt=fopen("chair-range.ppm","rb")) == NULL)
	  {
	  printf("Unable to open chair-range.ppm for reading\n");
	  exit(0);
	  }
	ROWS = 128;
	COLS = 128;
	fscanf(fpt,"%s %d %d %d\n",header,&cols,&rows,&bytes);
	fread(RangeImage,1,128*128,fpt);
	fclose(fpt);
	
	thresh_image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

	for(r=0; r<ROWS; r++){
		for(c=0; c<COLS; c++){
			if(RangeImage[r*COLS+c]<120){
				thresh_image[r*COLS+c] = RangeImage[r*COLS+c];
			}
			else{
				thresh_image[r*COLS+c] = 255;
			}
		}
	}


	cp[0]=1220.7;		/* horizontal mirror angular velocity in rpm */
	cp[1]=32.0;		/* scan time per single pixel in microseconds */
	cp[2]=(COLS/2)-0.5;		/* middle value of columns */
	cp[3]=1220.7/192.0;	/* vertical mirror angular velocity in rpm */
	cp[4]=6.14;		/* scan time (with retrace) per line in milliseconds */
	cp[5]=(ROWS/2)-0.5;		/* middle value of rows */
	cp[6]=10.0;		/* standoff distance in range units (3.66cm per r.u.) */

	cp[0]=cp[0]*3.1415927/30.0;	/* convert rpm to rad/sec */
	cp[3]=cp[3]*3.1415927/30.0;	/* convert rpm to rad/sec */
	cp[0]=2.0*cp[0];		/* beam ang. vel. is twice mirror ang. vel. */
	cp[3]=2.0*cp[3];		/* beam ang. vel. is twice mirror ang. vel. */
	cp[1]/=1000000.0;		/* units are microseconds : 10^-6 */
	cp[4]/=1000.0;			/* units are milliseconds : 10^-3 */
	
	ScanDirectionFlag=1;

		/* start with semi-spherical coordinates from laser-range-finder: */
		/*			(r,c,RangeImage[r*COLS+c])		  */
		/* convert those to axis-independant spherical coordinates:	  */
		/*			(xangle,yangle,dist)			  */
		/* then convert the spherical coordinates to cartesian:           */
		/*			(P => X[] Y[] Z[])			  */
	
	/*fpt=fopen("XYZ_begin.txt","w");
	fprintf(fpt,"%f %f %f\n","X","Y","Z");
	for(k = 0; k<(128*128); k++){
		X = P[0][k];
		Y = P[1][k];
		Z = P[2][k];
		
		fprintf(fpt,"%f %f %f\n",X,Y,Z);
	}
	fclose(fpt);*/
	
	for (r=0; r<ROWS; r++)
		{
		for (c=0; c<COLS; c++)
		  {
		  SlantCorrection=cp[3]*cp[1]*((double)c-cp[2]);
		  xangle=cp[0]*cp[1]*((double)c-cp[2]);
		  yangle=(cp[3]*cp[4]*(cp[5]-(double)r))+	/* Standard Transform Part */
		SlantCorrection*ScanDirectionFlag;	/*  + slant correction */
		  dist=(double)thresh_image[r*COLS+c]+cp[6];
		  P[2][r*COLS+c]=sqrt((dist*dist)/(1.0+(tan(xangle)*tan(xangle))
		+(tan(yangle)*tan(yangle))));
		  P[0][r*COLS+c]=tan(xangle)*P[2][r*COLS+c];
		  P[1][r*COLS+c]=tan(yangle)*P[2][r*COLS+c];
		  }
		}

	
		
	fpt=fopen("XYZ_end.txt","w");
	fprintf(fpt,"%s %s %s\n","X","Y","Z");
	for (r=0; r<ROWS; r++)
		{
		for (c=0; c<COLS; c++)
		  {
			  X = P[0][r*COLS+c];
			  Y = P[1][r*COLS+c];
			  Z = P[2][r*COLS+c];
			  
			  fprintf(fpt,"%lf %lf %lf\n",P[0][r*COLS+c],P[1][r*COLS+c],P[2][r*COLS+c]);
		  }
		}
		
		//
	
	fclose(fpt);
	
	
	
	pdist = 3;
	
  for (r=pdist; r<ROWS-pdist; r++)
	{
	for (c=pdist; c<COLS-pdist; c++)
	  {
		  
		  
		  //horizontal
		  Va[0][0] = P[0][r*COLS+(c+pdist)] - P[0][r*COLS+c];
		  Va[1][0] = P[1][r*COLS+(c+pdist)] - P[1][r*COLS+c];
		  Va[2][0] = P[2][r*COLS+(c+pdist)] - P[2][r*COLS+c];
		  
		  Vb[0][0] = P[0][(r+pdist)*COLS+c] - P[0][r*COLS+c];
		  Vb[1][0] = P[1][(r+pdist)*COLS+c] - P[1][r*COLS+c];
		  Vb[2][0] = P[2][(r+pdist)*COLS+c] - P[2][r*COLS+c];
		  //printf("Va X, Y, Z, is %f, %f, %f\n",Va[0][0],Va[1][0],Va[2][0]);
		  //printf("Vb X, Y, Z, is %f, %f, %f\n",Vb[0][0],Vb[1][0],Vb[2][0]);
		  //printf("Sn_start X, Y, Z, is %f, %f, %f\n",Sn[0][r*COLS+c],Sn[1][r*COLS+c],Sn[2][r*COLS+c]);
		  
		  Sn[0][r*COLS+c] = (Vb[1][0]*Va[2][0]) - (Vb[2][0]*Va[1][0]);
		  Sn[1][r*COLS+c] = (Vb[2][0]*Va[0][0]) - (Vb[0][0]*Va[2][0]);
		  Sn[2][r*COLS+c] = (Vb[0][0]*Va[1][0]) - (Vb[1][0]*Va[0][0]);
		  //printf("Sn X, Y, Z, is %f, %f, %f\n",Sn[0][r*COLS+c],Sn[1][r*COLS+c],Sn[2][r*COLS+c]);
		  
	  }
	  
	}
/*
  for (r=0; r<pdist; r++)
	{
	for (c=0; c<pdist; c++)
	  {
		  
		  Sn[0][r*COLS+c] = 0.0;
		  Sn[1][r*COLS+c] = 0.0;
		  Sn[2][r*COLS+c] = 0.0;
		  //printf("Sn X, Y, Z, is %f, %f, %f\n",Sn[0][r*COLS+c],Sn[1][r*COLS+c],Sn[2][r*COLS+c]);
		  
		
		//printf("Sn X, Y, Z, is %f, %f, %f\n",Sn[0][0],Sn[1][0],Sn[2][0]);
	  }
	  
	}
	
  for (r=ROWS-pdist; r<ROWS; r++)
	{
	for (c=COLS-pdist; c<COLS; c++)
	  {
		  
		  Sn[0][r*COLS+c] = 0.0;
		  Sn[1][r*COLS+c] = 0.0;
		  Sn[2][r*COLS+c] = 0.0;
		  //printf("Sn X, Y, Z, is %f, %f, %f\n",Sn[0][r*COLS+c],Sn[1][r*COLS+c],Sn[2][r*COLS+c]);
		  
		
		//printf("Sn X, Y, Z, is %f, %f, %f\n",Sn[0][0],Sn[1][0],Sn[2][0]);
	  }
	  
	}	
	*/
	
		/* Allocate memory for images.  Read image (raw grey). */
	fpt2=fopen("XYZ_normals.txt","w");
	fprintf(fpt,"%s %s %s\n","nX","nY","nZ");
	for (r=0; r<ROWS; r++)
		{
		for (c=0; c<COLS; c++)
		  {
			  X = Sn[0][r*COLS+c];
			  Y = Sn[1][r*COLS+c];
			  Z = Sn[2][r*COLS+c];
			  //printf("X, Y, Z, %f, %f, %f\n",X,Y,Z);
			  fprintf(fpt2,"%lf %lf %lf\n",Sn[0][r*COLS+c],Sn[1][r*COLS+c],Sn[2][r*COLS+c]);
		  }
		}
		
		//
	
	fclose(fpt2);
	
	image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
		/* segmentation image = labels; calloc initializes all labels to 0 */
	labels=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
		/* used to quickly erase small grown regions */
	indices=(int *)calloc(ROWS*COLS,sizeof(int));
	
	
	for (r=0; r<ROWS; r++)
		{
		for (c=0; c<COLS; c++)
		  {
			  image[r*COLS+c] = RangeImage[r*COLS+c];
			  
		  }
		}
	
	if ((fpt=fopen("range_image_test.ppm","wb")) == NULL)
	  {
	  printf("Unable to open file for writing\n");
	  exit(0);
	  }
	//fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
	fwrite(image,1,ROWS*COLS,fpt);
	fclose(fpt);
	
	TotalRegions=0;
	for (r=2; r<ROWS-2; r++)
	  {
	  for (c=2; c<COLS-2; c++)
		{
			//printf("Sn X, Y, Z, is %f, %f, %f, row, col, %d, %d\n",Sn[0][r*COLS+c],Sn[1][r*COLS+c],Sn[2][r*COLS+c],r,c);
		if (labels[r*COLS+c] != 0){
		  continue;
		}
		/*if (!((Sn[0][r*COLS+c] == 0.0) && (Sn[1][r*COLS+c] == 0.0) && (Sn[2][r*COLS+c] == 0.0)))
		  continue;*/
		/*if (image[r*COLS+c]>=128)
		  continue;*/
		
		tot[0][0]=0.0;
		tot[1][0]=0.0;
		tot[2][0]=0.0;	/* compute in 5x5 window */
		ct = 0;
		for (r2=-2; r2<=2; r2++){
		  for (c2=-2; c2<=2; c2++){
			 if ((labels[(r+r2)*COLS+(c+c2)] != 0) || (thresh_image[(r+r2)*COLS+(c+c2)] == 255)){
				 ct++;
				 //printf("ct is %d\n",ct);
			 }
		  }
		}
		/*
		avg[0][0]=tot[0][0]/49.0;
		avg[1][0]=tot[1][0]/49.0;
		avg[2][0]=tot[2][0]/49.0;
		
		nm_avg = sqrt(pow(avg[0][0], 2.0) + pow(avg[1][0], 2.0) + pow(avg[2][0], 2.0));
		nm_Sn = sqrt(pow(Sn[0][r*COLS+c], 2.0) + pow(Sn[1][r*COLS+c], 2.0) + pow(Sn[2][r*COLS+c], 2.0));
		dp = (avg[0][0]*Sn[0][r*COLS+c]) + (avg[1][0]*Sn[1][r*COLS+c]) + (avg[2][0]*Sn[2][r*COLS+c]);
		tha = acos(dp/(nm_avg*nm_Sn));
		*/
		//printf("theta is %f\n",tha);
		if (ct < 1)	/* condition for seeding a new region is low var */
		  {
		  // printf("%d,%d avg=%lf var=%lf\n",r,c,avg,var);
		  TotalRegions++;
		  if (TotalRegions == 255)
			{
			printf("Segmentation incomplete.  Ran out of labels.\n");
			break;
			}
		  RegionGrow(RangeImage,labels,ROWS,COLS,r,c,0,TotalRegions,
			indices,&RegionSize,Sn);
		  if (RegionSize < 100)
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
	
	for (r=0; r<ROWS; r++)
	  {
	  for (c=0; c<COLS; c++)
		{
			labels[r*COLS+c] = labels[r*COLS+c]*(255/TotalRegions);
		}
	  }

	if ((fpt=fopen("seg_test.ppm","wb")) == NULL)
	  {
	  printf("Unable to open file for writing\n");
	  exit(0);
	  }
	fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
	fwrite(labels,1,ROWS*COLS,fpt);
	fclose(fpt);
}

	/*
	** Given an image, a starting point, and a label, this routine
	** paint-fills (8-connected) the area with the given new label
	** according to the given criteria (pixels close to the average
	** intensity of the growing region are allowed to join).
	*/

#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */

void RegionGrow(unsigned char *image,	/* image data */
		unsigned char *labels,	/* segmentation labels */
		int ROWS,int COLS,	/* size of image */
		int r,int c,		/* pixel to paint from */
		int paint_over_label,	/* image label to paint over */
		int new_label,		/* image label for painting */
		int *indices,		/* output:  indices of pixels painted */
		int *count,		/* output:  count of pixels painted */
		double		Sn_[3][128*128])		
{
int	r2,c2;
int	queue[MAX_QUEUE],qh,qt;
double	average[3][1];
double	total[3][1];	/* average and total intensity in growing region */
double	norm_avg;
double	norm_Sn;
double	dot_p;
double	theta;

*count=0;
if (labels[r*COLS+c] != paint_over_label){
  return;
}
labels[r*COLS+c]=new_label;
average[0][0]=Sn_[0][r*COLS+c];
average[1][0]=Sn_[1][r*COLS+c];
average[2][0]=Sn_[2][r*COLS+c];

total[0][0]=Sn_[0][r*COLS+c];
total[1][0]=Sn_[1][r*COLS+c];
total[2][0]=Sn_[2][r*COLS+c];

if (indices != NULL){
  indices[0]=r*COLS+c;
}
queue[0]=r*COLS+c;
qh=1;	/* queue head */
qt=0;	/* queue tail */
(*count)=1;
while (qt != qh)
  {
  if ((*count)%1 == 0)	/* recalculate average after each 50 pixels join */
    {
    average[0][0]=total[0][0]/(double)(*count);
	average[1][0]=total[1][0]/(double)(*count);
	average[2][0]=total[2][0]/(double)(*count);
    // printf("new avg=%d\n",average);
    }
  for (r2=-1; r2<=1; r2++){
    for (c2=-1; c2<=1; c2++)
      {
      if (r2 == 0  &&  c2 == 0){
        continue;
	  }
      if ((queue[qt]/COLS+r2) < 3  ||  (queue[qt]/COLS+r2) >= ROWS-3  ||
	  (queue[qt]%COLS+c2) < 3  ||  (queue[qt]%COLS+c2) >= COLS-3){
        continue;
	  }
      if (labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]!=paint_over_label){
        continue;
	  }
	  norm_avg = sqrt(pow(average[0][0], 2.0) + pow(average[1][0], 2.0) + pow(average[2][0], 2.0));
	  norm_Sn = sqrt(pow(Sn_[0][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2], 2.0) + pow(Sn_[1][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2], 2.0) + pow(Sn_[2][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2], 2.0));
	  dot_p = (average[0][0]*Sn_[0][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]) + (average[1][0]*Sn_[1][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]) + (average[2][0]*Sn_[2][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]);
	  theta = acos(dot_p/(norm_avg*norm_Sn));
	  
		/* test criteria to join region */
		//printf("theta is %f\n",theta);
      if (theta > 0.6){
        continue;
	  }
      labels[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]=new_label;
      if (indices != NULL){
        indices[*count]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
	  }
      total[0][0]=total[0][0]+Sn_[0][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2];
	  total[1][0]=total[1][0]+Sn_[1][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2];
	  total[2][0]=total[2][0]+Sn_[2][(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2];
      (*count)++;
      queue[qh]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
      qh=(qh+1)%MAX_QUEUE;
      if (qh == qt)
        {
        printf("Max queue size exceeded\n");
        exit(0);
        }
      }
  }
  qt=(qt+1)%MAX_QUEUE;
  }
}
