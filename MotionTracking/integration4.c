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
	
	accX_dis=(float *)calloc(12,sizeof(float));
	accY_dis=(float *)calloc(12,sizeof(float));
	accZ_dis=(float *)calloc(12,sizeof(float));
	pitch_dis=(float *)calloc(12,sizeof(float));
	roll_dis=(float *)calloc(12,sizeof(float));
	yaw_dis=(float *)calloc(12,sizeof(float));
	
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
		accZ[j] = aZ+1;
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
		accX_dis[i]=0.0;
		accY_dis[i]=0.0;
		accZ_dis[i]=0.0;
		pitch_dis[i]=0.0;
		roll_dis[i]=0.0;
		yaw_dis[i]=0.0;
		
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
			accX_dis[i] = vaccXavg*delta_t + accX_dis[i];
			aaccX = accX[j];
			
			vaccYavg = ((9.81*(accY[j] - aaccY)*delta_t) - vaccY)/2;
			vaccY = ((accY[j] - aaccY)*delta_t);
			accY_dis[i] = vaccYavg*delta_t + accY_dis[i];
			aaccY = accY[j];
			
			vaccZavg = ((9.81*(accZ[j] - aaccZ)*delta_t) - vaccZ)/2;
			vaccZ = ((accZ[j] - aaccZ)*delta_t);
			accZ_dis[i] = vaccZavg*delta_t + accZ_dis[i];
			aaccZ = accZ[j];
			
			/*
			accX_dis[i] = 9.81*((accX[j+1] - accX[j])/2.0)*delta_t + accX_dis[i];
			accY_dis[i] = 9.81*((accY[j+1] - accY[j])/2.0)*delta_t + accY_dis[i];
			accZ_dis[i] = 9.81*((accZ[j+1] - accZ[j])/2.0)*delta_t + accZ_dis[i];
			*/
		}
		
		for(j = gyro_bi[i]; j<=gyro_ei[i]; j++){
			pitch_dis[i] = pitch[j]*delta_t + pitch_dis[i];
			roll_dis[i] = roll[j]*delta_t + roll_dis[i];
			yaw_dis[i] = yaw[j]*delta_t + yaw_dis[i];
		}
		
	}
	
	fpt=fopen("motion_periods.txt","w");
	fprintf(fpt,"%s %s %s %s %s %s %s %s %s %s %s %s %s %s\n","st_ind_acc","e_ind_acc","st_time_acc","e_time_acc","accX_dis","accY_dis","accZ_dis","st_ind_gyro","e_ind_gyro","st_time_gyro","e_time_gyro","pitch_dis","roll_dis","yaw_dis");
	for(k = 0; k<12; k++){
		si_acc = (float)acc_bi[k];
		ei_acc = (float)acc_ei[k];
		st_acc = time[(int)si_acc];
		et_acc = time[(int)ei_acc];
		
		si_gyro = (float)gyro_bi[k];
		ei_gyro = (float)gyro_ei[k];
		st_gyro = time[(int)si_gyro];
		et_gyro = time[(int)ei_gyro];
		
		aX = accX_dis[k];
		aY = accY_dis[k];
		aZ = accZ_dis[k];
		pch = pitch_dis[k];
		rl = roll_dis[k];
		yw = yaw_dis[k];
		
		fprintf(fpt,"%d %d %f %f %f %f %f %d %d %f %f %f %f %f\n",(int)si_acc,(int)ei_acc,st_acc,et_acc,aX,aY,aZ,(int)si_gyro,(int)ei_gyro,st_gyro,et_gyro,pch,rl,yw);
	}
	fclose(fpt);
	
	
}