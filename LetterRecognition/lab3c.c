#include <stdio.h>
#include <stdlib.h>

void main()

{
	FILE *fpt, *fpt2, *fpt3;
	unsigned char	*image;
	unsigned char	*msf, *blank, *temp, *bin, *bincopy;
	char filename[25];
	char header[320];
	char theader[320];
	char letter[1];
	int	 ROWS, COLS, BYTES;
	int	 tROWS, tCOLS, tBYTES;
	int	 r, c, r2, c2;
	int  trans, neigh, a, b, e, d, pass, pix, pix2 = 0, thresh;
	int  end, branch, xcord, ycord, count, check, let, sum1, sum2;
	long TP, FP, FN, TN;
	float TPR = 0, FPR = 0;

	/* read original image*/
	if ((fpt2 = fopen("parenthood.ppm", "rb")) == NULL)
	{
		printf("Unable to open parenthood.ppm for reading\n");
		exit(0);
	}
	fscanf(fpt2, "%s %d %d %d\n", header, &COLS, &ROWS, &BYTES);
	if (strcmp(header, "P5") != 0 || BYTES != 255)
	{
		printf("Not a greyscale 8-bit PPM image\n");
		exit(0);
	}
	image = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
	header[0] = fgetc(fpt2);	/* read white-space character*/
	fread(image, 1, COLS*ROWS, fpt2);
	fclose(fpt2);

	/*Read template image*/
	if ((fpt3 = fopen("parenthood_e_template.ppm", "rb")) == NULL)
	{
		printf("Unable to open parenthood_e_template.ppm for reading\n");
		exit(0);
	}
	fscanf(fpt3, "%s %d %d %d\n", theader, &tCOLS, &tROWS, &tBYTES);
	if (strcmp(theader, "P5") != 0 || tBYTES != 255)
	{
		printf("Not a greyscale 8-bit PPM image\n");
		exit(0);
	}
	temp = (unsigned char *)calloc(tROWS*tCOLS, sizeof(unsigned char));
	theader[0] = fgetc(fpt3);	/* read white-space character*/
	fread(temp, 1, tCOLS*tROWS, fpt3);
	fclose(fpt3);

	fpt3 = fopen("op_thresh.txt", "wb");

	/*Threshold Loop*/
	for (thresh = 200; thresh <= 255; thresh++)
	{
		TP = 0;
		FP = 0;
		FN = 0;
		TN = 0;
		count = 0;
		check = 0;
		let = 0;
		sum1 = 0;
		sum2 = 0;

		/* read msf image*/
		if ((fpt = fopen("nmsf.ppm", "rb")) == NULL)
		{
			printf("Unable to open thre_%d.ppm for reading\n", thresh);
			exit(0);
		}
		fscanf(fpt, "%s %d %d %d\n", header, &COLS, &ROWS, &BYTES);

		msf = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));

		fread(msf, 1, COLS*ROWS, fpt);
		fclose(fpt);

		/*Opening blank image*/
		blank = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));

		/*Setting the blank image to white*/
		for (r = 0; r < ROWS; r++)
			for (c = 0; c < COLS; c++)
			{
			blank[r*COLS + c] = 255;
			}

		/*Copying original image at dots*/
		for (r = (tROWS / 2); r < ROWS - (tROWS / 2) - 1; r++)
			for (c = (tCOLS / 2); c < COLS - (tCOLS / 2) - 1; c++)
			{
			if (msf[r*COLS + c] == 255)
			{
				count += 1;
				for (r2 = -(tROWS / 2); r2 < (tROWS / 2) + 1; r2++)
					for (c2 = -(tCOLS / 2); c2 < (tCOLS / 2) + 1; c2++)
					{
					blank[(r + r2)*COLS + c + c2] = image[(r + r2)*COLS + c + c2];
					}
			}
			}
		fpt = fopen("copied3.ppm", "wb");
		fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);

		for (r = 0; r < ROWS*COLS; r++)
		{
			fprintf(fpt, "%c", blank[r]);
		}
		fclose(fpt);

		/*Allocating memory for binary image*/
		bin = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));
		bincopy = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));

		/*Thresholding copied image*/
		for (r = 0; r < ROWS; r++)
			for (c = 0; c < COLS; c++)
			{
			if (blank[r*COLS + c] > 128)
			{
				bin[r*COLS + c] = 255;
				bincopy[r*COLS + c] = 255;
			}
			else
			{
				bin[r*COLS + c] = 0;
				bincopy[r*COLS + c] = 0;
			}
			}

		/*Writing the binary image*/
		fpt = fopen("bin.ppm", "wb");
		fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);

		for (r = 0; r < ROWS*COLS; r++)
		{
			fprintf(fpt, "%c", bin[r]);
		}
		fclose(fpt);

		/*Skeletonization*/
		while (1)
		{
			pix = 0;
			for (r = 1; r < ROWS - 1; r++)
				for (c = 1; c < COLS - 1; c++)
				{
				trans = 0;
				neigh = 0;
				a = 0;
				b = 0;
				e = 0;
				d = 0;
				pass = 0;
				/*Edge -> Non-edge transitions*/

				if (bin[r*COLS + (c + 1)] - bin[(r + 1)*COLS + (c + 1)] < 0)
					trans += 1;

				if (bin[(r + 1)*COLS + (c + 1)] - bin[(r + 1)*COLS + c] < 0)
					trans += 1;

				if (bin[(r + 1)*COLS + c] - bin[(r + 1)*COLS + (c - 1)] < 0)
					trans += 1;

				if (bin[(r + 1)*COLS + (c - 1)] - bin[r*COLS + (c - 1)] < 0)
					trans += 1;

				if (bin[r*COLS + (c - 1)] - bin[(r - 1)*COLS + (c - 1)] < 0)
					trans += 1;

				if (bin[(r - 1)*COLS + (c - 1)] - bin[(r - 1)*COLS + c] < 0)
					trans += 1;

				if (bin[(r - 1)*COLS + c] - bin[(r - 1)*COLS + (c + 1)] < 0)
					trans += 1;

				if (bin[(r - 1)*COLS + (c + 1)] - bin[r*COLS + (c + 1)] < 0)
					trans += 1;

				/*Edge Neighbor Pixels and N,E or (W & S0*/

				if (bin[r*COLS + c] == bin[r*COLS + (c + 1)])
				{
					neigh += 1;
					b = 1;
				}

				if (bin[r*COLS + c] == bin[(r + 1)*COLS + (c + 1)])
					neigh += 1;

				if (bin[r*COLS + c] == bin[(r + 1)*COLS + c])
				{
					neigh += 1;
					e = 1;
				}

				if (bin[r*COLS + c] == bin[(r + 1)*COLS + (c - 1)])
					neigh += 1;

				if (bin[r*COLS + c] == bin[r*COLS + (c - 1)])
				{
					neigh += 1;
					d = 1;
				}

				if (bin[r*COLS + c] == bin[(r - 1)*COLS + (c - 1)])
					neigh += 1;

				if (bin[r*COLS + c] == bin[(r - 1)*COLS + c])
				{
					neigh += 1;
					a = 1;
				}


				if (bin[r*COLS + c] == bin[(r - 1)*COLS + (c + 1)])
					neigh += 1;

				/*Checking N, E, S & W*/
				if (a != 1 || b != 1 || (e != 1 && d != 1))
				{
					pass = 1;
				}

				/*Marking pixel*/
				if (trans == 1 && neigh >= 3 && neigh <= 7 && pass == 1)
				{
					bincopy[r*COLS + c] = 100;
					pix += 1;
				}
				}

			/*Erasing the marked pixels*/
			for (r = 0; r < ROWS; r++)
				for (c = 0; c < COLS; c++)
				{
				if (bincopy[r*COLS + c] == 100)
				{
					bin[r*COLS + c] = 255;
				}
				}

			if ((pix - pix2) == 0)
				break;
			pix2 = pix;
		}

		/*Opening the ground truth file*/
		if ((fpt2 = fopen("parenthood_gt.txt", "r")) == NULL)
		{
			printf("Unable to open parenthood.txt for reading\n");
			exit(0);
		}

		/*Calculating branch points and end points*/
		while (!feof(fpt2))
		{

			/*Reading ground truth*/
			fscanf(fpt2, "%c %d %d\n", letter, &xcord, &ycord);
			let += 1;
			end = 0;
			branch = 0;

			/*Checking the area around the coordinates*/
			//if (letter[0] == 'e')
			{
				for (r = xcord - 9; r < xcord + 9; r++)
					for (c = ycord - 12; c < ycord + 12; c++)
					{
					trans = 0;
					if (r*COLS + c <= COLS*ROWS && bin[r*COLS + c] == 0)
					{
						if (bin[r*COLS + (c + 1)] - bin[(r + 1)*COLS + (c + 1)] < 0)
							trans += 1;

						if (bin[(r + 1)*COLS + (c + 1)] - bin[(r + 1)*COLS + c] < 0)
							trans += 1;

						if (bin[(r + 1)*COLS + c] - bin[(r + 1)*COLS + (c - 1)] < 0)
							trans += 1;

						if (bin[(r + 1)*COLS + (c - 1)] - bin[r*COLS + (c - 1)] < 0)
							trans += 1;

						if (bin[r*COLS + (c - 1)] - bin[(r - 1)*COLS + (c - 1)] < 0)
							trans += 1;

						if (bin[(r - 1)*COLS + (c - 1)] - bin[(r - 1)*COLS + c] < 0)
							trans += 1;

						if (bin[(r - 1)*COLS + c] - bin[(r - 1)*COLS + (c + 1)] < 0)
							trans += 1;

						if (bin[(r - 1)*COLS + (c + 1)] - bin[r*COLS + (c + 1)] < 0)
							trans += 1;

						if (trans == 1)
							end += 1;

						else if (trans > 2)
							branch += 1;
					}
					}
			}

			if (end == 1 && branch == 1)
			{
				TP += 1;
			}

			if (letter[0] == 'e')
				check += 1;
		}

		FP = count - TP;
		FN = check - TP;
		TN = let - TP - FP - FN;

		sum1 = TP + FN;
		sum2 = FP + TN;

		/*Calculating TPR and FPR*/
		TPR = (float)TP / sum1;
		FPR = (float)FP / sum2;

		/*Writing to text file*/
		fprintf(fpt3, "%d %f %f\n", thresh, TPR, FPR);
		rewind(fpt2);

		/*Writing the image*/
		fpt = fopen("skel.ppm", "wb");
		fprintf(fpt, "P5 %d %d 255\n", COLS, ROWS);

		for (r = 0; r < ROWS*COLS; r++)
		{
			fprintf(fpt, "%c", bin[r]);
		}
		fclose(fpt);
		fclose(fpt2);

	}
	fclose(fpt3);
}
