#include <stdio.h>
#include <stdlib.h>
#include <time.h>


main()
{
FILE		*fpt1, *fpt2, *fpt3;
unsigned char	*image, *template, *nmsf, *binary;
float           *zmct;
float           *msf;
char		header1[320], header2[320], alph, ch;
int		ROWS1,COLS1,BYTES1, ROWS2, COLS2, BYTES2, i, r, c, rt, ct, sum, num, Threshold, yes, no, tp, fp, tn, fn, j, k, flag;
float           mean, max, min, sum2, tpr, fpr;

	
	/* read image */
if ((fpt1=fopen("parenthood.ppm","rb")) == NULL)
  {
  printf("Unable to open image for reading\n");
  exit(0);
  }
fscanf(fpt1,"%s %d %d %d\n",header1,&COLS1,&ROWS1,&BYTES1);

image=(unsigned char *)calloc(ROWS1*COLS1,sizeof(unsigned char));
//header1[0]=fgetc(fpt1);	/* read white-space character that separates header */
fread(image,1,COLS1*ROWS1,fpt1);
fclose(fpt1);

      /* read template */

if ((fpt2=fopen("template.ppm","rb")) == NULL)
  {
  printf("Unable to open template for reading\n");
  exit(0);
  }
fscanf(fpt2,"%s %d %d %d\n",header2,&COLS2,&ROWS2,&BYTES2);

template=(unsigned char *)calloc(ROWS2*COLS2,sizeof(unsigned char));
//header2[0]=fgetc(fpt2);	/* read white-space character that separates header */
fread(template,1,COLS2*ROWS2,fpt2);
fclose(fpt2);

     /* mean of the template */
sum =0;

for (i=0; i<=ROWS2*COLS2; i++)
 {
    sum+= template[i];
 }

mean = (float)sum/135;

  /*memory allocation for zero mean centered template*/

zmct=(float *)calloc(ROWS2*COLS2,sizeof(float));

  /* zero mean centering the template */

for (i=0; i<=ROWS2*COLS2; i++)
 {
    zmct[i] = template[i]-mean;
 }

   /* Memory allocation for MSF image */

msf=(float *)calloc(ROWS1*COLS1,sizeof(float));

   /* Matched Spatial Filtering */
for (r=7; r<=ROWS1-8; r++){
 for (c=4; c<=COLS1-5; c++)
  {
   sum2=0; 
   for (rt=-7; rt<=7; rt++){
    for(ct=-4; ct<=4; ct++)
    {
     sum2+=image[((r+rt)*COLS1+(c+ct))]*zmct[((rt+7)*COLS2+(ct+4))];
    }
    }
    msf[r*COLS1+c]=sum2;
  }
}

  /* Identifying Maximum and Minimum pixel values of the MSF image */

max= msf[0];

for (i=1; i<=ROWS1*COLS1; i++)
{ 
 if(msf[i]>max)
  {
    max= msf[i];
  }
}

min= msf[0];

for (i=1; i<ROWS1*COLS1; i++)
{ 
 if(msf[i]<min)
  {
    min= msf[i];
  }
}

   /* Memory allocation for normalized MSF */

nmsf=(unsigned char *)calloc(ROWS1*COLS1,sizeof(unsigned char));

for (i=0; i<ROWS1*COLS1; i++)
 {
  nmsf[i] = (((msf[i]-min)*255)/(max-min));
 }

     /* write out NMSF image to see result */

fpt1=fopen("nmsf.ppm","w");
fprintf(fpt1,"P5 %d %d 255\n",COLS1,ROWS1);
fwrite(nmsf,COLS1*ROWS1,1,fpt1);
fclose(fpt1);

 
          /*Binary images for various thresholds*/
binary=(unsigned char *)calloc(ROWS1*COLS1,sizeof(unsigned char));

Threshold=210;
  for (i=0; i<ROWS1*COLS1; i++)
  {
    if (nmsf[i]>Threshold)
    {
      binary[i] = 255;
    }
    if (nmsf[i]<=Threshold)
    {
      binary[i] = 0;
    }
  }  
  fpt2=fopen("binary image.ppm","w");
  fprintf(fpt2,"P5 %d %d 255\n",COLS1,ROWS1);
  fwrite(binary,COLS1*ROWS1,1,fpt2);
  fclose(fpt2);
 

    /* Reading ground truth file amd counting number of letters */

fpt3 = fopen("gt.txt", "r");
    if (fpt3 == NULL) {
        printf("Cannot open %s\n", "gt.txt");
        exit(0);
    }

num=0;

    while(1)
    {
        alph= fgetc(fpt3);
        if((alph>='a' && alph<='z') || (alph>='A' && alph<='Z'))
        {
           num++;
        }
        
        if (alph == EOF)
        break;
    }

printf("Number of letters in parenthood.ppm is %d\n",num);

fclose(fpt3);

         /* Checking */

fpt3=fopen("gt.txt","r");
char ch1;
int a,b;
int discard1=0,discard2=0;
tp=0;fp=0;
yes=0;no=0;
rewind(fpt3);
for(i=0;i<num;i++)
{
  fscanf(fpt3,"%c",&ch1);
  fscanf(fpt3,"%d %d",&a,&b);
  fseek(fpt3, 1, SEEK_CUR);
  if(ch1!='e')
    {
     no=no+1;
       for(j=a-5;j<a+4;j++)
       {
         flag=0;
           if(a>563 || b>643 && a<5 || b<7)
           {
           discard1= discard1+1;
           break;
           }
           for(k=b-7;k<=b+7;k++)
           {
             if(nmsf[j*COLS1+k]>Threshold)
              {
               fp=fp+1;
               flag=1;
               break;
              }
           }
           if(flag==1)
           break;
      }
    }
  
  if(ch1=='e')
   {
    yes=yes+1;
     for(j=a-5;j<a+4;j++)
       {
        flag=0;
        if(a>563 || b>643 && a<5 || b<7)
          {
           discard2= discard2+1;
           break; 
          }
           for(k=b-7;k<=b+7;k++)
             {
              if(nmsf[j*COLS1+k]>Threshold)
                {
                 tp=tp+1;
                 flag=1;
                 break;
                }
             }
             if(flag==1)
             break;
       }
   }
}
printf("Total number of e's on file: %d and not e's: %d\n",yes,no);
printf("True Positives:%d False Positives:%d\n",tp,fp);
printf("Number of discarded value of other alphabet and e %d & %d\n",discard1,discard2);
fn=(yes-discard2)-tp;
tn=(no-discard1)-fp;
printf("Number of true negative and False negative are %d & %d\n",tn,fn);
tpr= ((float)tp)/(tp+fn);
fpr= ((float)fp)/(fp+tn);
printf("TPR and FPR are %f & %f\n",tpr,fpr);
fclose(fpt3);
}











