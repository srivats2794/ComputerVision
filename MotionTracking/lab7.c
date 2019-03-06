
/* LAB 7: MOTION TRACKING */
/*SRIVATSAN SRINIVASAN*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

main()
{
  FILE *fpt;
  float a, b, c, d, e, f, g, t[1250], accX[1250], accY[1250], accZ[1250], pitch[1250], roll[1250], yaw[1250];
  float meanaccX, meanaccY, meanaccZ, newaccX, newaccY, newaccZ, variance1, variance2, variance3, SDacc[1250];
  float SDgyro[1250], meanpitch, meanroll,meanyaw,newpitch,newroll,newyaw;
  float variance5, variance6;
  int window=10;
 //READING DATA (ALREADY SMOOTHED)
 fpt = fopen("acc_gyro_smooth.txt", "r");
 int i=0, j=0, k=0;
 while ( !feof(fpt))
   {
    fscanf(fpt,"%f %f %f %f %f %f %f\n", a, b, c, d, e, f, g);
     t[i] = a;
     accX[i]=b; accY[i]=c; accZ[i]=d;
     pitch[i]=e;roll[i]=f;yaw[i]=g;
     i++;
   }
fclose(fpt);

//VARIANCE AND STANDARD DEVIATION FOR ACCELERATION DATA (WINDOW SIZE 10)
for(j = 0; j<i; j+=window)
{
		newaccX = 0; newaccY = 0; newaccZ = 0;
		meanaccX = 0; meanaccY = 0; meanaccZ = 0;

    for(k= 0; k<=window; k++)
    {
			newaccX += accX[j+k]; newaccY += accY[j+k]; newaccZ += accZ[j+k];
		}
		meanaccX = newaccX/(float)window; meanaccY = newaccY/(float)window; meanaccZ = newaccZ/(float)window;

		for(k = 0; k<=window; k++)
    {
			variance1 += (powf((accX[j+k]-meanaccX),2.0))/window;
			variance2 += (powf((accY[j+k]-meanaccY),2.0))/window;
			variance3 += (powf((accZ[j+k]-meanaccZ),2.0))/window;
		}
    for(k = 0; k<=window; k++)
    {
		SDacc[j+k] = sqrt(variance1)+sqrt(variance2)+sqrt(variance3);
    }
}
//VARIANCE AND STANDARD DEVIATION FOR GYROSCOPE DATA (WINDOW SIZE 10)
j=0, k=0;

for(j = 0; j<i; j+=window)
{
		newpitch = 0; newroll = 0; newyaw = 0;
		meanpitch = 0; meanroll = 0; meanyaw = 0;

    for(k= 0; k<=window; k++)
    {
			newpitch += pitch[j+k]; newroll += roll[j+k]; newyaw += yaw[j+k];
		}
		meanpitch = newpitch/(float)window; meanroll = newroll/(float)window; meanyaw = newyaw/(float)window;

		for(k = 0; k<window; k++)
    {
			variance4 += (powf((pitch[j+k]-meanpitch),2.0))/window;
			variance5 += (powf((roll[j+k]-meanroll),2.0))/window;
			variance6 += (powf((yaw[j+k]-meanyaw),2.0))/window;
		}

    for(k = 0; k<=window; k++)
    {
		SDgyro[j+k] = sqrt(variance4)+sqrt(variance5)+sqrt(variance6);
    }
}
printf("\nThe 6 variances are %f %f %f %f %f %f\n",variance1,variance2,variance3,variance4,variance5,variance6);
}

//THRESHOLDING TO TRACK MOVEMENT AREAS
float Tacc[1250], Tgyro[1250];
float ULacc= 0.0085, LLacc= 0.0015, ULgyro = 0.0023, LLgyro = 0.016;

for(j=0;j<i;j++)
{
  Tacc[j]=1; Tgyro[j]=1;
}
for(j=0; j<i; j++)
{
  if(SDacc[j]<LLacc)
		{
			Tacc[j] = 1;
		}
	else if(SDacc[j]>ULacc)
		{
			Tacc[j] = 0;
		}
	else if(SDgyro[j]<LLgyro)
		{
			Tgyro[j] = 1;
		}
	else if(SDgyro[j]>ULgyro)
		{
			Tgyro[j] = 0;
		}
}

for(j=0; j<i-1; j++)
{
		if(Tacc[i]==0 && SDacc[j+1]<ULacc)
		{
			Tacc[i+1] = 1;
		}
		if(Tgyro[j]==0 && SDgyro[j+1]<ULgyro)
		{
			Tgyro[j+1] = 1;
		}
}

//INTEGRATION OVER TIME

float DELtime = 0.05;
int accINIT = 0, gyroINIT = 0, accSTART[1250], accEND[1250], gyroSTART[1250], gyroEND[1250];

	for(j = 0; j<i-1; j++)
  {
	 if(Tacc[j]==1 && Tacc[j+1]==0)
		{
			accSTART[accINIT] = j+1;
		}
	 if(Tacc[j]==0 && Tacc[j+1]==1)
		{
			accEND[accINIT] = j+1;
			accINIT++;
		}

		if(Tgyro[j]==1 && Tgyro[j+1]==0)
		{
			gyroSTART[gyroINIT] = j+1;
		}
		if(Tgyro[j]==0 && Tgyro[j+1]==1)
		{
			gyroEND[gyroINIT] = ;
			gyroINIT++;
		}
	}
 float dispX[1250], dispY[1250], dispZ[1250], disppitch[1250], disproll[1250], dispyaw[1250];
 float vaccX, vaccY, vaccZ, vaccXavg, vaccYavg, vaccZavg, aaccx, aaccy, aaccz;

  for(k = 0; k<12; k++)
  {
		dispX[k]=0.0; dispY[k]=0.0; dispZ[k]=0.0;
		disppitch[k]=0.0; disproll[k]=0.0; dispyaw[k]=0.0;
		vaccX = 0.0; vaccY = 0.0; vaccZ = 0.0;
		vaccXavg = 0.0; vaccYavg = 0.0; vaccZavg = 0.0;
		aaccX = 0.0; aaccY = 0.0; aaccZ = 0.0;

    for(j = accSTART[k]; j<=accEND[k]; j++)
    {
			vaccXavg = ((9.81*(accX[j] - aaccX)*DELtime) - vaccX)/2;
			vaccX = ((accX[j] - aaccX)*DELtime);
			dispX[k] = vaccXavg*DELtime + dispX[k];
			aaccX = accX[j];

			vaccYavg = ((9.81*(accY[j] - aaccY)*DELtime) - vaccY)/2;
			vaccY = ((accY[j] - aaccY)*DELtime);
			dispY[k] = vaccYavg*DELtime + dispY[k];
			aaccY = accY[j];

			vaccZavg = ((9.81*(accZ[j] - aaccZ)*DELtime) - vaccZ)/2;
			vaccZ = ((accZ[j] - aaccZ)*DELtime);
			dispZ[k] = vaccZavg*DELtime + dispZ[k];
			aaccZ = accZ[j];
		}

		for(j = gyroSTART[k]; j<=gyroEND[k]; j++)
                {
			disppitch[k] = pitch[j]*DELtime + disppitch[k];
			disproll[k] = roll[j]*DELtime + disproll[k];
			dispyaw[k] = yaw[j]*DELtime + dispyaw[k];
		}

	}



//DISPLACEMENT
int ACCstart, ACCend, GYROstart, GYROend;
float  ACC_ST, ACC_ET, GYRO_ST, GYRO_ET, DISPx, DISPy, DISPz, DISPpitch, DISProll, DISPyaw;
	fpt=fopen("solution.txt","w");

	for(j = 0; j<12; j++)
  {
		ACCstart = (float)accSTART[j];
		ACCend = (float)accEND[j];
		ACC_ST = time[(int)ACCstart];
		ACC_ET = time[(int)ACCend];

		GYROstart = (float)gyroSTART[j];
		GYROend = (float)gyroEND[j];
		GYRO_ST = time[(int)GYROstart];
		GYRO_ET = time[(int)GYROend];

		DISPx = dispX[j];
		DISPy = dispY[j];
		DISPz = dispZ[j];
		DISPpitch = disppitch[j];
		DISProll = disproll[j];
		DISPyaw = dispyaw[j];

		fprintf(fpt,"%d %d %lf %lf %lf %lf %lf %d %d %lf %lf %lf %lf %lf\n",(int)ACCstart,(int)ACCend,ACC_ST,ACC_ET,DISPx,DISPy,DISPz,(int)GYROstart,(int)GYROend,GYRO_ST,GYRO_ET,pch,rl,yw);
	}
	fclose(fpt);
}
