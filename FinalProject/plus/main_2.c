
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>	/* needed for multithreading */
#include "resource.h"
#include "globals.h"
#include <tchar.h>


int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
				LPTSTR lpCmdLine, int nCmdShow)

{
MSG			msg;
HWND		hWnd;
WNDCLASS	wc;

wc.style=CS_HREDRAW | CS_VREDRAW;
wc.lpfnWndProc=(WNDPROC)WndProc;
wc.cbClsExtra=0;
wc.cbWndExtra=0;
wc.hInstance=hInstance;
wc.hIcon=LoadIcon(hInstance,"ID_PLUS_ICON");
wc.hCursor=LoadCursor(NULL,IDC_ARROW);
wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
wc.lpszMenuName="ID_MAIN_MENU";
wc.lpszClassName="PLUS";

if (!RegisterClass(&wc))
  return(FALSE);

hWnd=CreateWindow("PLUS","plus program",
		WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
		CW_USEDEFAULT,0,900,650,NULL,NULL,hInstance,NULL);
if (!hWnd)
  return(FALSE);

ShowScrollBar(hWnd,SB_BOTH,FALSE);
ShowWindow(hWnd,nCmdShow);
UpdateWindow(hWnd);
MainWnd=hWnd;

SelectContour = 0;

strcpy(filename,"");
OriginalImage=NULL;
ROWS=COLS=0;

InvalidateRect(hWnd,NULL,TRUE);
UpdateWindow(hWnd);

while (GetMessage(&msg,NULL,0,0))
  {
  TranslateMessage(&msg);
  DispatchMessage(&msg);
  }
return(msg.wParam);
}



LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg,
		WPARAM wParam, LPARAM lParam)

{
HMENU				hMenu;
OPENFILENAME		ofn;
FILE				*fpt;
HDC					hDC;
char				header[320],text[320];
int					MAXVAL,BYTES,xPos,yPos;

switch (uMsg)
  {
  case WM_COMMAND:
    switch (LOWORD(wParam))
      {
	  
	  case ID_DISPLAY_REVERTTOORIGINAL:
		  PaintImage();
		  break;
	  case ID_DISPLAY_DRAWCONTOUR:
		  SetContourFlag = 1;
		  break;
	  case ID_DISPLAY_SAVECONTOUR:
		  SetContourFlag = 0;
		  DownsizeBand();
		  DrawDownsizedContour();
		  break;
	  case ID_DISPLAY_ACTIVATECONTOUREXT:
		  //Downsized contour arrays:
		  PaintImage();
		  DownsizeBand();
		  DrawDownsizedContour();
		  ActiveContouring_Ext();
		  break;
	  case ID_DISPLAY_ACTIVATECONTOURINT:
		  DrawDownsizedContour_Int();
		  ActiveContouring_Int();
		  break;
	  case ID_DISPLAY_SOBELIMAGE:
		  PaintImage();
		  SobelImage();
		  DisplaySobelImage();
		  break;
	  case ID_DISPLAY_VARIANCEIMAGE:
		  PaintImage();
		  VarianceImageFilter();
		  DisplayVarianceImage();
		  break;
	  case ID_FILE_LOAD:
		if (OriginalImage != NULL)
		  {
		  free(OriginalImage);
		  OriginalImage=NULL;
		  }
		memset(&(ofn),0,sizeof(ofn));
		ofn.lStructSize=sizeof(ofn);
		ofn.lpstrFile=filename;
		filename[0]=0;
		ofn.nMaxFile=MAX_FILENAME_CHARS;
		ofn.Flags=OFN_EXPLORER | OFN_HIDEREADONLY;
		ofn.lpstrFilter = "PNM files\0*.pnm\0All files\0*.*\0\0";
		if (!( GetOpenFileName(&ofn))  ||  filename[0] == '\0')
		  break;		/* user cancelled load */
		if ((fpt=fopen(filename,"rb")) == NULL)
		  {
		  MessageBox(NULL,"Unable to open PNM file",filename,MB_OK | MB_APPLMODAL);
		  break;
		  }
		fscanf(fpt, "%s\n%d %d\n%d\n",header,&COLS,&ROWS,&MAXVAL);
		//sprintf(text, "Here goes the number: %s %d %d %d", header, COLS, ROWS, MAXVAL);
		//MessageBox(NULL, text,"", MB_OK | MB_APPLMODAL);

		if (strcmp(header,"P6") != 0)
		  {
		  MessageBox(NULL,"Not a PNM (P6) image",filename, MB_OK | MB_APPLMODAL);
		  fclose(fpt);
		  break;
		  }
		OriginalImage=(unsigned char *)calloc(ROWS*COLS*3,1);
		OriginalImageR = (unsigned char *)calloc(ROWS*COLS, 1);
		OriginalImageG = (unsigned char *)calloc(ROWS*COLS, 1);
		OriginalImageB = (unsigned char *)calloc(ROWS*COLS, 1);
		OriginalImageGrey = (unsigned char *)calloc(ROWS*COLS, 1);
		sobel_image = (float *)calloc(ROWS*COLS, sizeof(float));
		varianceImage = (float *)calloc(ROWS*COLS, sizeof(float));
		if (OriginalImage == NULL)
		{
			MessageBox(NULL, "Memory Allocation Failed!", filename, MB_OK | MB_APPLMODAL);
			fclose(fpt);
			break;
		}
		header[0]=fgetc(fpt);	/* whitespace character after header */
		fread(OriginalImage, sizeof(*OriginalImage), 3 * ROWS*COLS, fpt);
		
		fclose(fpt);
		SetWindowText(hWnd,filename);
		PaintImage();
		break;


      case ID_FILE_QUIT:
        DestroyWindow(hWnd);
        break;
      }
    break;
  case WM_SIZE:		  /* could be used to detect when window size changes */
	PaintImage();
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
	break;

  case WM_MOUSEMOVE:
	  TxPos = LOWORD(lParam);
	  TyPos = HIWORD(lParam);
	  if (SetContourFlag == 1) {
		  if (wParam & MK_LBUTTON)
		  {
			  xPos = LOWORD(lParam);
			  yPos = HIWORD(lParam);
			  if (xPos >= 0 && xPos < COLS  &&  yPos >= 0 && yPos < ROWS)
			  {
				  sprintf(text, "Left Click: %d,%d=>%d       ", xPos, yPos, OriginalImage[yPos*COLS + xPos]);
				  hDC = GetDC(MainWnd);
				  TextOut(hDC, 0, 0, text, strlen(text));		/* draw text on the window */
				  //SetPixel(hDC, xPos, yPos, RGB(255, 0, 0));
				  ReleaseDC(MainWnd, hDC);
			  }

			  SaveBand_Ext();

			  if (ShiftPress == 1) {
				  SaveNewContour();
			  }
		  }
		  else if (wParam & MK_RBUTTON)
		  {
			  xPos = LOWORD(lParam);
			  yPos = HIWORD(lParam);
			  if (xPos >= 0 && xPos < COLS  &&  yPos >= 0 && yPos < ROWS)
			  {
				  sprintf(text, "Right Click: %d,%d=>%d       ", xPos, yPos, OriginalImage[yPos*COLS + xPos]);
				  hDC = GetDC(MainWnd);
				  TextOut(hDC, 0, 0, text, strlen(text));		/* draw text on the window */
																//SetPixel(hDC, xPos, yPos, RGB(255, 0, 0));
				  ReleaseDC(MainWnd, hDC);
			  }

			  xContour_old_int = malloc(sizeof(int) * 360 / d +1);
			  yContour_old_int = malloc(sizeof(int) * 360 / d +1);

			  SaveBand_Int();

			  if (ShiftPress == 1) {
				  //SaveNewContour();
			  }
		  }
	  }
	  else {
		  SetContourFlag = 0;
	  }
	  return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	break;
  case WM_LBUTTONDOWN:
	  q = 0;
	  SelectContour = 1;
	  //ThreadRunning = 1;
	  //_beginthread(AnimationThread, 0, MainWnd);

	  break;
  case WM_RBUTTONDOWN:
	  q = 0;
	  SelectContour = 0;
	  //ThreadRunning = 1;
	  //_beginthread(AnimationThread, 0, MainWnd);

	  break;
  case WM_KEYDOWN:

	  switch (wParam) {
		  case VK_SHIFT:
			  ShiftPress = 1;
			  sprintf(text, "Shift key=1");
			  hDC = GetDC(MainWnd);
			  TextOut(hDC, 0, 0, text, strlen(text));		/* draw text on the window */
			  ReleaseDC(MainWnd, hDC);
			  break;
		  case VK_ESCAPE:
			  ThreadRunning = 0;
			  break;
	  }
	  break;
  case WM_KEYUP:
	  switch (wParam) {
	  case VK_SHIFT:
		  ShiftPress = 0;
		  sprintf(text, "Shift key=0");
		  hDC = GetDC(MainWnd);
		  TextOut(hDC, 0, 0, text, strlen(text));		/* draw text on the window */
		  ReleaseDC(MainWnd, hDC);
	  }
	  break;

  case WM_TIMER:	  /* this event gets triggered every time the timer goes off */
	  hDC = GetDC(MainWnd);
	  SetPixel(hDC, TimerCol, TimerRow, RGB(R, G, B));	/* color the animation pixel blue */
	  ReleaseDC(MainWnd, hDC);
	  TimerRow++;
	  TimerCol += 2;
	  break;
  case WM_HSCROLL:	  /* this event could be used to change what part of the image to draw */
	  PaintImage();	  /* direct PaintImage calls eliminate flicker; the alternative is InvalidateRect(hWnd,NULL,TRUE); UpdateWindow(hWnd); */
	  return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	  break;
  case WM_VSCROLL:	  /* this event could be used to change what part of the image to draw */
	  PaintImage();
	  return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	  break;

  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
    break;
  }

hMenu=GetMenu(MainWnd);

DrawMenuBar(hWnd);

return(0);
}


void ActiveContouring_Ext() {

	HDC					hDC;

	int		i, i1, i2, j, R, G, B, dx, dy, steps,k,m,o,location_r,location_c;
	int		r, c,rc,cc,c_old,r_old;
	float	Xinc, Yinc, sum_dist,min_Ei,max_Ei,min_Ei2,max_Ei2,min_Ee,max_Ee,minimum;

	float	avg_dist[30];
	float	*E_ext;
	float	*E_internal;
	float	*E_internal2;
	float	*E_vi;
	float	*E_internal_norm;
	float	*E_internal2_norm;
	float	*E_ext_norm;
	int *xContour_new, *yContour_new;

	R = 255;
	G = 0;
	B = 0;

	SobelImage();
	VarianceImageFilter();
	size_Index = Count / 5;

	xContour_new = malloc(sizeof(int)*size_Index + 1);
	yContour_new = malloc(sizeof(int)*size_Index + 1);

	E_internal = (float  *)calloc(7 * 7, sizeof(float));
	E_internal2 = (float  *)calloc(7 * 7, sizeof(float));
	E_ext = (float  *)calloc(7 * 7, sizeof(float));
	E_vi = (float *)calloc(7 * 7, sizeof(float));
	E_internal_norm = (float *)calloc(7 * 7, sizeof(float));
	E_internal2_norm = (float *)calloc(7 * 7, sizeof(float));
	E_ext_norm = (float *)calloc(7 * 7, sizeof(float));
	
	for (k = 0;k < 30 ;k++) {

		Sleep(50);
		sum_dist = 0.0;

		for (o = 1;o<size_Index;o++) {
			sum_dist += sqrt(pow((xContour_old[o] - xContour_old[o-1]), 2) + pow((yContour_old[o] - yContour_old[o-1]), 2));
		}

		avg_dist[k] = sum_dist / (size_Index);

		for (i = 0;i < size_Index + 1; i++) {

			c = xContour_old[i];
			r = yContour_old[i];
			c_old = xContour_old[i + 1];
			r_old = yContour_old[i + 1];
			
			for (rc = -3; rc <= 3; rc++) {
				for (cc = -3; cc <= 3; cc++) {
					E_internal[(3 + rc) * 7 + (3 + cc)] = pow(((c + cc) - c_old), 2) + pow(((r + rc) - r_old), 2);
					E_internal2[(3 + rc) * 7 + (3 + cc)] = pow((avg_dist[k] - sqrt(pow(((r + rc) - r_old), 2) + pow(((c + cc) - c_old), 2))), 2);
					E_ext[(3 + rc) * 7 + (3 + cc)] = pow(sobel_image[(r + rc)*COLS + (c + cc)],2);
				}
			}

			//Internal energy normalized
			min_Ei = E_internal[0];
			max_Ei = E_internal[0];

			for (j = 0;j<49;j++) {
				if (E_internal[j] <= min_Ei) {
					min_Ei = E_internal[j];
				}
				if (E_internal[j] >= max_Ei) {
					max_Ei = E_internal[j];
				}
			}
			for (rc = -3; rc <= 3; rc++) {
				for (cc = -3; cc <= 3; cc++) {
					E_internal_norm[(3 + rc) * 7 + (3 + cc)] = (E_internal[(3 + rc) * 7 + (3 + cc)] - min_Ei) / (max_Ei - min_Ei);

				}
			}

			//Internal Energy 2 normalized
			min_Ei2 = E_internal2[0];
			max_Ei2 = E_internal2[0];
			for (j = 0;j<49;j++) {
				if (E_internal2[j] <= min_Ei2) {
					min_Ei2 = E_internal2[j];
				}
				if (E_internal2[j] >= max_Ei2) {
					max_Ei2 = E_internal2[j];
				}
			}
			for (rc = -3; rc <= 3; rc++) {
				for (cc = -3; cc <= 3; cc++) {
					E_internal2_norm[(3 + rc) * 7 + (3 + cc)] = (E_internal2[(3 + rc) * 7 + (3 + cc)] - min_Ei2) / (max_Ei2 - min_Ei2);
	
				}
			}

			//External energy normalized
			min_Ee = E_ext[0];
			max_Ee = E_ext[0];
			for (j = 0;j<49;j++) {
				if (E_ext[j] <= min_Ee) {
					min_Ee = E_ext[j];
				}
				if (E_ext[j] >= max_Ee) {
					max_Ee = E_ext[j];
				}
			}

			for (rc = -3; rc <= 3; rc++) {
				for (cc = -3; cc <= 3; cc++) {
					E_ext_norm[(3 + rc) * 7 + (3 + cc)] = -(E_ext[(3 + rc) * 7 + (3 + cc)] - min_Ee) / (max_Ee - min_Ee);

				}
			}

			//Sum of energies
			for (rc = -3; rc <= 3; rc++) {
				for (cc = -3; cc <= 3; cc++) {
					E_vi[(3 + rc) * 7 + (3 + cc)] = E_internal_norm[(3 + rc) * 7 + (3 + cc)] + E_internal2_norm[(3 + rc) * 7 + (3 + cc)] + E_ext_norm[(3 + rc) * 7 + (3 + cc)];
				}
			}

			minimum = E_vi[0];
			for (rc = -3; rc <= 3; rc++) {
				for (cc = -3; cc <= 3; cc++) {
					if (E_vi[(3 + rc) * 7 + (3 + cc)] <= minimum) {
						minimum = E_vi[(3 + rc) * 7 + (3 + cc)];
						location_r = rc;
						location_c = cc;
					}
				}
			}

			yContour_new[i] = r + location_r;
			xContour_new[i] = c + location_c;
		}

	    //Last pixel is first pixel -> full circle
		yContour_new[size_Index] = yContour_new[0]; //r
		xContour_new[size_Index] = xContour_new[0]; //c
													
		//Allocating new contour
		for (m = 0;m<size_Index+1;m++) {
			yContour_old[m] = yContour_new[m]; //r
			xContour_old[m] = xContour_new[m]; //c
		}

		//PaintImage();

		RedrawContour();
		DrawDownsizedContour();

	}

}

void ActiveContouring_Int() {

	HDC					hDC;

	int		i, i1, i2, j, R, G, B, dx, dy, steps, k, m, o, location_r, location_c;
	int		r, c, rc, cc, c_old, r_old;
	float	Xinc, Yinc, sum_dist, min_Ei, max_Ei, min_Ei2, max_Ei2, min_Ee, max_Ee, minimum;

	float	avg_dist[30];
	float	*E_ext;
	float	*E_internal;
	float	*E_internal2;
	float	*E_vi;
	float	*E_internal_norm;
	float	*E_internal2_norm;
	float	*E_ext_norm;
	int *xContour_new, *yContour_new;

	R = 255;
	G = 0;
	B = 0;

	SobelImage();

	xContour_new = malloc(sizeof(int)*size_Index_int+1);
	yContour_new = malloc(sizeof(int)*size_Index_int+1);

	E_internal = (float  *)calloc(7 * 7, sizeof(float));
	E_internal2 = (float  *)calloc(7 * 7, sizeof(float));
	E_ext = (float  *)calloc(7 * 7, sizeof(float));
	E_vi = (float *)calloc(7 * 7, sizeof(float));
	E_internal_norm = (float *)calloc(7 * 7, sizeof(float));
	E_internal2_norm = (float *)calloc(7 * 7, sizeof(float));
	E_ext_norm = (float *)calloc(7 * 7, sizeof(float));

	for (k = 0; k < 30; k++) {

		Sleep(1);
		sum_dist = 0.0;

		for (o = 1; o<size_Index_int; o++) {
			sum_dist += sqrt(pow((xContour_old_int[o] - xContour_old_int[o - 1]), 2) + pow((yContour_old_int[o] - yContour_old_int[o - 1]), 2));
		}

		avg_dist[k] = sum_dist / size_Index_int;

		for (i = 0;i < size_Index_int +1; i++) {

			c = xContour_old_int[i];
			r = yContour_old_int[i];
			c_old = xContour_old_int[i + 1];
			r_old = yContour_old_int[i + 1];

			for (rc = -3; rc <= 3; rc++) {
				for (cc = -3; cc <= 3; cc++) {
					E_internal[(3 + rc) * 7 + (3 + cc)] = pow(((c + cc) - c_old), 2) + pow(((r + rc) - r_old), 2);
					E_internal2[(3 + rc) * 7 + (3 + cc)] = pow((avg_dist[k] - sqrt(pow(((r + rc) - r_old), 2) + pow(((c + cc) - c_old), 2))), 2);
					E_ext[(3 + rc) * 7 + (3 + cc)] = pow(sobel_image[(r + rc)*COLS + (c + cc)], 2);
				}
			}

			//Internal energy normalized
			min_Ei = E_internal[0];
			max_Ei = E_internal[0];

			for (j = 0;j<49;j++) {
				if (E_internal[j] <= min_Ei) {
					min_Ei = E_internal[j];
				}
				if (E_internal[j] >= max_Ei) {
					max_Ei = E_internal[j];
				}
			}
			for (rc = -3; rc <= 3; rc++) {
				for (cc = -3; cc <= 3; cc++) {
					E_internal_norm[(3 + rc) * 7 + (3 + cc)] = (E_internal[(3 + rc) * 7 + (3 + cc)] - min_Ei) / (max_Ei - min_Ei);

				}
			}

			//Internal Energy 2 normalized
			min_Ei2 = E_internal2[0];
			max_Ei2 = E_internal2[0];
			for (j = 0;j<49;j++) {
				if (E_internal2[j] <= min_Ei2) {
					min_Ei2 = E_internal2[j];
				}
				if (E_internal2[j] >= max_Ei2) {
					max_Ei2 = E_internal2[j];
				}
			}
			for (rc = -3; rc <= 3; rc++) {
				for (cc = -3; cc <= 3; cc++) {
					E_internal2_norm[(3 + rc) * 7 + (3 + cc)] = (E_internal2[(3 + rc) * 7 + (3 + cc)] - min_Ei2) / (max_Ei2 - min_Ei2);

				}
			}

			//External energy normalized
			min_Ee = E_ext[0];
			max_Ee = E_ext[0];
			for (j = 0;j<49;j++) {
				if (E_ext[j] <= min_Ee) {
					min_Ee = E_ext[j];
				}
				if (E_ext[j] >= max_Ee) {
					max_Ee = E_ext[j];
				}
			}

			for (rc = -3; rc <= 3; rc++) {
				for (cc = -3; cc <= 3; cc++) {
					E_ext_norm[(3 + rc) * 7 + (3 + cc)] = -(E_ext[(3 + rc) * 7 + (3 + cc)] - min_Ee) / (max_Ee - min_Ee);

				}
			}

			//Sum of energies
			for (rc = -3; rc <= 3; rc++) {
				for (cc = -3; cc <= 3; cc++) {
					E_vi[(3 + rc) * 7 + (3 + cc)] = E_internal_norm[(3 + rc) * 7 + (3 + cc)] + E_internal2_norm[(3 + rc) * 7 + (3 + cc)] + E_ext_norm[(3 + rc) * 7 + (3 + cc)];
				}
			}

			minimum = E_vi[0];
			for (rc = -3; rc <= 3; rc++) {
				for (cc = -3; cc <= 3; cc++) {
					if (E_vi[(3 + rc) * 7 + (3 + cc)] >= minimum) {
						minimum = E_vi[(3 + rc) * 7 + (3 + cc)];
						location_r = rc;
						location_c = cc;
					}
				}
			}

			yContour_new[i] = r + location_r;
			xContour_new[i] = c + location_c;
		}

		//Last pixel is first pixel -> full circle
		yContour_new[size_Index] = yContour_new[0]; //r
		xContour_new[size_Index] = xContour_new[0]; //c

													//Allocating new contour
		for (m = 0;m<size_Index ;m++) {
			yContour_old[m] = yContour_new[m]; //r
			xContour_old[m] = xContour_new[m]; //c
		}

		DrawDownsizedContour_Int();
	}

}

void SobelImage() {

	int r, c, r1, c1, r2, c2, i;
	float max, min;
	int   sum1, sum2;
	float sum;
		
	// sobel image
	int sobel_matrix_f1[3][3] = { { -1, 0, 1 },
	{ -2, 0, 2 },
	{ -1, 0, 1 } };
	int sobel_matrix_f2[3][3] = { { -1, -2, -1 },
	{ 0,  0,  0 },
	{ 1,  2,  1 } };

	
	for (r = 1; r<ROWS - 1; r++) {
		for (c = 1; c<COLS - 1; c++)
		{
			sum1 = 0;
			for (r1 = -1; r1 <= 1; r1++) {
				for (c1 = -1; c1 <= 1; c1++) {
					sum1 += sobel_matrix_f1[r1 + 1][c1 + 1] * (int)OriginalImageGrey[(r + r1)*COLS + (c + c1)];
				}
			}
			sum2 = 0;
			for (r2 = -1; r2 <= 1; r2++) {
				for (c2 = -1; c2 <= 1; c2++) {
					sum2 += sobel_matrix_f2[r2 + 1][c2 + 1] * (int)OriginalImageGrey[(r + r2)*COLS + (c + c2)];
				}
			}

			sum = sqrt(pow(sum1, 2) + pow(sum2, 2));
			sobel_image[r*COLS + c] = sum;
		}
	}
}

void VarianceImageFilter() {

	int r, c, r1, c1,c2,r2;
	float mean_sum, mean_rc, var_sum, var_rc;
	float *meanImage;
	
	meanImage = (float *)calloc(ROWS*COLS, sizeof(float));


	for (r = 1; r<ROWS - 1; r++) {
		for (c = 1; c<COLS - 1; c++)
		{
			mean_sum = 0;
			for (r1 = -1; r1 <= 1; r1++) {
				for (c1 = -1; c1 <= 1; c1++) {
					mean_sum += (float)OriginalImageGrey[(r + r1)*COLS + (c + c1)];
				}
			}
			
			mean_rc = mean_sum/9;

			var_sum = 0;
			for (r2 = -1; r2 <= 1; r2++) {
				for (c2 = -1; c2 <= 1; c2++) {
					var_sum += (float)(OriginalImageGrey[(r + r1)*COLS + (c + c1)]*OriginalImageGrey[(r + r1)*COLS + (c + c1)]);
				}
			}

			var_rc = var_sum / 9;
			varianceImage[r*COLS + c] = var_rc - mean_rc;
		}
	}
}

void DisplaySobelImage() {

	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO			*bm_info;
	int					i, j, r, c, DISPLAY_ROWS, DISPLAY_COLS;
	float max, min;

	unsigned char		*DisplayImage;

	if (OriginalImage == NULL)
		return;		/* no image to draw */

	max = sobel_image[0];
	min = sobel_image[0];
	for (i = 0; i<ROWS*COLS; i++) {
		if (sobel_image[i] > max) {
			max = sobel_image[i];
		}
		if (sobel_image[i] < min) {
			min = sobel_image[i];
		}
	}

	normalized_sobel_image = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));

	for (i = 0; i<ROWS*COLS; i++) {
		normalized_sobel_image[i] = (unsigned char)((sobel_image[i] - min) * 255 / (max - min));
	}

	/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
	DISPLAY_ROWS = ROWS;
	DISPLAY_COLS = COLS;
	if (DISPLAY_ROWS % 4 != 0)
		DISPLAY_ROWS = (DISPLAY_ROWS / 4 + 1) * 4;
	if (DISPLAY_COLS % 4 != 0)
		DISPLAY_COLS = (DISPLAY_COLS / 4 + 1) * 4;
	DisplayImage = (unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS, 1);
	for (r = 0; r<ROWS; r++)
		for (c = 0; c<COLS; c++)
			DisplayImage[r*DISPLAY_COLS + c] = normalized_sobel_image[r*COLS + c];

	BeginPaint(MainWnd, &Painter);
	hDC = GetDC(MainWnd);
	bm_info_header.biSize = sizeof(BITMAPINFOHEADER);
	bm_info_header.biWidth = DISPLAY_COLS;
	bm_info_header.biHeight = -DISPLAY_ROWS;
	bm_info_header.biPlanes = 1;
	bm_info_header.biBitCount = 8;
	bm_info_header.biCompression = BI_RGB;
	bm_info_header.biSizeImage = 0;
	bm_info_header.biXPelsPerMeter = 0;
	bm_info_header.biYPelsPerMeter = 0;
	bm_info_header.biClrUsed = 256;
	bm_info_header.biClrImportant = 256;
	bm_info = (BITMAPINFO *)calloc(1, sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	bm_info->bmiHeader = bm_info_header;
	for (i = 0; i<256; i++)
	{
		bm_info->bmiColors[i].rgbBlue = bm_info->bmiColors[i].rgbGreen = bm_info->bmiColors[i].rgbRed = i;
		bm_info->bmiColors[i].rgbReserved = 0;
	}

	SetDIBitsToDevice(hDC, 0, 0, DISPLAY_COLS, DISPLAY_ROWS, 0, 0,
		0, /* first scan line */
		DISPLAY_ROWS, /* number of scan lines */
		DisplayImage, bm_info, DIB_RGB_COLORS);
	ReleaseDC(MainWnd, hDC);
	EndPaint(MainWnd, &Painter);

	free(DisplayImage);
	free(bm_info);

}

void DisplayVarianceImage() {

	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO			*bm_info;
	int					i, j, r, c, DISPLAY_ROWS, DISPLAY_COLS;
	float max, min;

	unsigned char		*DisplayImage;
	unsigned char		*normalized_varianceImage;

	if (OriginalImage == NULL)
		return;		/* no image to draw */

	max = varianceImage[0];
	min = varianceImage[0];
	for (i = 0; i<ROWS*COLS; i++) {
		if (varianceImage[i] > max) {
			max = varianceImage[i];
		}
		if (varianceImage[i] < min) {
			min = varianceImage[i];
		}
	}

	normalized_varianceImage = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));

	for (i = 0; i<ROWS*COLS; i++) {
		normalized_varianceImage[i] = (unsigned char)((varianceImage[i] - min) * 255 / (max - min));
	}

	/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
	DISPLAY_ROWS = ROWS;
	DISPLAY_COLS = COLS;
	if (DISPLAY_ROWS % 4 != 0)
		DISPLAY_ROWS = (DISPLAY_ROWS / 4 + 1) * 4;
	if (DISPLAY_COLS % 4 != 0)
		DISPLAY_COLS = (DISPLAY_COLS / 4 + 1) * 4;
	DisplayImage = (unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS, 1);
	for (r = 0; r<ROWS; r++)
		for (c = 0; c<COLS; c++)
			DisplayImage[r*DISPLAY_COLS + c] = normalized_varianceImage[r*COLS + c];

	BeginPaint(MainWnd, &Painter);
	hDC = GetDC(MainWnd);
	bm_info_header.biSize = sizeof(BITMAPINFOHEADER);
	bm_info_header.biWidth = DISPLAY_COLS;
	bm_info_header.biHeight = -DISPLAY_ROWS;
	bm_info_header.biPlanes = 1;
	bm_info_header.biBitCount = 8;
	bm_info_header.biCompression = BI_RGB;
	bm_info_header.biSizeImage = 0;
	bm_info_header.biXPelsPerMeter = 0;
	bm_info_header.biYPelsPerMeter = 0;
	bm_info_header.biClrUsed = 256;
	bm_info_header.biClrImportant = 256;
	bm_info = (BITMAPINFO *)calloc(1, sizeof(BITMAPINFO) + 256 * sizeof(RGBQUAD));
	bm_info->bmiHeader = bm_info_header;
	for (i = 0; i<256; i++)
	{
		bm_info->bmiColors[i].rgbBlue = bm_info->bmiColors[i].rgbGreen = bm_info->bmiColors[i].rgbRed = i;
		bm_info->bmiColors[i].rgbReserved = 0;
	}

	SetDIBitsToDevice(hDC, 0, 0, DISPLAY_COLS, DISPLAY_ROWS, 0, 0,
		0, /* first scan line */
		DISPLAY_ROWS, /* number of scan lines */
		DisplayImage, bm_info, DIB_RGB_COLORS);
	ReleaseDC(MainWnd, hDC);
	EndPaint(MainWnd, &Painter);

	free(DisplayImage);
	free(bm_info);

}

void SaveBand_Ext()
{
	HDC					hDC;
	int x_coord, y_coord,i,R,G,B;
	//int size_Index;

	R = 0;
	G = 255;
	B = 0; 

	//size_Index = sizeof(xIndex) / sizeof(xIndex[0]);

	x_coord = TxPos;
	y_coord = TyPos;
	xIndex[q] = x_coord;
	yIndex[q] = y_coord;
	q++;
	hDC = GetDC(MainWnd);
	SetPixel(hDC, x_coord, y_coord, RGB(R, G, B));
	Sleep(1);
	ReleaseDC(MainWnd, hDC);

}

void SaveBand_Int()
{
	HDC					hDC;
	int x_coord, y_coord, i, i1,i2,theta,radius;
	//int size_Index;

	R = 0;
	G = 255;
	B = 0;

	x_coord = TxPos;
	y_coord = TyPos;

	q_int = 0;
	for (theta = 0; theta < 360; theta += d) {
		radius = 10;
		i1 = radius*cos(theta*3.1415926535 / 180); //x is c
		i2 = radius*sin(theta*3.14 / 180); //y is r
		hDC = GetDC(MainWnd);
		SetPixel(hDC, x_coord + i1, y_coord + i2, RGB(R, G, B));
		Sleep(0.1);
		xContour_old_int[q_int] = x_coord + i1;
		yContour_old_int[q_int] = y_coord + i2;
		ReleaseDC(MainWnd, hDC);
		q_int++;
	}

	size_Index_int = 360/d;

	// full circle
	xContour_old_int[size_Index_int] = xContour_old_int[0];
	yContour_old_int[size_Index_int] = yContour_old_int[0];
}


void SaveNewContour()
{
	HDC					hDC;
	int					x_coord, y_coord, i, k, l, R, G, B;
	int					dx, dy, steps, i1, i2;
	int					c0, r0, c1, r1, r, c;
	float				Xinc, Yinc;

	x_coord = TxPos;
	y_coord = TyPos;

	int *xContour_3, *yContour_3;
	xContour_3 = malloc(sizeof(int)*(sizeof(xContour_old)/sizeof(xContour_old[0])));
	yContour_3 = malloc(sizeof(int)*(sizeof(xContour_old) / sizeof(xContour_old[0])));

	int j = 0;
	int p = 0;
	int newContourx, newContoury;
	for (i = 0; i < w;i++) {
		if (xContour_old[p] == x_coord_old[i] && yContour_old[p] == y_coord_old[i]) {
			xContour_3[j] = xContour_old[p];
			yContour_3[j] = yContour_old[p];
			j++;
			p++;
		}
		//if (x_coord == x_coord_old[i] && y_coord == y_coord_old[i]) {
		//	xContour_3[j] = x_coord;
		//	yContour_3[j] = y_coord;
		//	j++;
		//}
	}

	for (l = 0; l <= j; l++) {
		float x;
		x = xContour_3[l];
		float y;
		y = yContour_3[l];

		float x_coord1 = xContour_3[l + 1];
		float y_coord1 = yContour_3[l + 1];

		dx = x_coord1 - x;
		dy = y_coord1 - y;

		steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
		Xinc = dx / (float)steps;
		Yinc = dy / (float)steps;

		for (k = 0; k <= steps; k++) {
			hDC = GetDC(MainWnd);

			for (i1 = -1; i1 <= 1; i1++) {
				for (i2 = -1; i2 <= 1; i2++) {
					if (sqrt(i1*i1 + i2*i2) < 1.5) {
						SetPixel(hDC, x + i2, y + i1, RGB(255, 0, 0));
						//SetPixel(hDC, x_coord, y_coord, RGB(R, G, B));
						//x_coord_old[w] = x_coord + i2;
						//y_coord_old[w] = y_coord + i1;
						//w = w + 1;
					}
				}
			}

			x += Xinc;
			y += Yinc;

			ReleaseDC(MainWnd, hDC);
		}
	}
}


void DownsizeBand()
{

	int x_coord0, y_coord0, x_coord1, y_coord1, i, j, R, G, B, dx, dy, steps, i1, i2;
	float Xinc, Yinc;
	//int size_Index;

	R = 0;
	G = 0;
	B = 255;

	xIndex[q] = xIndex[0];
	yIndex[q] = yIndex[0];

	//find all pixels in that contour so when you select every fifth pixel, they're evenly spaced

	/*for (i = 0;i <= q - 1;i++) {

		x_coord0 = xIndex[i];
		y_coord0 = yIndex[i];
		x_coord1 = xIndex[i + 1];
		y_coord1 = yIndex[i + 1];
		dx = x_coord1 - x_coord0;
		dy = y_coord1 - y_coord0;

		steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
		Xinc = dx / (float)steps;
		Yinc = dy / (float)steps;

		float x_coord = x_coord0;
		float y_coord = y_coord0;

		for (j = 0;j <= steps;j++) {
			xContour[Count] = (int)x_coord;
			yContour[Count] = (int)y_coord;
			Count++;
			x_coord += Xinc;
			y_coord += Yinc;
		}
	}*/

	Count = q;

	size_Index = Count / 5;

	xContour_old = malloc(sizeof(int)*(size_Index + 1));
	yContour_old = malloc(sizeof(int)*(size_Index + 1));

	xContour_older = malloc(sizeof(int)*(size_Index + 1));
	yContour_older = malloc(sizeof(int)*(size_Index + 1));

	//select only every fifth pixel 
	for (i = 0;i < size_Index ;i++) {

		xContour_old[i] = xIndex[5 * i];
		yContour_old[i] = yIndex[5 * i];
	}

	//Last pixel = first pixel -> full circle
	xContour_old[size_Index] = xContour_old[0];
	yContour_old[size_Index] = yContour_old[0];

	//xContour_old and yContour_old are the downsized contour arrays

}

void RedrawContour() {


	HDC					hDC;
	int i,i1,i2;


	for (i = 0;i <= w; i++) {

		hDC = GetDC(MainWnd);

		int y = y_coord_old[i];
		int x = x_coord_old[i];
		R = OriginalImageGrey[y * COLS + x];
		G = OriginalImageGrey[y * COLS + x];
		B = OriginalImageGrey[y * COLS + x];
		SetPixel(hDC, x, y, RGB(R, G, B));
		ReleaseDC(MainWnd, hDC);
		//Sleep(10);
	}

}

void DrawDownsizedContour(){


	HDC					hDC;
	int					x_coord0, y_coord0, x_coord1, y_coord1, i, j, R, G, B, dx, dy, steps, i1, i2;

	int					c0, r0, c1, r1, r, c;
	float				Xinc, Yinc;



	w = 0;
	//y_coord_old = { 0 };
	//x_coord_old = { 0 };
	//Draw that downsized contour
	for (i = 0;i < size_Index; i++) {

		float x_coord;
		x_coord = xContour_old[i];
		float y_coord;
		y_coord = yContour_old[i];

		x_coord1 = xContour_old[i + 1];
		y_coord1 = yContour_old[i + 1];

		dx = x_coord1 - x_coord;
		dy = y_coord1 - y_coord;

		steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
		Xinc = dx / (float)steps;
		Yinc = dy / (float)steps;

		for (j = 0; j <= steps; j++) {
			hDC = GetDC(MainWnd);
			R = 0;
			G = 0;
			B = 255;

			for (i1 = -1; i1 <= 1; i1++) {
				for (i2 = -1; i2 <= 1; i2++) {
					if (sqrt(i1*i1 + i2*i2) < 1.5) {
						SetPixel(hDC, x_coord + i2, y_coord + i1, RGB(R, G, B));
						//SetPixel(hDC, x_coord, y_coord, RGB(R, G, B));
						x_coord_old[w] = x_coord + i2;
						y_coord_old[w] = y_coord + i1;
						w = w + 1;
					}
				}
			}

			x_coord += Xinc;
			y_coord += Yinc;

			//Sleep(100);
			ReleaseDC(MainWnd, hDC);
		}
	}

}

void DrawDownsizedContour_Int() {
	HDC					hDC;
	int x_coord0, y_coord0, x_coord1, y_coord1, i, j, R, G, B, dx, dy, steps, i1, i2;
	float Xinc, Yinc;

	R = 0;
	G = 0;
	B = 255;

	//Draw that downsized contour
	for (i = 0;i < size_Index_int; i++) {

		x_coord0 = xContour_old_int[i];
		y_coord0 = yContour_old_int[i];
		x_coord1 = xContour_old_int[i + 1];
		y_coord1 = yContour_old_int[i + 1];
		dx = x_coord1 - x_coord0;
		dy = y_coord1 - y_coord0;

		steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
		Xinc = dx / (float)steps;
		Yinc = dy / (float)steps;
		Sleep(1);

		float x_coord = x_coord0;
		float y_coord = y_coord0;

		for (j = 0; j <= steps; j++) {
			hDC = GetDC(MainWnd);
			for (i1 = -2; i1 <= 2; i1++) {
				for (i2 = -2; i2 <= 2; i2++) {
					if (sqrt(i1*i1 + i2*i2) < 2.5) {
						SetPixel(hDC, x_coord + i1, y_coord+i2, RGB(R, G, B));
					}
				}
			}
			x_coord += Xinc;
			y_coord += Yinc;
			Sleep(0.1);
			ReleaseDC(MainWnd, hDC);
		}
	}
}



void PaintImage()

{
PAINTSTRUCT			Painter;
HDC					hDC;
BITMAPINFOHEADER	bm_info_header;
BITMAPINFO			*bm_info;
int					i,j,r,c,DISPLAY_ROWS,DISPLAY_COLS;

unsigned char		*DisplayImage;

if (OriginalImage == NULL)
  return;		/* no image to draw */

OriginalImageR = (unsigned char *)calloc(ROWS*COLS, 1);
OriginalImageG = (unsigned char *)calloc(ROWS*COLS, 1);
OriginalImageB = (unsigned char *)calloc(ROWS*COLS, 1);
OriginalImageGrey = (unsigned char *)calloc(ROWS*COLS, 1);

for (r = 0;r < ROWS;r++) {
	for (c = 0;c < COLS;c++) {

		OriginalImageR[r*COLS + c] = OriginalImage[(r*COLS + c) * 3];
		OriginalImageG[r*COLS + c] = OriginalImage[(r*COLS + c) * 3 + 1];
		OriginalImageB[r*COLS + c] = OriginalImage[(r*COLS + c) * 3 + 2];
	}
}

for (r = 0; r<ROWS; r++)
	for (c = 0; c<COLS; c++)
		OriginalImageGrey[r*COLS + c] = (OriginalImageR[r*COLS + c] + OriginalImageG[r*COLS + c] + OriginalImageB[r*COLS + c])/3;

		/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
DISPLAY_ROWS=ROWS;
DISPLAY_COLS=COLS;
if (DISPLAY_ROWS % 4 != 0)
  DISPLAY_ROWS=(DISPLAY_ROWS/4+1)*4;
if (DISPLAY_COLS % 4 != 0)
  DISPLAY_COLS=(DISPLAY_COLS/4+1)*4;
DisplayImage=(unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS,1);
for (r=0; r<ROWS; r++)
  for (c=0; c<COLS; c++)
	DisplayImage[r*DISPLAY_COLS+c]=OriginalImageGrey[r*COLS+c];

BeginPaint(MainWnd,&Painter);
hDC=GetDC(MainWnd);
bm_info_header.biSize=sizeof(BITMAPINFOHEADER); 
bm_info_header.biWidth=DISPLAY_COLS;
bm_info_header.biHeight=-DISPLAY_ROWS; 
bm_info_header.biPlanes=1;
bm_info_header.biBitCount=8; 
bm_info_header.biCompression=BI_RGB; 
bm_info_header.biSizeImage=0; 
bm_info_header.biXPelsPerMeter=0; 
bm_info_header.biYPelsPerMeter=0;
bm_info_header.biClrUsed=256;
bm_info_header.biClrImportant=256;
bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
bm_info->bmiHeader=bm_info_header;
for (i=0; i<256; i++)
  {
  bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
  bm_info->bmiColors[i].rgbReserved=0;
  } 

SetDIBitsToDevice(hDC,0,0,DISPLAY_COLS,DISPLAY_ROWS,0,0,
			  0, /* first scan line */
			  DISPLAY_ROWS, /* number of scan lines */
			  DisplayImage,bm_info,DIB_RGB_COLORS);
ReleaseDC(MainWnd,hDC);
EndPaint(MainWnd,&Painter);

free(DisplayImage);
free(bm_info);
}






void AnimationThread(HWND AnimationWindowHandle)

{
	HDC		hDC;
	
	int					_ROWS, _COLS,r,c;
	int					*indices, *rIndex, *cIndex;
	int					RegionSize, TotalRegions;
	unsigned char		*Image, *Labels;

	//_ROWS = ROWS;
	//_COLS = COLS;
	//if (_ROWS % 4 != 0)
	//	_ROWS = (_ROWS / 4 + 1) * 4;
	//if (_COLS % 4 != 0)
	//	_COLS = (_COLS / 4 + 1) * 4;
	//Image = (unsigned char *)calloc(_ROWS*_COLS, 1);
	//for (r = 0; r<ROWS; r++)
	//	for (c = 0; c<COLS; c++)
	//		Image[r*_COLS + c] = OriginalImageGrey[r*COLS + c];

	//ThreadRow = ThreadCol = 0;
	//indices = (int *)calloc(_ROWS*_COLS, sizeof(int));
	//Labels = (unsigned char *)calloc(_ROWS*_COLS, sizeof(unsigned char));
	TotalRegions = 1;

	rIndex = TyPos;
	cIndex = TxPos;
	//SelectContour = 1;
	//ActiveContouring(OriginalImageGrey, rIndex, cIndex);
  
	

}

