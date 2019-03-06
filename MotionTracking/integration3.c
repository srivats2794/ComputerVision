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
	char	h1[20],h2[20],h3[20],h4[20],h5[20],h6[20],h7[20];
	float	*accX,*accY,*accZ,*pitch,*roll,*yaw;
	float	vaccX,vaccY,vaccZ,vaccXavg,vaccYavg,vaccZavg,aaccX,aaccY,aaccZ;
	float	*accX_dis,*accY_dis,*accZ_dis,*pitch_dis,*roll_dis,*yaw_dis;
	float	aX_dis,aY_dis,aZ_dis,pch_dis,rl_dis,yw_dis;
	float	*time,*acc_ssd,*gyro_ssd,*threshacc,*threshgyro;
	float	aX,aY,aZ,pch,rl,yw;
	float	st_acc,et_acc,si_acc,ei_acc,st_gyro,et_gyro,si_gyro,ei_gyro;
	
	float	tm,a_ssd,g_ssd,t_acc,t_gyro;
	float	delta_t;
	
	int		i,j,k,sum,sum2;
	int		acc_index,gyro_index,num_ind,max_len,var_win;
	int		*acc_bi,*acc_ei,*gyro_bi,*gyro_ei;
	
	time=(float *)calloc(1250,sizeof(float));
	acc_ssd=(float *)calloc(1250,sizeof(float));
	gyro_ssd=(float *)calloc(1250,sizeof(float));
	threshacc=(float *)calloc(1250,sizeof(float));
	threshgyro=(float *)calloc(1250,sizeof(float));
	
	accX=(float *)calloc(1250,sizeof(float));
	accY=(float *)calloc(1250,sizeof(float));
	accZ=(float *)calloc(1250,sizeof(float));
	pitch=(float *)calloc(1250,sizeof(float));
	roll=(float *)calloc(1250,sizeof(float));
	yaw=(float *)calloc(1250,sizeof(float));
	
	acc_bi=(int *)calloc(12,sizeof(int));
	acc_ei=(int *)calloc(12,sizeof(int));
	gyro_bi=(int *)calloc(12,sizeof(int));
	gyro_ei=(int *)calloc(12,sizeof(int));
	
	accX_dis=(float *)calloc(1250,sizeof(float));
	accY_dis=(float *)calloc(1250,sizeof(float));
	accZ_dis=(float *)calloc(1250,sizeof(float));
	pitch_dis=(float *)calloc(1250,sizeof(float));
	roll_dis=(float *)calloc(1250,sizeof(float));
	yaw_dis=(float *)calloc(1250,sizeof(float));
	
	
	if ((fpt=fopen("s_acc_gyro.txt", "r")) == NULL)
	{
		printf("Unable to open acc_gyro textfile for reading\n");
		exit(0);
	}
	
	fscanf(fpt,"%s %s %s %s %s %s %s\n",h1,h2,h3,h4,h5,h6,h7);
	
	for(j = 0; j<1250; j++){
		fscanf(fpt,"%f %f %f %f %f %f %f\n",&tm,&aX,&aY,&aZ,&pch,&rl,&yw);
		time[j] = tm;
		accX[j] = aX;
		accY[j] = aY;
		accZ[j] = aZ+1.0;
		pitch[j] = pch;
		roll[j] = rl;
		yaw[j] = yw;
	}
	fclose(fpt);
	
	var_win = 11;
	num_ind = (int)floor(1250.0/(float)var_win);
	max_len = num_ind*var_win;
	
	if ((fpt=fopen("seg_acc_v2.txt", "r")) == NULL)
	{
		printf("Unable to open seg_acc textfile for reading\n");
		exit(0);
	}
	
	fscanf(fpt,"%s %s %s %s %s\n",h1,h2,h3,h4,h5);
	
	for(j = 0; j<max_len; j++){
		fscanf(fpt,"%f %f %f %f %f\n",&tm,&t_acc,&a_ssd,&t_gyro,&g_ssd);
		time[j] = tm;
		threshacc[j] = t_acc;
		acc_ssd[j] = a_ssd;
		threshgyro[j] = t_gyro;
		gyro_ssd[j] = g_ssd;
	}
	fclose(fpt);
	
	
	
	delta_t = 0.05;
	
	acc_index = 0;
	gyro_index = 0;
	
	for(i = 0; i<max_len-1; i++){
		
		
		if(threshacc[i]==1.0 && threshacc[i+1]==0.0)
		{
			acc_bi[acc_index] = i+1;
		}
		if(threshacc[i]==0.0 && threshacc[i+1]==1.0)
		{
			acc_ei[acc_index] = i+1;
			acc_index++;
		}
		
		if(threshgyro[i]==1.0 && threshgyro[i+1]==0.0)
		{
			gyro_bi[gyro_index] = i+1;
		}
		if(threshgyro[i]==0.0 && threshgyro[i+1]==1.0)
		{
			gyro_ei[gyro_index] = i;
			gyro_index++;
		}
	}
	
	
	
	for(i = 0; i<12; i++){
		aX_dis=0.0;
		aY_dis=0.0;
		aZ_dis=0.0;
		pch_dis=0.0;
		rl_dis=0.0;
		yw_dis=0.0;
		
		vaccX = 0.0;
		vaccY = 0.0;
		vaccZ = 0.0;
		
		vaccXavg = 0.0;
		vaccYavg = 0.0;
		vaccZavg = 0.0;
		
		aaccX = 0.0;
		aaccY = 0.0;
		aaccZ = 0.0;
		
		for(j = acc_bi[i]; j<=acc_ei[i]; j++){
			vaccXavg = ((9.81*(accX[j] - aaccX)*delta_t) - vaccX)/2;
			vaccX = ((accX[j] - aaccX)*delta_t);
			aX_dis = vaccXavg*delta_t + aX_dis;
			accX_dis[j] = aX_dis;
			aaccX = accX[j];
			
			vaccYavg = ((9.81*(accY[j] - aaccY)*delta_t) - vaccY)/2;
			vaccY = ((accY[j] - aaccY)*delta_t);
			aY_dis = vaccYavg*delta_t + aY_dis;
			accY_dis[j] = aY_dis;
			aaccY = accY[j];
			
			vaccZavg = ((9.81*(accZ[j] - aaccZ)*delta_t) - vaccZ)/2;
			vaccZ = ((accZ[j] - aaccZ)*delta_t);
			aZ_dis = vaccZavg*delta_t + aZ_dis;
			accZ_dis[j] = aZ_dis;
			aaccZ = accZ[j];
			
			/*
			accX_dis[i] = 9.81*((accX[j+1] - accX[j])/2.0)*delta_t + accX_dis[i];
			accY_dis[i] = 9.81*((accY[j+1] - accY[j])/2.0)*delta_t + accY_dis[i];
			accZ_dis[i] = 9.81*((accZ[j+1] - accZ[j])/2.0)*delta_t + accZ_dis[i];
			*/
		}
		
		for(j = gyro_bi[i]; j<=gyro_ei[i]; j++){
			pch_dis = pitch[j]*delta_t + pch_dis;
			pitch_dis[j] = pch_dis;
			rl_dis = roll[j]*delta_t + rl_dis;
			roll_dis[j] = rl_dis;
			yw_dis = yaw[j]*delta_t + yw_dis;
			yaw_dis[j] = yw_dis;
		}
		
	}
	
	fpt=fopen("motion.txt","w");
	fprintf(fpt,"%s %s %s %s %s %s %s\n",h1,"accX_dis","accY_dis","accZ_dis","pitch_dis","roll_dis","yaw_dis");
	for(k = 0; k<max_len; k++){
		tm = time[k];
		aX = accX_dis[k];
		aY = accY_dis[k];
		aZ = accZ_dis[k];
		pch = pitch_dis[k];
		rl = roll_dis[k];
		yw = yaw_dis[k];
		
		fprintf(fpt,"%f %f %f %f %f %f %f\n",tm,aX,aY,aZ,pch,rl,yw);
	}
	fclose(fpt);
	
	
}