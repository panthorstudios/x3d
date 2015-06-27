/***************************************************************
* main.h
*
* This file is included by all other files, and contains
* the libraries used and structure definitions.
****************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
/* #include <floatingpoint.h> 
#include <sys/ieeefp.h> */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include <X11/Command.h>
#include <X11/Paned.h>
#include <X11/Shell.h>
#include <X11/cursorfont.h>

#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/Sme.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/AsciiText.h>

#define  FALSE                 0
#define  TRUE                  1
#define  ON                    2
#define  OFF                   3
#define  MAX_POLYGONS         30
#define  MAX_VERTICES         60
#define  X                     1
#define  Y                     0
#define  MIN                   1
#define  MAX                   0
#define  WINDOW_H            501
#define  WINDOW_V            501
#define  MAX_ARGS             15
#define  MAX_CHARS_IN_WINDOW  80
#define  XROT         1
#define  YROT         2
#define  ZROT         3
#define  PERSPECTIVE  0
#define  PARALLEL     1
#define  WIREFRAME    0
#define  FLAT         1
#define  PHONG        2
#define  GOURAUD      3

#define  VRP          0
#define  VPN          1 
#define  VUP          2
#define  COP          3 
#define  DOP          4 
#define  UMIN         5 
#define  UMAX         6 
#define  VMIN         7 
#define  VMAX         8 
#define  F            9 
#define  B           10 

#define  sqr(M)    ((M)*(M))
typedef float vector[4];
typedef float matrix[4][4];
typedef float vertex[3];

struct polygon {
    int num_verts;
    unsigned long color;
    int *verts;
    struct polygon *next;
};

typedef struct {
    unsigned long color;
    int verts[3];
} triangle;

