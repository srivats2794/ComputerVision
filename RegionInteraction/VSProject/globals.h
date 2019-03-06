
#define SQR(x) ((x)*(x))	/* macro for square */
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
int		ShowPixelCoords;
int		ShowBigDots;
int		PlayMode;
int		StepMode;
int		wait_loop;
HANDLE	step;
int		ret;

		// Image data
unsigned char	*OriginalImage;
int				ROWS,COLS;
int				distance_set = 10;
int				intensity_set = 10;
int				*BufferImage;
int				*label;
int				rows, cols;
unsigned char	paint_over_label;
char			new_label;
DWORD			rgb_set;
int				ctrl;
int				ver = 0;



#define TIMER_SECOND	1			/* ID of timer used for animation */

		// Drawing flags
int		TimerRow,TimerCol;
int		ThreadRow,ThreadCol;
int		ThreadRunning;

		// Function prototypes
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void PaintImage();
void AnimationThread(void *);		/* passes address of window */
void RegionGrow(HWND AnimationWindowHandle);
void RegionPaint(int *arr, int chk);

