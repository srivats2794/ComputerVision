
#define SQR(x) ((x)*(x))	/* macro for square */
#define MAX_QUEUE 10000
#ifndef M_PI			/* in case M_PI not found in math.h */
#define M_PI 3.1415927
#endif
#ifndef M_E
#define M_E 2.718282
#endif

#define MAX_FILENAME_CHARS	320

char	filename[MAX_FILENAME_CHARS];

HWND	MainWnd;

		// Display flags
int		SelectContour;
int		SetContourFlag;
int		ChannelBlue;
int		ChannelRed;
int		ChannelGreen;
int		predicate1, predicate2;



		// Image data
unsigned char	*OriginalImage, *OriginalImageR, *OriginalImageG, *OriginalImageB, *OriginalImageGrey;
unsigned char	*normalized_sobel_image, *normalized_varianceImage;
float			*sobel_image,	*variance_image;
int				ROWS, COLS, _ROWS, _COLS, R, G, B, n = 0, Count = 0, size_Index, size_Index_int, d = 5, w = 0, w1 = 0; w2 = 0, contour_ptx, contour_pty,kc=0;
unsigned char	*Image;
unsigned char	*Labels;
int				*indices, xIndex[5000], yIndex[5000], xContour[10000], yContour[10000], *xContour_old, *yContour_old, *xContour_old_neu, *yContour_old_neu, *xContour_older, *yContour_older;
int				xIndex_int[5000], yIndex_int[5000], xContour_int[10000], yContour_int[10000], *xContour_old_int, *yContour_old_int;
float			Xinc_old, Yinc_old,y_coord_old[10000],x_coord_old[10000],location_flag[10000], one_x[500], one_y[500], y_coord_old_int[10000], x_coord_old_int[10000], centroid_x[500], centroid_y[500];

#define TIMER_SECOND	1			/* ID of timer used for animation */

		// Drawing flags
int		TimerRow,TimerCol,Timer,new_label, ShiftPress=0;
int		ThreadRow, ThreadCol, TxPos, TyPos, q = 0, q_int = 0,step_old,p=0;
int		ThreadRunning;

		// Function prototypes
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void PaintImage();
void SaveBand_Ext();
void SaveBand_Int();
void DownsizeBand();
void DisplaySobelImage();
void DisplayVarianceImage();
void DrawDownsizedContour();
void RedrawContour();
void ActiveContouring_Ext();
void ActiveContouring_Int();
float *SobelImage(unsigned char *image);
unsigned char *VarianceImageFilter(unsigned char *image);
void DragBlob();
void RedrawBlobPix();
void DrawBlob();
void NeutralContouring();
void DrawInternalContour();
void RedrawContour_int();

void AnimationThread(HWND AnimationWindowHandle);		/* passes address of window */

