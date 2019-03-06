/* 
This code was developed to take in an image of a paragraph and a 
template image of a letter and apply an MSF in order to detect the
instances of the letter.  This code brings in a ground truth data
and generates an ROC curve for evaluating the detection performance
at different threshold values.
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 


int main (int argc, char *argv[])
{
	
	FILE	*fpt;
	
	int		ROWS,COLS,BYTES;
	char	h1[20],h2[20],h3[20],h4[20],h5[20],h6[20],h7[20];
	float	*time;
	float	*accX,*accY,*accZ,*pitch,*roll,*yaw;
	float	tm,aX,aY,aZ,pch,rl,yw;
	float	maX,maY,maZ,mpch,mrl,myw;
	int		*threshacc;
	int		*threshgyro;
	float	TLacc,THacc,TLgyro,THgyro;
	
	float	*saccX,*saccY,*saccZ,*spitch,*sroll,*syaw;
	float	*vaccX,*vaccY,*vaccZ,*vpitch,*vroll,*vyaw;
	float	*acc_ssd, *gyro_ssd;
	int		smooth_win,var_win,gsmooth_win,gvar_win;
	
	int		i,j,k,sum,sum2,index,num_ind,max_len;
	
	time=(float *)calloc(1250,sizeof(float));
	accX=(float *)calloc(1250,sizeof(float));
	accY=(float *)calloc(1250,sizeof(float));
	accZ=(float *)calloc(1250,sizeof(float));
	pitch=(float *)calloc(1250,sizeof(float));
	roll=(float *)calloc(1250,sizeof(float));
	yaw=(float *)calloc(1250,sizeof(float));
	
	saccX=(float *)calloc(1250,sizeof(float));
	saccY=(float *)calloc(1250,sizeof(float));
	saccZ=(float *)calloc(1250,sizeof(float));
	spitch=(float *)calloc(1250,sizeof(float));
	sroll=(float *)calloc(1250,sizeof(float));
	syaw=(float *)calloc(1250,sizeof(float));
	
	acc_ssd=(float *)calloc(1250,sizeof(float));
	gyro_ssd=(float *)calloc(1250,sizeof(float));
	
	vaccX=(float *)calloc(1250,sizeof(float));
	vaccY=(float *)calloc(1250,sizeof(float));
	vaccZ=(float *)calloc(1250,sizeof(float));
	vpitch=(float *)calloc(1250,sizeof(float));
	vroll=(float *)calloc(1250,sizeof(float));
	vyaw=(float *)calloc(1250,sizeof(float));
	
	threshacc=(int *)calloc(1250,sizeof(int));
	threshgyro=(int *)calloc(1250,sizeof(int));

smooth_win = 11;
	var_win = 11;
	gsmooth_win = 15;
	gvar_win = 11;
	num_ind = (int)floor(1250.0/(float)var_win);
	
	max_len = num_ind*var_win;		
	
fpt = fopen("acc_gyro_smooth.txt", "r");
i=0; j=0; k=0;
 while ( !feof(fpt))
   {
    fscanf(fpt,"%f %f %f %f %f %f %f\n",&tm,&aX,&aY,&aZ,&pch,&rl,&yw);
     time[i] = tm;
     saccX[i]=aX; saccY[i]=aY; saccZ[i]=aZ;
     spitch[i]=pch;sroll[i]=rl;syaw[i]=yw;
     i++;
   }
fclose(fpt);
	
	for(i = 0; i<max_len; i+=var_win){
		aX = 0;
		aY = 0;
		aZ = 0;
		
		maX = 0;
		maY = 0;
		maZ = 0;
		for(j = 0; j<var_win; j++){
			aX += saccX[i+j];
			aY += saccY[i+j];
			aZ += saccZ[i+j];
		}
		
		maX = aX/(float)var_win;
		maY = aY/(float)var_win;
		maZ = aZ/(float)var_win;
		
		aX = 0;
		aY = 0;
		aZ = 0;
		
		for(j = 0; j<var_win; j++){
			aX += powf((saccX[i+j]-maX),2.0);
			aY += powf((saccY[i+j]-maY),2.0);
			aZ += powf((saccZ[i+j]-maZ),2.0);
		}
		for(j = 0; j<var_win; j++){
			
			acc_ssd[i+j] = sqrt((aX/(var_win-1))) + sqrt((aY/(var_win-1))) + sqrt((aZ/(var_win-1)));
		}
		
		
	}
	
	for(i = 0; i<max_len; i+=var_win){
		pch = 0;
		rl = 0;
		yw = 0;
		
		mpch = 0;
		mrl = 0;
		myw = 0;
		for(j = 0; j<var_win; j++){
			pch += spitch[i+j];
			rl += sroll[i+j];
			yw += syaw[i+j];			
		}
		
		mpch = pch/(float)var_win;
		mrl = rl/(float)var_win;
		myw = yw/(float)var_win;
		
		pch = 0;
		rl = 0;
		yw = 0;
		
		for(j = 0; j<var_win; j++){
			pch += powf((spitch[i+j]-mpch),2.0);
			rl += powf((sroll[i+j]-mrl),2.0);
			yw += powf((syaw[i+j]-myw),2.0);
		}
		for(j = 0; j<var_win; j++){
			
			gyro_ssd[i+j] = sqrt((pch/(var_win-1))) + sqrt((rl/(var_win-1))) + sqrt((yw/(var_win-1)));
		}
		
		
	}
	printf("\nThe six variances: %f %f %f %f %f %f\n", aX, aY, aZ, pch, rl, yw);
	//
		
	fpt=fopen("s_acc_gyro.txt","w");
	fprintf(fpt,"%s %s %s %s %s %s %s\n",h1,h2,h3,h4,h5,h6,h7);
	for(k = 0; k<1250; k++){
		tm = time[k];
		aX = saccX[k];
		aY = saccY[k];
		aZ = saccZ[k];
		pch = spitch[k];
		rl = sroll[k];
		yw = syaw[k];
		
		fprintf(fpt,"%f %f %f %f %f %f %f\n",tm,aX,aY,aZ,pch,rl,yw);
		
	}
	fclose(fpt);
	
	fpt=fopen("ssds.txt","w");
	fprintf(fpt,"%s %s %s\n",h1,"acc_ssd","gyro_ssd");
	for(k = 0; k<max_len; k++){
		tm = time[k];
		aX = acc_ssd[k];
		aY = gyro_ssd[k];
		
		fprintf(fpt,"%f %f %f\n",tm,aX,aY);
		
	}
	fclose(fpt);
	
	TLacc = 0.0015;
	THacc = 0.0085;
	TLgyro = 0.0023;
	THgyro = 0.016;

	
	for(i = 0; i<max_len; i++){
		threshacc[i] = 1;
		threshgyro[i] = 1;
	}
	
	for(i=0; i<max_len; i++){
		if(acc_ssd[i]<TLacc)
		{
			threshacc[i] = 1;
		}
		if(acc_ssd[i]>THacc)
		{
			threshacc[i] = 0;
		}
		if(gyro_ssd[i]<TLgyro)
		{
			threshgyro[i] = 1;
		}
		if(gyro_ssd[i]>THgyro)
		{
			threshgyro[i] = 0;
		}
	}
	
	for(i=0; i<max_len-1; i++){
		if(threshacc[i]==0 && acc_ssd[i+1]<THacc)
		{
			threshacc[i+1] = 1;
		}
		if(threshgyro[i]==0 && gyro_ssd[i+1]<THgyro)
		{
			threshgyro[i+1] = 1;
		}
	}
	
	/*
	for(i=1; i<max_len-1; i++){
		sum = threshacc[i-1]+threshacc[i]+threshacc[i+1];
		sum2 = threshgyro[i-1]+threshgyro[i]+threshgyro[i+1];
		if(sum>1)
		{
			threshacc[i] = 1;
		}
		else
		{
			threshacc[i] = 0;
		}
		if(sum2>1)
		{
			threshgyro[i] = 1;
		}
		else
		{
			threshgyro[i] = 0;
		}
	}
	*/
	
	fpt=fopen("seg_acc_v2.txt","w");
	fprintf(fpt,"%s %s %s %s %s\n",h1,"acc_segments","acc_ssd","gyro_segments","gyro_ssd");
	for(k = 0; k<max_len; k++){
		tm = time[k];
		aX = (float)threshacc[k];
		aY = acc_ssd[k];
		pch = (float)threshgyro[k];
		rl = gyro_ssd[k];
		fprintf(fpt,"%f %d %f %d %f\n",tm,(int)aX,aY,(int)pch,rl);
	}
	fclose(fpt);
	
}
