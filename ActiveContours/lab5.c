/* LAB - Active Contours- Srivatsan Srinivasan*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


main()
{
  FILE *fpt1;
  FILE *fpt2, *fpt3, *fpt4, *hawk_init;
  int columns;int rows;
  unsigned char *orighawk,*g, *binary, *output1;
  double *G;
  char header[320];
  int ROWS, COLS, BYTES;
  int r,c, *Eint, *Eext, *Eint2, *Eimg;

  /*Reading original image*/
  if ((fpt1=fopen("hawk.ppm","rb"))==NULL)
  {printf("Image not found\n");exit(0);}
  fscanf(fpt1,"%s%d%d%d\n",header,&COLS,&ROWS,&BYTES);
  if (strcmp(header,"P5")!=0||BYTES!=255)
  {printf("Not a greyscale 8-bit PPM image\n"); exit(0);}
  orighawk=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  fread(orighawk,1,COLS*ROWS,fpt1);
  fclose(fpt1);

  /* Convolution using sobel operator*/
  int f1[3][3]={{ -1,  0,  1 },
		            { -2,  0,  2 },
		            { -1,  0,  1 }};

  int f2[3][3]= {{ -1,  0,  1 },
		             { -2,  0,  2 },
		             { -1,  0,  1 }};
  int ROWS2=3,COLS2=3, r2, c2,r3,c3,r4,c4, i=0, max, min, threshold;
  double sum1, sum2, sum;
  G=(double *)calloc(ROWS*COLS,sizeof(double));

  for(r=1;r<ROWS-1;r++)
  {
    for(c=1;c<COLS-1;c++)
    {
      sum1=0; sum2=0;
      for(r2=-1;r2<=1;r2++)
      {
       for(c2=-1;c2<=1;c2++)
       {
         sum1+=orighawk[(r+r2)*COLS+(c+c2)]*f1[1+r2][1+c2];
       }
      }
      for(r2=-1;r2<=1;r2++)
      {
       for(c2=-1;c2<=1;c2++)
       {
        sum2+=orighawk[(r+r2)*COLS+(c+c2)]*f2[r2+1][c2+1];
       }
      }
      sum = sqrt(pow(sum1,2) + pow(sum2,2));
      G[r*COLS+c] = sum;
    }
  }

  max= G[0];

  for (i=1; i<=ROWS*COLS; i++)
  {
   if(G[i]>max)
    {
      max= G[i];
    }
  }

  min= G[0];

  for (i=1; i<ROWS*COLS; i++)
  {
   if(G[i]<min)
    {
      min= G[i];
    }
  }
  g=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  for (i=0; i<ROWS*COLS; i++)
 {
  g[i] = (((G[i]-min)*255)/(max-min));
 }

  fpt2=fopen("sobelhawk.ppm","w");
  fprintf(fpt2,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(g,COLS*ROWS,1,fpt2);
  fclose(fpt2);

 /*Reading Contour points file*/
 hawk_init = fopen("hawk_init.txt", "r");
 char columns1[20] = "", rows1[20] ="";
 int columns2[42], rows2[42], Y[43], X[43], iterations, j;
 i=0;
 while ( !feof(hawk_init))
   {
     fscanf(hawk_init,"%s %s\n", columns1, rows1);
     columns = atoi(columns1);
     rows = atoi(rows1);
     columns2[i] = columns;
     rows2[i] = rows;
     i++;
   }
fclose(hawk_init);
int num=sizeof(columns2)/sizeof(columns2[0]);
for(i=0;i<num;i++)
 {
   Y[i] = columns2[i];
   X[i] = rows2[i];
 }
 Y[42] = Y[0]; //completing the contour rubber band
 X[42] = X[0];
/*Marking + sign on contour points*/
output1=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

for (r=1; r<ROWS-1; r++)
 {
  for (c=1; c<COLS-1; c++)
  {
    output1[r*COLS + c] = orighawk[r*COLS + c];
  }
 }
num= sizeof(Y)/sizeof(Y[0]);
for(i=0;i<num-1;i++)
{
  r3 = X[i];
  c3 = Y[i];
  for(r4=-3; r4<=3; r4++)
   {
     output1[(r3+r4)*COLS + c3] = 0;
   }
  for(c4=-3; c4<=3; c4++)
   {
      output1[r3*COLS + c3+c4] = 0;
   }
 }

 /*Contour plotting*/
 fpt1=fopen("output1.ppm","w");
 fprintf(fpt1,"P5 %d %d 255\n",COLS,ROWS);
 fwrite(output1,1,ROWS*COLS,fpt1);
 fclose(fpt1);
 double avgdist;
 int d, dtotal, davg, r8, c8, r7, c7, r6, c6, r5, c5, r4, c4, r0, c0, j, k;
 int minima, c8minima, r8minima, pos, cpos, rpos, value, maximum, minimum;
 Eint=(unsigned int *)calloc(7*7,sizeof(unsigned int));
 Eext=(unsigned int *)calloc(7*7,sizeof(unsigned int));
 Eint2=(unsigned int *)calloc(7*7,sizeof(unsigned int));
 Eimg=(unsigned int *)calloc(7*7,sizeof(unsigned int));
 Eintnorm=(unsigned int *)calloc(7*7,sizeof(unsigned int));
Eint2norm=(unsigned int *)calloc(7*7,sizeof(unsigned int));

 for(iterations=0;iterations<=30;iterations++)
 {
   dtotal=0;
     for(i=1;i<n;i++)
     {
       r5 = r4[i];
       c5 = c4[i];
       r6 = r4[i-1];
       c6 = c4[i-1];
       d[i-1] = sqrt(pow((r5-r6),2)+pow((c5-c6),2));
       dtotal += d[i-1];
    }
    davg[iterations]=dtotal/42;

    for(j=0;j<num;j++)
    {
      r7 = r4[j];
      c7 = c4[j];
      for(r8=-3; r8<=3; r8++)
      {
        for(c8=-3; c8<=3; c8++)
        {
        Eint[(r8+3)*7+(c8+3)] = pow(((c7+c8)-c4[j+1]),2) + pow(((r7+r8)-r4[j+1]),2);
        Eext[(r8+3)*7+(c8+3)] = - g[(r7+r8)*COLS+(c7+c8)];
        Eint2[(r8+3)*7+(c8+3)] = pow((davg[iterations] - sqrt(pow((r7-r4[j+1]),2) + pow((c7-c4[j+1]),2))),2);
        }
      }
      /*Normalizing energy values*/
          maximum= Eint[0];
          for(r=0;r<49;r++)
          {
            if(Eint[r]>maximum)
            {
               maximum=Eint[r];
            }
          }
          minimum = Eint[0];
          for(r=0;r<49;r++)
          {
            if(Eint[r]<minimum)
            {
               minimum=Eint[r];
            }
          }
          for(r=0;r<49;r++)
          {
          value= ((float)(Eint[r]-minimum))/(maximum-minimum) ;
          Eintnorm[r]=value;
          }

          maximum= Eint2[0];
          for(r=0;r<49;r++)
          {
            if(Eint2[r]>maximum)
            {
               maximum=Eint2[r];
            }
          }
          minimum = Eint[0];
          for(r=0;r<49;r++)
          {
            if(Eint[r]<minimum)
            {
               minimum=Eint2[r];
            }
          }
          for(r=0;r<49;r++)
          {
          value= ((float)(Eint2[r]-minimum))/(maximum-minimum) ; Eint2norm[r]=value;
          }
/* Sum of energies*/
          for(r8=-3; r8<=3; r8++)
          {
            for(c8=-3; c8<=3; c8++)
            {
            Eimg[(r8+3)*7+(c8+3)] = Eintnorm[(r8+3)*7+(c8+3)] + Eext[(r8+3)*7+(c8+3)] + Eint2norm[(r8+3)*7+(c8+3)];
            }
          }
      minima = Eimg[0];
      for(k=1;k<49;k++){
        if(Eimg[k]<minima){
          minima = Eimg[k];
          pos = k;
        }
      }

      c8minima = (pos)%7;
      r8minima = (pos-c8)/7;
      rpos[j] = r7 + (-3+r8minima);
      cpos[j] = c7 + (-3+c8minima);
    }

    for(i=0;i<num;i++)
    {
      r4[i]=rpos[i];
      c4[i]=cpos[i];
    }

  }
  output2=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
  for (r2=1; r2<ROWS-1; r2++)
  {
    for (c2=1; c2<COLS-1; c2++)
    {
      output2[r2*COLS + c2] = orighawk[r2*COLS + c2];
    }
  }

  for(i=0;i<n-1;i++)
  {
    r4 = rpos[i];
    c4 = cpos[i];
    for(r8=-3; r8<=3; r8++)
    {
        output2[(r4+r8)*COLS + c4] = 0;
    }
    for(c8=-3; c8<=3; c8++)
    {
        output2[r4*COLS + c4+c8] = 0;
    }
  }
  fpt=fopen("output2.ppm","w");
  fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
  fwrite(output2,1,ROWS*COLS,fpt);
  fclose(fpt);
}
