
/*****************************************************************
* init.c
*
* This file contains the functions which initialize the X toolkit
* and the main program, as well as a callback function.
*
* Author: Patrick Joiner and Miguel Quintero
*******************************************************************/
#include "main.h"
#include "x3d_icon.h"
    extern vertex *vnormals;
    extern triangle *polygons;
    extern void paramPopupPrompt();
    extern void quitX();
    extern int num_polygons;
    extern void view3d();
    extern void make_rot_matrix();
    extern vertex *vertices;
    extern int view,num_vertices;
    void mainCallback();
    extern void loadReturn();
    extern void loadTheFile();
    extern void DestroyPopupPrompt();
    extern void filePopupPrompt();
    extern void rotatePopupPrompt();
    extern vector  vrp,vup,dov,vpn,dop,cop;
    extern float   umax,umin,vmax,vmin,back,front;
    extern vertex light;
    extern float ints,kdiff,ambient;
    extern int spec,culled;
    void doneReturn();
 
    XtAppContext appCon;               
    Display *theDisplay; /* X's display id */
    GC       mainGC;      /* X's graphics context id */
    Window   mainWindow;
    Window   outerWindow=(Window)NULL;  /* the outermost window */
    unsigned long  LightBluePen,BluePen,WhitePen,BlackPen,YellowPen,
                   RedPen,GreenPen,grey70Pen,pinkPen;
    int      *theScreen,tempx[2],tempy[2],tempz[2];
    int      shading,file_loaded,paramno,trans_done;
    char     *item;
    Widget      topLevel,windowW;
    Widget      mainview,mainmsg,labelmsg,cullbox;
    Widget      vrp1,vrp2,vrp3,vpn1,vpn2,vpn3;
    Widget      dopmsg,copmsg,vupmsg,vrpmsg,vpnmsg;
    Widget      umsg,umsg1,vmsg1,vmsg,fmsg,bmsg,vwmsg;
    Widget      parbox,perbox;
    Widget      f1,b1,u1,u2,v1,v2,vup1,vup2,vup3,dop1,dop2,dop3;
    Widget      cop1,cop2,cop3,donebox,rotbox,quitbox,loadbox;
    Widget      rotmsg,rangmsg,raxmsg,rptmsg,wirebox,flatbox,phongbox,gourbox;
    Widget      rang,rax1,rax2,rax3,rpt1,rpt2,rpt3;
    Widget      lmsg,l1,l2,l3;
    Widget      intensity,ambilabel,amb,kdiff_id,spec_id;
    Pixmap      pgmIconPattern;          /* program icon */
    vector      rotdir,rotpnt;
    vertex      light,light1;
    int         rotinput =0;
    float       rotangle;
    unsigned long  grays[128];
    XtActionsRec actionTableFileMenu[] =
    {
        {"loadReturn",           loadReturn},
        {"doneReturn",           doneReturn},
        NULL
    };
/***************************************************************
* createXWindow()
*
* This function creates a window from the main view widget, and
* then creates a graphics context for the main window and
* initializes the colors.
****************************************************************/
void createXWindow()
{
   XSetWindowAttributes theWindowAttributes;
   unsigned long        theWindowMask;

   Arg       argList[MAX_ARGS];    /* resource list   */            
   XGCValues theGCValues;          /* return value from XCreateGC */
   outerWindow   = XtWindow( topLevel );
   mainWindow  = XtWindow( mainview );

    if ((mainGC = XCreateGC( theDisplay, mainWindow, (unsigned long) 0,
        &theGCValues)) == 0)
    quitX("initX couldn't get a graphic context");

    XSetForeground(theDisplay, mainGC, WhitePen);
    XSetBackground(theDisplay, mainGC, BlackPen);
}

/*****************************************************************
* set_colors()
*
* This functions retrieves a colormap and allocates some colors.
******************************************************************/
int set_colors() {
int theDepth;
Colormap theColormap;
XColor tempcolor,theHardwareColor, theRGBColor;
int i;

    theDisplay    = XtDisplay( topLevel );
    theScreen     = ( int *)XtScreen( topLevel );
    theDepth  = DefaultDepth(theDisplay,*theScreen);
    BlackPen  = BlackPixel( theDisplay, *theScreen);
    WhitePen  = WhitePixel( theDisplay, *theScreen);
    YellowPen = WhitePen;
    LightBluePen = WhitePen;
    RedPen = WhitePen;
    BluePen = WhitePen;
    GreenPen = WhitePen;
    if (theDepth > 1) {
        theColormap = DefaultColormap(theDisplay,*theScreen);

        for (i=0; i < 128; i++) {
            theHardwareColor.red = (i+1) * 0xFFFF/128;
            theHardwareColor.green = (i+1) * 0xFFFF/128;
            theHardwareColor.blue = (i+1) * 0xFFFF/128;

            if (XAllocColor(theDisplay,theColormap,&theHardwareColor)) {
                grays[i] = theHardwareColor.pixel;
            }
            else {
                /* could not allocate the colors */
                return 0;
            }
        }
        if ((XLookupColor(theDisplay,theColormap,"Yellow",
            &theRGBColor,&theHardwareColor)) &&
            (XAllocColor(theDisplay,theColormap,&theHardwareColor))) {
            YellowPen = theHardwareColor.pixel;
        }
        if ((XLookupColor(theDisplay,theColormap,"LightBlue",
            &theRGBColor,&theHardwareColor)) &&
            (XAllocColor(theDisplay,theColormap,&theHardwareColor))) {
            LightBluePen = theHardwareColor.pixel;
        }
        if ((XLookupColor(theDisplay,theColormap,"Blue",
            &theRGBColor,&theHardwareColor)) &&
            (XAllocColor(theDisplay,theColormap,&theHardwareColor))) {
            BluePen = theHardwareColor.pixel;
        }
        if ((XLookupColor(theDisplay,theColormap,"Red",
            &theRGBColor,&theHardwareColor)) &&
            (XAllocColor(theDisplay,theColormap,&theHardwareColor))) {
            RedPen = theHardwareColor.pixel;
        }
        if ((XLookupColor(theDisplay,theColormap,"Green",
            &theRGBColor,&theHardwareColor)) &&
            (XAllocColor(theDisplay,theColormap,&theHardwareColor))) {
            GreenPen = theHardwareColor.pixel;
        }
	if ((XLookupColor(theDisplay,theColormap,"gray70",
            &theRGBColor,&theHardwareColor)) &&
            (XAllocColor(theDisplay,theColormap,&theHardwareColor))) {
            grey70Pen = theHardwareColor.pixel;
        }
	if ((XLookupColor(theDisplay,theColormap,"pink",
            &theRGBColor,&theHardwareColor)) &&
            (XAllocColor(theDisplay,theColormap,&theHardwareColor))) {
            pinkPen = theHardwareColor.pixel;
	 }
    }

}

/******************************************************************
* makeIcon()
*
* This function creates an icon for the program.
*******************************************************************/
void makeIcon()
{
    XWMHints theWMHints;
    
    pgmIconPattern = XCreateBitmapFromData( theDisplay, outerWindow,
        x3d_icon_bits, 64,64);
    theWMHints.icon_pixmap = pgmIconPattern; 
    theWMHints.initial_state = NormalState; 
    theWMHints.flags = IconPixmapHint | StateHint; 
    XSetWMHints( theDisplay, outerWindow, &theWMHints );
}


/*******************************************************************
* createWidgets()
*
* This function creates the widgets used by the program.  Although
* the program makes Xlib and Xtoolkit calls, the program is
* initialized as a toolkit application.  Low level X calls are only
* used when necessary.
*********************************************************************/
void createWidgets() {
    Widget       entry,pro_form,pro_form2,pro_form3,intens,
                 kdifflabel,speclabel,sub_rot_form,sub_draw;
    Arg          argList[MAX_ARGS];    /* resource list   */            
    int          n,i;                  /* argument count  */
    char         displayString[MAX_CHARS_IN_WINDOW];
    XtTranslations toggletable;
    String trans =
        "<EnterWindow>: highlight(Always) \n\
         <LeaveWindow>:unhighlight() \n\
         <Btn1Down>,<Btn1Up>: set()notify() ";

    toggletable =  XtParseTranslationTable(trans);

    XtAppAddActions(appCon, actionTableFileMenu,
                    XtNumber(actionTableFileMenu));

    n=0;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    windowW = XtCreateManagedWidget( "Big Window", formWidgetClass, 
        topLevel,argList,n);

    
    n =0;
    XtSetArg(argList[n] ,XtNhorizDistance,3); n++;
    XtSetArg(argList[n],XtNvertDistance, 3); n++;
    XtSetArg(argList[n],XtNbackground, grey70Pen); n++;
    sub_draw = XtCreateManagedWidget("sub_draw",formWidgetClass,
				                windowW,argList,n);
    n=0;
    XtSetArg( argList[n], XtNheight, 500);          n++;
    XtSetArg( argList[n], XtNwidth, 500);           n++;
    XtSetArg( argList[n], XtNbackground, BlackPen); n++;
    mainview = XtCreateManagedWidget("main_view", formWidgetClass, sub_draw, 
				     argList, n);


    n =0;
    XtSetArg(argList[n] ,XtNfromHoriz,sub_draw); n++;
    XtSetArg(argList[n],XtNbackground, grey70Pen); n++;
    pro_form = XtCreateManagedWidget("pro_form",formWidgetClass,
				                windowW,argList,n);

    n =0;
    XtSetArg(argList[n],XtNfromVert,pro_form); n++;
    XtSetArg(argList[n] ,XtNfromHoriz, sub_draw); n++;
/*    XtSetArg(argList[n] ,XtNhorizDistance,3); n++;
    XtSetArg(argList[n],XtNvertDistance, 3); n++;
*/    XtSetArg(argList[n],XtNbackground, pinkPen); n++;
    sub_rot_form = XtCreateManagedWidget("sub_rot_form",formWidgetClass,
                                                windowW,argList,n);

    n =0;
    XtSetArg(argList[n] ,XtNfromHoriz, sub_draw); n++;
    XtSetArg(argList[n],XtNfromVert,sub_rot_form); n++;
/*    XtSetArg(argList[n],XtNvertDistance, 206); n++;
*/    XtSetArg(argList[n],XtNbackground, grey70Pen); n++;
    pro_form2 = XtCreateManagedWidget("pro_form2",formWidgetClass,
				                windowW,argList,n);

    n =0;
    XtSetArg(argList[n] ,XtNfromHoriz,sub_draw); n++;
    XtSetArg(argList[n],XtNfromVert,pro_form2); n++;
    XtSetArg(argList[n],XtNbackground, grey70Pen); n++;
    XtSetArg(argList[n],XtNwidth, 500); n++;
    pro_form3 = XtCreateManagedWidget("pro_form3",formWidgetClass,
				                windowW,argList,n);


    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg( argList[n], XtNlabel, "VRP");  n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    vrpmsg = XtCreateManagedWidget("vrpmessage", labelWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNfromHoriz, vrpmsg);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "0.5");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    vrp1 = XtCreateManagedWidget("vrp1", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNfromHoriz, vrp1);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "0.5");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    vrp2 = XtCreateManagedWidget("vrp2", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNfromHoriz, vrp2);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "0.5");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    vrp3 = XtCreateManagedWidget("vrp3", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNfromVert, vrp1);    n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg( argList[n], XtNlabel, "VPN");  n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    vpnmsg = XtCreateManagedWidget("vpnmessage", labelWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, vpnmsg);    n++;
    XtSetArg( argList[n], XtNfromVert, vrp1);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "1");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    vpn1 = XtCreateManagedWidget("vpn1", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, vpn1);    n++;
    XtSetArg( argList[n], XtNfromVert, vrp2);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "1");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    vpn2 = XtCreateManagedWidget("vpn2", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, vpn2);    n++;
    XtSetArg( argList[n], XtNfromVert, vrp3);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "1");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    vpn3 = XtCreateManagedWidget("vpn3", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg( argList[n], XtNfromVert, vpn1);    n++;
    XtSetArg( argList[n], XtNlabel, "COP");  n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    copmsg = XtCreateManagedWidget("copmessage", labelWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, copmsg);    n++;
    XtSetArg( argList[n], XtNfromVert, vpn1);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "400");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    cop1 = XtCreateManagedWidget("cop1", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, cop1);    n++;
    XtSetArg( argList[n], XtNfromVert, vpn2);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "400");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    cop2 = XtCreateManagedWidget("cop2", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, cop2);    n++;
    XtSetArg( argList[n], XtNfromVert, vpn3);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "400");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    cop3 = XtCreateManagedWidget("cop3", asciiTextWidgetClass,
                   pro_form, argList, n);
 
    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg( argList[n], XtNfromVert, cop1);    n++;
    XtSetArg( argList[n], XtNlabel, "DOP");  n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    dopmsg = XtCreateManagedWidget("dopmessage", labelWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, dopmsg);    n++;
    XtSetArg( argList[n], XtNfromVert, cop1);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "0");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    dop1 = XtCreateManagedWidget("dop1", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, dop1);    n++;
    XtSetArg( argList[n], XtNfromVert, cop2);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "0");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    dop2 = XtCreateManagedWidget("dop2", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, dop2);    n++;
    XtSetArg( argList[n], XtNfromVert, cop3);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "-1");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    dop3 = XtCreateManagedWidget("dop3", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg( argList[n], XtNfromVert, dop1);    n++;
    XtSetArg( argList[n], XtNlabel, "VUP");  n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    vupmsg = XtCreateManagedWidget("vupmessage", labelWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, vupmsg);    n++;
    XtSetArg( argList[n], XtNfromVert, dop1);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "0");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    vup1 = XtCreateManagedWidget("vup1", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, vup1);    n++;
    XtSetArg( argList[n], XtNfromVert, dop2);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "1");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    vup2 = XtCreateManagedWidget("vup2", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, vup2);    n++;
    XtSetArg( argList[n], XtNfromVert, dop3);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "0");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    vup3 = XtCreateManagedWidget("vup3", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg( argList[n], XtNfromVert, vup1);    n++;
    XtSetArg( argList[n], XtNlabel, "umin");  n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    umsg = XtCreateManagedWidget("umessage", labelWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, umsg);    n++;
    XtSetArg( argList[n], XtNfromVert, vup1);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "-10");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    u1 = XtCreateManagedWidget("u1", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg( argList[n], XtNfromVert, vup1);    n++;
    XtSetArg( argList[n], XtNfromHoriz, u1);    n++;
    XtSetArg( argList[n], XtNlabel, "umax");  n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    umsg1 = XtCreateManagedWidget("umessage1", labelWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, umsg1);    n++;
    XtSetArg( argList[n], XtNfromVert, vup1);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "10");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    u2 = XtCreateManagedWidget("u2", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg( argList[n], XtNfromVert, u1);    n++;
    XtSetArg( argList[n], XtNlabel, "vmin");  n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    vmsg = XtCreateManagedWidget("vmessage", labelWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, vmsg);    n++;
    XtSetArg( argList[n], XtNfromVert, u1);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "-10");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    v1 = XtCreateManagedWidget("v1", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg( argList[n], XtNfromVert, u1);    n++;
    XtSetArg( argList[n], XtNfromHoriz, v1);    n++;
    XtSetArg( argList[n], XtNlabel, "vmax");  n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    vmsg1 = XtCreateManagedWidget("vmessage1", labelWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, vmsg1);    n++;
    XtSetArg( argList[n], XtNfromVert, u1);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "10");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    v2 = XtCreateManagedWidget("v2", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNfromVert, v1);    n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg( argList[n], XtNlabel, "F");  n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    fmsg = XtCreateManagedWidget("fmessage", labelWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNfromHoriz, fmsg);    n++;
    XtSetArg( argList[n], XtNfromVert, v1);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "0");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    f1 = XtCreateManagedWidget("f1", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNfromVert, v1);    n++;
    XtSetArg( argList[n], XtNfromHoriz, f1);    n++;
    XtSetArg( argList[n], XtNlabel, "B");  n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    bmsg = XtCreateManagedWidget("bmessage", labelWidgetClass,
                   pro_form, argList, n);
    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromHoriz, bmsg);    n++;
    XtSetArg( argList[n], XtNfromVert, v1);    n++;
    XtSetArg( argList[n], XtNwidth, 80);           n++;
    XtSetArg( argList[n], XtNstring, "1");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    b1 = XtCreateManagedWidget("b1", asciiTextWidgetClass,
                   pro_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNvertDistance,3);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNlabel, "ROTATION:");  n++;
    XtSetArg(argList[n], XtNbackground, pinkPen); n++;
    rotmsg = XtCreateManagedWidget("rotmessage", labelWidgetClass,
                   sub_rot_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNfromVert, rotmsg);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNlabel, "Axis");  n++;
    XtSetArg(argList[n], XtNbackground, pinkPen); n++;
    raxmsg = XtCreateManagedWidget("raxmessage", labelWidgetClass,
                   sub_rot_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, rotmsg);    n++;
    XtSetArg( argList[n], XtNfromHoriz, raxmsg);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "1");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    rax1 = XtCreateManagedWidget("rax1", asciiTextWidgetClass,
                   sub_rot_form, argList, n);
    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, rotmsg);    n++;
    XtSetArg( argList[n], XtNfromHoriz, rax1);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "0");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    rax2 = XtCreateManagedWidget("rax2", asciiTextWidgetClass,
                   sub_rot_form, argList, n);
    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, rotmsg);    n++;
    XtSetArg( argList[n], XtNfromHoriz, rax2);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "0");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    rax3 = XtCreateManagedWidget("rax3", asciiTextWidgetClass,
                   sub_rot_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNfromVert, raxmsg);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNlabel, "Point");  n++;
    XtSetArg(argList[n], XtNbackground, pinkPen); n++;
    rptmsg = XtCreateManagedWidget("rptmessage", labelWidgetClass,
                   sub_rot_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, raxmsg);    n++;
    XtSetArg( argList[n], XtNfromHoriz, rptmsg);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "0");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    rpt1 = XtCreateManagedWidget("rpt1", asciiTextWidgetClass,
                   sub_rot_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, raxmsg);    n++;
    XtSetArg( argList[n], XtNfromHoriz, rpt1);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "0");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    rpt2 = XtCreateManagedWidget("rpt2", asciiTextWidgetClass,
                   sub_rot_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, raxmsg);    n++;
    XtSetArg( argList[n], XtNfromHoriz, rpt2);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "0");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    rpt3 = XtCreateManagedWidget("rpt3", asciiTextWidgetClass,
                   sub_rot_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNfromVert, rptmsg);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNlabel, "Degrees");  n++;
    XtSetArg(argList[n], XtNbackground, pinkPen); n++;
    rangmsg = XtCreateManagedWidget("rangmessage", labelWidgetClass,
                   sub_rot_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, rptmsg);    n++;
    XtSetArg( argList[n], XtNfromHoriz, rangmsg);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "45");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    rang = XtCreateManagedWidget("rang", asciiTextWidgetClass,
                   sub_rot_form, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft); n++;
    XtSetArg( argList[n], XtNfromVert, sub_rot_form);    n++; 
    XtSetArg( argList[n], XtNvertDistance, 4); n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNlabel, "Light Source");  n++;
    lmsg = XtCreateManagedWidget("lmsg", labelWidgetClass,
                   pro_form2, argList, n);
    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, sub_rot_form);    n++;
    XtSetArg( argList[n], XtNvertDistance, 4); n++;
    XtSetArg( argList[n], XtNfromHoriz, lmsg);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "1000");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    l1 = XtCreateManagedWidget("l1", asciiTextWidgetClass,
                   pro_form2, argList, n);
    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, sub_rot_form);    n++;
    XtSetArg( argList[n], XtNvertDistance, 4); n++;
    XtSetArg( argList[n], XtNfromHoriz, l1);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "1000");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    l2 = XtCreateManagedWidget("l2", asciiTextWidgetClass,
                   pro_form2, argList, n);
    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, sub_rot_form);    n++;
    XtSetArg( argList[n], XtNvertDistance, 4); n++;
    XtSetArg( argList[n], XtNfromHoriz, l2);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "1000");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    l3 = XtCreateManagedWidget("l3", asciiTextWidgetClass,
                   pro_form2, argList, n);

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);  n++;
    XtSetArg( argList[n], XtNbackground, grey70Pen); n++;
    XtSetArg( argList[n], XtNfromVert, lmsg); n++;
    XtSetArg( argList[n], XtNlabel,"Intensity"); n++;
    XtSetArg( argList[n], XtNwidth, 100); n++;
    XtSetArg( argList[n], XtNborderWidth,0); n++;
    intens = XtCreateManagedWidget("intens",labelWidgetClass,
				            pro_form2,argList,
				            n);


    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, lmsg);    n++;
    XtSetArg( argList[n], XtNfromHoriz, intens);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "1");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    intensity = XtCreateManagedWidget("intensity",asciiTextWidgetClass,
				            pro_form2,argList,
				            n);

    n = 0;   
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n],XtNbackground,grey70Pen); n++;
    XtSetArg( argList[n], XtNfromVert, intens); n++;
    XtSetArg( argList[n], XtNlabel,"Ambient"); n++;
    XtSetArg( argList[n], XtNwidth, 100); n++;
    XtSetArg( argList[n], XtNborderWidth,0); n++;

    ambilabel = XtCreateManagedWidget("ambilabel",labelWidgetClass,
				      pro_form2, argList,n); 
    
    
    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, intensity);    n++;
    XtSetArg( argList[n], XtNfromHoriz, ambilabel);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "0");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    amb = XtCreateManagedWidget("amb",asciiTextWidgetClass,
				pro_form2,argList,n);
    

    

    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n],XtNbackground,grey70Pen); n++;
    XtSetArg( argList[n], XtNfromVert, ambilabel); n++;
    XtSetArg( argList[n], XtNlabel,"Diff. Coef."); n++;
    XtSetArg( argList[n], XtNwidth, 100); n++;
    XtSetArg( argList[n], XtNborderWidth,0); n++;

    kdifflabel = XtCreateManagedWidget("coeff",labelWidgetClass,
				      pro_form2, argList,n);



    n =0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert, amb);    n++;
    XtSetArg( argList[n], XtNfromHoriz, kdifflabel);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "0.7");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    kdiff_id = XtCreateManagedWidget("kdiff",asciiTextWidgetClass,
				pro_form2,argList,n);



    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n],XtNbackground,grey70Pen); n++;
    XtSetArg( argList[n], XtNfromVert, kdiff_id); n++;
    XtSetArg( argList[n], XtNlabel,"Specular"); n++;
    XtSetArg( argList[n], XtNwidth, 100); n++;
    XtSetArg( argList[n], XtNborderWidth,0); n++;

    speclabel = XtCreateManagedWidget("speclabel",labelWidgetClass,
				      pro_form2, argList,n);



    n =0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNfromVert,kdiff_id );    n++;
    XtSetArg( argList[n], XtNfromHoriz, speclabel);    n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg( argList[n], XtNstring, "50");  n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    spec_id = XtCreateManagedWidget("spec",asciiTextWidgetClass,
				pro_form2,argList,n);
    
    n = 0;
    XtSetArg( argList[n], XtNjustify, XtJustifyLeft);    n++;
    XtSetArg( argList[n], XtNborderWidth, 0);    n++;
    XtSetArg( argList[n], XtNvertDistance, 3);    n++;
    XtSetArg( argList[n], XtNlabel, "Projection");  n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg(argList[n], XtNbackground, grey70Pen); n++;
    vwmsg = XtCreateManagedWidget("vwmessage", labelWidgetClass,
                   pro_form3, argList, n);


    n = 0;
    XtSetArg( argList[n], XtNfromVert, wirebox);    n++;
    XtSetArg( argList[n], XtNvertDistance, 2); n++;
    XtSetArg( argList[n], XtNfromHoriz, vwmsg);    n++;
    XtSetArg( argList[n], XtNtranslations, toggletable);    n++;
    XtSetArg( argList[n], XtNstate, FALSE);    n++;
    XtSetArg( argList[n], XtNlabel, "PARALLEL");  n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    parbox = XtCreateManagedWidget("parbox", toggleWidgetClass,
                   pro_form3, argList, n);
    XtAddCallback(parbox,XtNcallback,mainCallback,NULL);

    n = 0;
    XtSetArg( argList[n], XtNfromVert, wirebox);    n++;
    XtSetArg( argList[n], XtNvertDistance, 2); n++;
    XtSetArg( argList[n], XtNfromHoriz, parbox);    n++;
    XtSetArg( argList[n], XtNtranslations, toggletable);    n++;
    XtSetArg( argList[n], XtNstate, TRUE);    n++;
    XtSetArg( argList[n], XtNradioGroup, parbox);    n++;
    XtSetArg( argList[n], XtNlabel, "PERSPECTIVE");  n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    perbox = XtCreateManagedWidget("perbox", toggleWidgetClass,
                   pro_form3, argList, n);
    XtAddCallback(perbox,XtNcallback,mainCallback,NULL);

    n = 0;
    XtSetArg( argList[n], XtNfromVert, vwmsg);    n++;
    XtSetArg( argList[n], XtNlabel,"Redraw");  n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    donebox = XtCreateManagedWidget("donebox", commandWidgetClass,
                   pro_form3, argList, n);

    XtAddCallback(donebox,XtNcallback,mainCallback,NULL);

    n = 0;
    XtSetArg( argList[n], XtNfromVert, vwmsg);    n++;
    XtSetArg( argList[n], XtNfromHoriz, donebox);    n++;
    XtSetArg( argList[n], XtNlabel,"Rotate");  n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    rotbox = XtCreateManagedWidget("donebox", commandWidgetClass,
                   pro_form3, argList, n);

    XtAddCallback(rotbox,XtNcallback,mainCallback,NULL);

    n = 0;
    XtSetArg( argList[n], XtNfromVert, vwmsg);    n++;
    XtSetArg( argList[n], XtNfromHoriz, rotbox);    n++;
    XtSetArg( argList[n], XtNlabel,"Load");  n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    loadbox = XtCreateManagedWidget("loadbox", commandWidgetClass,
                   pro_form3, argList, n);

    XtAddCallback(loadbox,XtNcallback,filePopupPrompt,NULL);

    n = 0;
    XtSetArg( argList[n], XtNfromVert, vwmsg);    n++;
    XtSetArg( argList[n], XtNfromHoriz, loadbox);    n++;
    XtSetArg( argList[n], XtNlabel,"Quit");  n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    quitbox = XtCreateManagedWidget("quitbox", commandWidgetClass,
                   pro_form3, argList, n);

    XtAddCallback(quitbox,XtNcallback,mainCallback,NULL);

    n = 0;
    XtSetArg( argList[n], XtNfromVert, vwmsg);    n++;
    XtSetArg( argList[n], XtNfromHoriz, quitbox);    n++;
    XtSetArg( argList[n], XtNlabel,"Culled");  n++;
    XtSetArg( argList[n], XtNwidth, 50);           n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    XtSetArg(argList[n], XtNstate, TRUE); n++;
    cullbox = XtCreateManagedWidget("quitbox", toggleWidgetClass,
                   pro_form3, argList, n);
    XtAddCallback(cullbox,XtNcallback,mainCallback,NULL);

    n = 0;
    XtSetArg( argList[n], XtNfromVert, quitbox);    n++;
    XtSetArg( argList[n], XtNtranslations, toggletable);    n++;
    XtSetArg( argList[n], XtNstate, TRUE);    n++;
    XtSetArg( argList[n], XtNlabel, "Wireframe");  n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    wirebox = XtCreateManagedWidget("wirebox", toggleWidgetClass,
                   pro_form3, argList, n);
    XtAddCallback(wirebox,XtNcallback,mainCallback,NULL);

    n = 0;
    XtSetArg( argList[n], XtNfromVert, quitbox);    n++;
    XtSetArg( argList[n], XtNfromHoriz, wirebox);    n++;
    XtSetArg( argList[n], XtNtranslations, toggletable);    n++;
    XtSetArg( argList[n], XtNradioGroup, wirebox);    n++;
    XtSetArg( argList[n], XtNstate, FALSE);    n++;
    XtSetArg( argList[n], XtNlabel, "Polygonal");  n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    flatbox = XtCreateManagedWidget("flatbox", toggleWidgetClass,
                   pro_form3, argList, n);
    XtAddCallback(flatbox,XtNcallback,mainCallback,NULL);

    n = 0;
    XtSetArg( argList[n], XtNfromVert, quitbox);    n++;
    XtSetArg( argList[n], XtNfromHoriz, flatbox);    n++;
    XtSetArg( argList[n], XtNtranslations, toggletable);    n++;
    XtSetArg( argList[n], XtNradioGroup, wirebox);    n++;
    XtSetArg( argList[n], XtNstate, FALSE);    n++;
    XtSetArg( argList[n], XtNlabel, "Gouraud");  n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    gourbox = XtCreateManagedWidget("phongbox", toggleWidgetClass,
                   pro_form3, argList, n);
    XtAddCallback(gourbox,XtNcallback,mainCallback,NULL);

    n = 0;
    XtSetArg( argList[n], XtNfromVert, quitbox);    n++;
    XtSetArg( argList[n], XtNfromHoriz, gourbox);    n++;
    XtSetArg( argList[n], XtNtranslations, toggletable);    n++;
    XtSetArg( argList[n], XtNradioGroup, wirebox);    n++;
    XtSetArg( argList[n], XtNstate, FALSE);    n++;
    XtSetArg( argList[n], XtNlabel, "Phong");  n++;
    XtSetArg( argList[n], XtNwidth, 100);           n++;
    XtSetArg(argList[n], XtNbackground, LightBluePen); n++;
    phongbox = XtCreateManagedWidget("phongbox", toggleWidgetClass,
                   pro_form3, argList, n);
    XtAddCallback(phongbox,XtNcallback,mainCallback,NULL);
}

/******************************************************************
* initX()
*
* This function sets up the X window user interface.   
*******************************************************************/
void initX( argc, argv)
   int argc;
   char *argv[];
{ 
    int i,j;
    char info1[MAX_CHARS_IN_WINDOW];
    XSetWindowAttributes     xswa;
    Arg argList[2];
    String fallbackResources[10];
  
    fallbackResources[0]  ="*Text*editType:          edit";
    fallbackResources[1]  = "*Text*translations: #override \\n <Key>Return: doneReturn()";
    fallbackResources[2]  = "*.resizable:              False";
    fallbackResources[3]  = "*.font: fixed"; // -*-helvetica-bold-r-normal--12-*";
    fallbackResources[4]  = "*loadPrompt*value.translations: #override \\n <Key>Return: loadReturn()";
    fallbackResources[5]  = "*main_view*pro_form.background: pink";
    fallbackResources[6]  = "*main_view*pro_form2.background:  pink";
    fallbackResources[7] = NULL;

    topLevel = XtAppInitialize(&appCon, "x3d - the 3d viewer", NULL, 0, &argc,
               argv,fallbackResources, NULL, 0);
    if (set_colors() == 0) printf("allocation failed\n");
    createWidgets();
    XtRealizeWidget(topLevel);
    createXWindow(); 
    makeIcon();
    xswa.backing_store = Always;
    XChangeWindowAttributes(theDisplay, mainWindow,
                            CWBackingStore, &xswa);

    file_loaded = FALSE;
    view = PERSPECTIVE;
    shading = WIREFRAME;
    front = 0.0; back = 1.0;
    umin = -10.0; umax = 10.0; vmin = -10.0; vmax = 10.0;

    vup[0] = 0; vup[1] = 1; vup[2] = 0; vup[3] = 1;
    vpn[0] = 1; vpn[1] = 1; vpn[2] = 1; vpn[3] = 1;
    vrp[0] = 0.5; vrp[1] = 0.5; vrp[2] = 0.5; vrp[3] = 1;

    dop[0] = 0; dop[1] =0 ; dop[2] = -1; dop[3] = 1;
    cop[0] =  4;
    cop[1] =  4;
    cop[2] =  4;
    cop[3] =  1;
    light[0] = 1; light[1] = 1; light[2]=1;
    trans_done = FALSE;
    culled = TRUE;
}

/**************************************************************
* mainCallback()
*
* This function is called whenever a command or toggle widget 
* is pressed and calls the appropriate routine.
***************************************************************/
void mainCallback(widget,client_data,call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
int i,j;
Arg argList[2];
String str;
vector tv,tv1,a,b,snormal;
matrix Nrot;

    if (widget== quitbox) quitX(NULL);
    if (widget== perbox) {
        view = PERSPECTIVE;
        view3d(view);
    }
    if (widget == parbox) {
        view = PARALLEL;
        view3d(view);
    }
    if (widget == donebox) {

        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(vrp1, argList, 1);
        vrp[0] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(vrp2, argList, 1);
        vrp[1] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(vrp3, argList, 1);
        vrp[2] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(vpn1, argList, 1);
        vpn[0] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(vpn2, argList, 1);
        vpn[1] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(vpn3, argList, 1);
        vpn[2] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(vup1, argList, 1);
        vup[0] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(vup2, argList, 1);
        vup[1] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(vup3, argList, 1);
        vup[2] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(cop1, argList, 1);
        cop[0] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(cop2, argList, 1);
        cop[1] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(cop3, argList, 1);
        cop[2] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(dop1, argList, 1);
        dop[0] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(dop2, argList, 1);
        dop[1] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(dop3, argList, 1);
        dop[2] = atof(str);

        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(u1, argList, 1);
        umin = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(u2, argList, 1);
        umax = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(v1, argList, 1);
        vmin = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(v2, argList, 1);
        vmax = atof(str);

        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(f1, argList, 1);
        front = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(b1, argList, 1);
        back = atof(str);

        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(l1, argList, 1);
        light[0] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(l2, argList, 1);
        light[1] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(l3, argList, 1);
        light[2] = atof(str);
	XtSetArg(argList[0],XtNstring,&str);
	XtGetValues(spec_id,argList,1);
	spec = atoi(str);
	XtSetArg(argList[0],XtNstring,&str);
	XtGetValues(kdiff_id,argList,1);
	kdiff = atof(str);
	XtSetArg(argList[0],XtNstring,&str);
	XtGetValues(amb,argList,1);
	ambient = atof(str);
	XtSetArg(argList[0],XtNstring,&str);
	XtGetValues(intensity,argList,1);
	ints = atof(str);

        trans_done = FALSE;
        view3d(view);
    }
    if (widget == rotbox) {
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(rax1, argList, 1);
        rotdir[0] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(rax2, argList, 1);
        rotdir[1] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(rax3, argList, 1);
        rotdir[2] = atof(str);

        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(rpt1, argList, 1);
        rotpnt[0] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(rpt2, argList, 1);
        rotpnt[1] = atof(str);
        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(rpt3, argList, 1);
        rotpnt[2] = atof(str);

        XtSetArg(argList[0], XtNstring, &str);
        XtGetValues(rang, argList, 1);
        rotangle = atof(str);

        make_axisrot_matrix(rotangle,Nrot);
        for (i = 0; i < num_vertices; i++) { ;
            tv[0] = vertices[i][0];
            tv[1] = vertices[i][1];
            tv[2] = vertices[i][2];
            tv[3] = 1;
            vec_mult(tv,Nrot,tv1);
            vertices[i][0] = tv1[0];
            vertices[i][1] = tv1[1];
            vertices[i][2] = tv1[2];
        }
        for (i = 0; i < num_vertices; i++) {
            vnormals[i][0] = 0;
            vnormals[i][1] = 0;
            vnormals[i][2] = 0;
        }

        for (i=0;i<num_polygons;i++) {
            a[0] = vertices[polygons[i].verts[1]][0]-
                vertices[polygons[i].verts[0]][0];
            a[1] = vertices[polygons[i].verts[1]][1]-
                vertices[polygons[i].verts[0]][1];
            a[2] = vertices[polygons[i].verts[1]][2]-
                vertices[polygons[i].verts[0]][2];

            b[0] = vertices[polygons[i].verts[2]][0]-
                vertices[polygons[i].verts[0]][0];
            b[1] = vertices[polygons[i].verts[2]][1]-
                vertices[polygons[i].verts[0]][1];
            b[2] = vertices[polygons[i].verts[2]][2]-
                vertices[polygons[i].verts[0]][2];
            cross_product(a,b,snormal);
            normalize(snormal);
            for (j=0;j<3;j++) {
                vnormals[polygons[i].verts[j]][0] += snormal[0];
                vnormals[polygons[i].verts[j]][1] += snormal[1];
                vnormals[polygons[i].verts[j]][2] += snormal[2];
            }
        }
	for(i=0; i < num_vertices; i++) 
		normalize(vnormals[i]);
        trans_done = FALSE;	
        view3d(view);
    }
    if (widget == wirebox)
        shading = WIREFRAME;
    if (widget == flatbox)
        shading = FLAT;
    if (widget == phongbox)
        shading = PHONG;
    if (widget == gourbox)
        shading = GOURAUD;
    if (widget == cullbox)
        culled = 1-culled;
}
void doneReturn(widget,client_data,call_data)
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
}

