/*LAB 3: LETTERS */


#include <stdio.h>
#include <stdlib.h>

main()
{
FILE		*fpt1, *fpt2, *fpt3, *fpt4;
unsigned char	*image,*nmsf, *temp, *binary, *random;
int		ROWS1,COLS1,BYTES1,j=0;
char		header1[320], header2[320];

/* read image */
if ((fpt1=fopen("parenthood.ppm","rb")) == NULL)
  {
  printf("Unable to open image for reading\n");
  exit(0);
  }
fscanf(fpt1,"%s %d %d %d\n",header1,&COLS1,&ROWS1,&BYTES1);

image=(unsigned char *)calloc(ROWS1*COLS1,sizeof(unsigned char));
fread(image,1,COLS1*ROWS1,fpt1);
fclose(fpt1);

/* read normalized MSF image */

if ((fpt2=fopen("nmsf.ppm","rb")) == NULL)
  {
  printf("Unable to open NMSF image for reading\n");
  exit(0);
  }
fscanf(fpt2,"%s %d %d %d\n",header1,&COLS1,&ROWS1,&BYTES1);

nmsf=(unsigned char *)calloc(ROWS1*COLS1,sizeof(unsigned char));
fread(nmsf,1,COLS1*ROWS1,fpt2);
fclose(fpt2);

/* reading GROUNDTRUTH file */

fpt3 = fopen("gt.txt", "r");
if (fpt3 == NULL)
  {
  printf("Cannot open %s\n", "gt.txt");
  exit(0);
  }

char alph;
int num=0, column[1262], row[1262];

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

rewind(fpt3);

int TP=0, FP=0, TN=0, FN=0, neighbor;
float tpr=0, fpr=0;
int yes=0, no=0, inter;
int r1, c1, r2, c2, i,r3,c3;
char ch, letters[1262];
for(i=0;i<num;i++)
{
  fscanf(fpt3,"%c",&ch);
  fscanf(fpt3,"%d %d",&c1,&r1);
  fseek(fpt3, 1, SEEK_CUR);
  letters[i]=ch; column[i]=c1; row[i]= r1;
  //printf("%d", c2);
  if(ch!='e')
    {
     no=no+1;
    }

  if(ch=='e')
   {
    yes=yes+1;
   }
}
printf("Total number of e's on file: %d and not e's: %d\n",yes,no);

fclose(fpt3);

       /*Operations */

int T= 195, ROWS2=15, COLS2=9, flag,r=0,c=0, N=0, S=0, E=0, W=0, count, edge, pass,end,branch;
printf("Threshold:%d\n",T);
temp=(unsigned char *)calloc(ROWS2*COLS2,sizeof(unsigned char)); //Allocation of memory for a temporary image for copying 15x9 blocks of image
binary=(unsigned char *)calloc(ROWS2*COLS2,sizeof(unsigned char)); //Allocation for binary image
random = (unsigned char *)calloc(ROWS2*COLS2, sizeof(unsigned char));
for (i=0; i<num; i++)
{
    flag=0;
    r1 = row[i];
    c1 = column[i];
    ch= letters[i];
       for (r2=-7; r2<=7; r2++)
        {
         for (c2=-4; c2<=4; c2++)
          {
          if(nmsf[(r1+r2)*COLS1+(c1+c2)]>T) // checking for pixels greater than threshold
            {
              flag=flag+1;
            }
          }
        }

    if(flag>=1)
     {
      for (r3=-7; r3<=7; r3++)
        {
         for (c3=-4; c3<=4; c3++)
          {
           temp[(r3+7)*COLS2+(c3+4)]=image[(r1+r3)*COLS1+(c1+c3)]; //copying from image
          }
        }
      }

  /* Making it a binary image */
       for (j=0; j<ROWS2*COLS2; j++)
          {
           if (temp[j]>128)
            {
             binary[j] = 255;
             random[j] =255;
            }
           else if (temp[j]<=128)
            {
             binary[j] = 0;
             random[i]=0;
            }
           }
       /* Checking for edges and neighbors and marking for erasure */
       for (r=1;c<ROWS2-1; r++)
        {
        for (c=1;c<COLS2-1;c++)
          {
           if(binary[r*COLS2+c]==0)
             {
              edge=0;
              neighbor=0, N=0, S=0, E=0, W=0;
              count=binary[(r-1)*COLS2+(c-1)]- binary[(r-1)*COLS2+c];
               if(count<0)
                {
                 edge=edge+1;
                }
              count=binary[(r-1)*COLS2+c]- binary[(r-1)*COLS2+(c+1)];
               if(count<0)
                {
                 edge=edge+1;
                }
              count=binary[(r-1)*COLS2+(c+1)]- binary[r*COLS2+(c+1)];
               if(count<0)
                {
                 edge=edge+1;
                }
              count=binary[r*COLS2+(c+1)]- binary[(r+1)*COLS2+(c+1)];
               if(count<0)
                {
                 edge=edge+1;
                }
              count=binary[(r+1)*COLS2+(c+1)]- binary[(r+1)*COLS2+c];
               if(count<0)
                {
                 edge=edge+1;
                }
              count= binary[(r+1)*COLS2+c]- binary[(r+1)*COLS2+(c-1)];
               if(count<0)
                {
                 edge=edge+1;
                }
              count=binary[(r+1)*COLS2+(c-1)]- binary[r*COLS2+(c-1)];
               if(count<0)
                {
                 edge=edge+1;
                }
              count=binary[r*COLS2+(c-1)]-binary[(r-1)*COLS2+(c-1)];
               if(count<0)
                {
                 edge=edge+1;
                }

               if (binary[r*COLS2+c] == binary[r*COLS2 + (c + 1)])
	             {
		            neighbor += 1;
		            E = 1;
               }
         	    if (binary[r*COLS2 + c] == binary[(r + 1)*COLS2 + (c + 1)])
		            {neighbor += 1;}
		          if (binary[r*COLS2 + c] == binary[(r + 1)*COLS2 + c])
		          {
		           neighbor += 1;
		           S = 1;
		          }
		         if (binary[r*COLS2 + c] == binary[(r + 1)*COLS2 + (c - 1)])
		          {neighbor += 1;}
		         if (binary[r*COLS2 + c] == binary[r*COLS2 + (c - 1)])
		          {
		           neighbor += 1;
		           W = 1;
		          }
		         if (binary[r*COLS2 + c] == binary[(r - 1)*COLS2 + (c - 1)])
		          {neighbor += 1;}
		        if (binary[r*COLS2 + c] == binary[(r - 1)*COLS2 + c])
		         {
		          neighbor += 1;
		          N = 1;
		         }
		       if (binary[r*COLS2 + c] == binary[(r - 1)*COLS2 + (c + 1)])
		         {neighbor += 1;}
           pass=0;
           if (N != 1 || E!= 1 || (S!= 1 && W!= 1))
     				{
     					pass = 1;
     				}
           if (edge == 1 && neighbor >= 3 && neighbor <= 7 && pass == 1)
    				{
    					random[r*COLS2 + c] = 100; // marking using random value assignment
    				}
          }
        }
      }
      fpt4=fopen("binary image.ppm","w");
      fprintf(fpt4,"P5 %d %d 255\n",COLS2,ROWS2);
      fwrite(binary,COLS2*ROWS2,1,fpt4);
      fclose(fpt4);

      for (r = 1; r < ROWS2; r++)
       {
        for (c = 1; c < COLS2; c++)
         {
          if (random[r*COLS2 + c] == 100)
           {
            binary[r*COLS2 + c] = 255; //Deletion of marked pixels
           }
         }
       }
  /* Checking for branchpoints and edges */
      end=0; branch=0;
      for (r= r1-7; r < r1 + 7; r++)
       {
        for (c = c1-4; c < c1 + 4; c++)
         {
          inter = 0;
         if (r*COLS2 + c <= COLS2*ROWS2 && binary[r*COLS2 + c] == 0)
         {
           if (binary[r*COLS2 + (c + 1)] - binary[(r + 1)*COLS2 + (c + 1)] < 0)
             inter += 1;

           if (binary[(r + 1)*COLS2 + (c + 1)] - binary[(r + 1)*COLS2 + c] < 0)
             inter += 1;

           if (binary[(r + 1)*COLS2 + c] - binary[(r + 1)*COLS2 + (c - 1)] < 0)
             inter += 1;

           if (binary[(r + 1)*COLS2 + (c - 1)] - binary[r*COLS2 + (c - 1)] < 0)
             inter += 1;

           if (binary[r*COLS2 + (c - 1)] - binary[(r - 1)*COLS2 + (c - 1)] < 0)
             inter += 1;

           if (binary[(r - 1)*COLS2 + (c - 1)] - binary[(r - 1)*COLS2 + c] < 0)
             inter += 1;

           if (binary[(r - 1)*COLS2 + c] - binary[(r - 1)*COLS2 + (c + 1)] < 0)
             inter += 1;

           if (binary[(r - 1)*COLS2 + (c + 1)] - binary[r*COLS2 + (c + 1)] < 0)
             inter += 1;

           if (inter == 1)
             end += 1;

           else if (inter > 2)
             branch += 1;
         }
        }
      }
  /* Checking for TP and FP */
if(ch=='e' && end==1 && branch==1)
{TP=TP+1;}
else if(ch=='e' && end!=1 || branch!=1)
{FN=FN+1;}
else if(ch!='e' && end==1 && branch==1)
{FP=FP+1;}
else if(ch!='e' && end!=1 || branch!=1)
{TN=TN+1;}
}
printf("True Positives:%d False Positives:%d\n",TP,FP);
printf("True Negatives:%d False Negatives:%d\n",TN,FN);
tpr=(float)TP/(TP+FN);
fpr=(float)FP/(FP+TN);
printf("TPR:%f FPR:%f\n",tpr,fpr);
  fpt4=fopen("binary image.ppm","w");
  fprintf(fpt4,"P5 %d %d 255\n",COLS2,ROWS2);
  fwrite(binary,COLS2*ROWS2,1,fpt4);
  fclose(fpt4);
}
