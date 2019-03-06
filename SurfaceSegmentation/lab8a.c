
/* LAB 8- Srivatsan Srinivasan */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define ROWS	128
#define COLS	128

#define MAX_QUEUE 10000
#define SQR(x) ((x)*(x))
main()
{
FILE		*fpt1, *fpt2, *fpt3, *fpt4;
unsigned char	*image1,*image2, *image3;
int		ROWS1,COLS1,BYTES1,j=0, Tmax, Tmin,r2,c2,i=0,x,y,regionflag=0;
char		header1[320], header2[320];
int* labels;
int queue[MAX_QUEUE],count,totalregion,color[50];
labels = (int*)calloc(128 * 128, sizeof(int));
/* read image */
if ((fpt1=fopen("chair-range.ppm","rb")) == NULL)
  {
  printf("Unable to open image for reading\n");
  exit(0);
  }
fscanf(fpt1,"%s %d %d %d\n",header1,&COLS1,&ROWS1,&BYTES1);

image1=(unsigned char *)calloc(ROWS1*COLS1,sizeof(unsigned char));
fread(image1,1,COLS1*ROWS1,fpt1);
fclose(fpt1);

/* Thresholding */
Tmax=130; Tmin=30;
image2 =(unsigned char *)calloc(ROWS1*COLS1,sizeof(unsigned char));
image3 =(unsigned char *)calloc(ROWS1*COLS1,sizeof(unsigned char));
for (j=0; j<ROWS1*COLS1; j++)
          {
           if (image1[j]>Tmax||image1[j]<Tmin)
            {
             image2[j] = 0;
            }
           else if (image1[j]<=Tmax && image1[j]>=Tmin)
            {
             image2[j] = image1[j];
            }
           }
fpt4=fopen("tchair-range.ppm","w");
fprintf(fpt4,"P5 %d %d 255\n",COLS1,ROWS1);
fwrite(image2,COLS1*ROWS1,1,fpt4);
fclose(fpt4);

int	r,c;
double	cp[7];
double	xangle,yangle,dist;
double	ScanDirectionFlag,SlantCorrection;
unsigned char	RangeImage[128*128];
double		P[3][128*128];
int             ImageTypeFlag;
char	Filename[160],Outfile[160];
FILE	*fpt;

printf("Enter range image file name:");
scanf("%s",Filename);
if ((fpt=fopen(Filename,"r")) == NULL)
  {
  printf("Couldn't open %s\n",Filename);
  exit(0);
  }
fread(RangeImage,1,128*128,fpt);
fclose(fpt);

printf("Up(-1), Down(1) or Neither(0)? ");
scanf("%d",&ImageTypeFlag);


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

switch(ImageTypeFlag)
  {
  case 1:		/* Odetics image -- scan direction upward */
    ScanDirectionFlag=-1;
    break;
  case 0:		/* Odetics image -- scan direction downward */
    ScanDirectionFlag=1;
    break;
  default:		/* in case we want to do this on synthetic model */
    ScanDirectionFlag=0;
    break;
  }

if (ImageTypeFlag != 3)
  {
  for (r=0; r<ROWS; r++)
    {
    for (c=0; c<COLS; c++)
      {
      SlantCorrection=cp[3]*cp[1]*((double)c-cp[2]);
      xangle=cp[0]*cp[1]*((double)c-cp[2]);
      yangle=(cp[3]*cp[4]*(cp[5]-(double)r))+	/* Standard Transform Part */
	SlantCorrection*ScanDirectionFlag;	/*  + slant correction */
      dist=(double)RangeImage[r*COLS+c]+cp[6];
      P[2][r*COLS+c]=sqrt((dist*dist)/(1.0+(tan(xangle)*tan(xangle))
	+(tan(yangle)*tan(yangle))));
      P[0][r*COLS+c]=tan(xangle)*P[2][r*COLS+c];
      P[1][r*COLS+c]=tan(yangle)*P[2][r*COLS+c];
      }
    }
  }
double X,Y,Z;
/*fprintf(Outfile,"coords.txt",Filename);
fpt=fopen(Outfile,"w");
fwrite(P[0],8,128*128,fpt);
fwrite(P[1],8,128*128,fpt);
fwrite(P[2],8,128*128,fpt);
fclose(fpt); */
fpt3=fopen("coords1.txt","w");
for (r=0; r<ROWS; r++)
  {
  for (c=0; c<COLS; c++)
    {
      X = P[0][r*COLS+c];
      Y = P[1][r*COLS+c];
      Z = P[2][r*COLS+c];

      fprintf(fpt3,"%lf %lf %lf\n",X,Y,Z);
    }
  }
fclose(fpt3);
double Ax0,Ax1,Ax2,Bx0,Bx1,Bx2;
double product;
double cross[3][128*128];
double dot[128*128];
double angle[128*128];
for(r=0;r<ROWS-4;r++)
{
  for(c=0;c<COLS-4;c++)
   {
     Ax0=P[0][r*COLS+c]- P[0][r*COLS+(c+3)];
     Ax1=P[1][r*COLS+c]- P[1][r*COLS+(c+3)];
     Ax2=P[2][r*COLS+c]- P[2][r*COLS+(c+3)];
     Bx0=P[0][r*COLS+c]- P[0][(r+3)*COLS+c];
  Bx1=P[1][r*COLS+c]- P[1][(r+3)*COLS+c];
  Bx2=P[2][r*COLS+c]- P[2][(r+3)*COLS+c];
  cross[0][r*COLS+c]= Ax1*Bx2-Ax2*Bx1;
  cross[1][r*COLS+c]= Ax2*Bx0-Ax0*Bx2;
  cross[2][r*COLS+c]= Ax0*Bx2-Ax2*Bx0;
  }
}
for(r=5;r<ROWS-5;r++)
  {
    for(c=5;c<COLS-5;c++)
      {
        dot[r*COLS+c]= cross[0][r*COLS+c]*Ax0+cross[1][r*COLS+c]*Ax1+cross[2][r*COLS+c]*Ax2;
        product=(sqrt((cross[0][r*COLS+c])*(cross[0][r*COLS+c])+(cross[1][r*COLS+c])*(cross[1][r*COLS+c])+(cross[2][r*COLS+c])*(cross[2][r*COLS+c])))*(sqrt(Ax0*Ax0+Ax1*Ax1+Ax2*Ax2));
        angle[r*COLS+c]= (dot[r*COLS+c])/product;
      }
  }
double avg,var;
double variance[128*128];
int flag=0;
/*for(r=0;r<50;r++){
printf("angle %lf\n",angle[r]);
} */
for (r=3; r<ROWS-3; r++)
{
  for (c=3; c<COLS-3; c++)
    {
      avg=var=0.0;
        for (r2=-3; r2<=3; r2++)
          for (c2=-3; c2<=3; c2++)
            avg+=(double)(angle[(r+r2)*COLS+(c+c2)]);
        avg/=49.0;
        for (r2=-3; r2<=3; r2++)
          for (c2=-3; c2<=3; c2++)
            var+=SQR(avg-(double)angle[(r+r2)*COLS+(c+c2)]);var=sqrt(var)/49.0;
        flag=flag+1;
    }
}
int a,b;
double avrx,avry,avrz,sumx,sumy,sumz,cosval;
int qt, qh;
totalregion = 0;
int cod = 0;
  for (i = 0; i <=6; i++)
    {
      color[i] = 40 * i;
    }
  for (i = 2; i < 126; i++)
    {
      for (y = 2; y < 126; y++)
        {
          regionflag =0;
          for (a = -2; a < 3; a++)
            for (b = -2;b < 3; b++)
              {
                if (labels[(i+a) * 128 + y+b] == 1) regionflag = 1;
              }
              if (regionflag == 0)
              {
                totalregion++;
                queue[0] = i*128 + y;
                RangeImage[i * 128 + y] = color[totalregion];
                qh = 1;qt = 0;count = 1;
                sumx = cross[0][i * 128 + y];
                sumy = cross[1][i * 128 + y];
                sumz = cross[2][i * 128 + y];
                while (qt != qh)
                  {
                    avrx = sumx / count;
                    avry = sumy / count;
                    avrz = sumz / count;
                    for (r2 = -1; r2 <= 1; r2++)
                    {
                      for (c2 = -1; c2 <= 1; c2++)
                        {
                          if (r2 == 0 && c2 == 0)
                          continue;
                    if ((queue[qt] / COLS + r2) < 0 || (queue[qt] / COLS + r2)>= ROWS ||(queue[qt] % COLS + c2) < 0 || (queue[qt] %COLS + c2) >= COLS)
                          continue;
                    if (labels[(queue[qt] / COLS + r2)*COLS + queue[qt]%COLS + c2] != 0)
                          continue;
                          /* test criteria to join region */
                          cod = (queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2;
                          cosval = (cross[0][cod] * avrx + cross[1][cod] * avry + cross[2][cod] * avrz) / sqrt((SQR(cross[0][cod]) + SQR(cross[1][cod]) + SQR(cross[2][cod]))*(SQR(avrx) + SQR(avry) +SQR(avrz)));
                          //printf("%lf ", cosval);
                          if (cosval < 0)
                          cosval = -cosval;
                          if (cosval<0.7)
                          continue;
                          image3[(queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2] = color[totalregion];
                          sumx += cross[0][cod];
                          sumy += cross[1][cod];
                          sumz += cross[2][cod];
                          count++;
                          queue[qh] = (queue[qt] / COLS + r2)*COLS + queue[qt] % COLS + c2;
                          qh = (qh + 1) % MAX_QUEUE;
                          if (qh == qt)
                          {
                            printf("Max queue size exceeded\n");
                            exit(0);
                          }
                        }
                      }
                      qt = (qt + 1) % MAX_QUEUE;
                    }
                  }
                }
              }
    printf("Totalregions %d",totalregion);
    //
    fpt4 = fopen("segimage.ppm", "wb");
    fprintf(fpt4, "P5 %d %d 255\n", 128, 128);
    fwrite(image3, 128 * 128, 1, fpt);
    fclose(fpt);

}
