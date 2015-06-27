#include "main.h"
extern Display *theDisplay;
extern int file_loaded,view,trans_done;
extern Window mainWindow;
extern Widget mainview,donebox;
extern vertex *vertices,*vnormals;
extern triangle *polygons;
extern int load_scene();
/* extern parammenuitems[11],rotatemenuitems[3],viewmenuitems[2]; */

    void loadReturn();
    void loadTheFile();
    void DestroyPopupPrompt();
    void filePopupPrompt();

void loadReturn(widget, event, params, num_params)
    Widget widget;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    Widget dialog = XtParent(widget);
    loadTheFile(widget, (XtPointer) dialog, (XtPointer) NULL);
}

void loadTheFile(w, client_data, call_data)
    Widget      w;                      /* "SAVE AS" widget */
    XtPointer   client_data, call_data;
{

    Widget dialog,     /* dialog box                    */
           popup;      /* Widget that contains the dialog box */
    String fileName;   /* input from user */
    char msg[MAX_CHARS_IN_WINDOW];    /* error message   */
    int i,j;
    Arg  argList[MAX_ARGS];

    /* Get file name */
    dialog = (Widget) client_data;
    fileName = XawDialogGetValueString(dialog);
    popup = XtParent( (Widget) client_data);
    XtDestroyWidget(popup);
    XFlush(theDisplay);
    if (file_loaded == TRUE) {
        free(polygons);
        free(vertices);
        free(vnormals);
    }
    if (load_scene(fileName) >=0) {
        file_loaded = TRUE;
        trans_done = FALSE;
        XClearWindow(theDisplay,mainWindow);
        view3d(view);
    }
}

void DestroyPopupPrompt(widget, client_data, call_data)
    Widget      widget;
    XtPointer client_data, call_data;
{

    Widget popup = XtParent( (Widget) client_data);
    XtDestroyWidget(popup);
    XFlush(theDisplay);
}

void filePopupPrompt(button, client_data, call_data)
    Widget      button;
    XtPointer client_data, call_data;
{
    Arg         args[5];
    Widget      popup, dialog;
    Position    x, y;
    Dimension   width, height;
    Cardinal    n;

    XtTranslateCoords(donebox, (Position) 10, (Position) 10, &x, &y);

    n = 0;
    XtSetArg(args[n], XtNx, x);                         n++;
    XtSetArg(args[n], XtNy, y);                         n++;

    popup = XtCreatePopupShell("loadPrompt", transientShellWidgetClass,
                                mainview, args, n);
    n=0;
    XtSetArg( args[n],XtNlabel, "Filename:");
    n++;
    XtSetArg( args[n],XtNvalue, "");
    n++;
    dialog = XtCreateManagedWidget("dialog", dialogWidgetClass, popup,
                                   args, n);

    XawDialogAddButton(dialog, "Load", loadTheFile, (XtPointer) dialog);
    XawDialogAddButton(dialog, "Cancel", DestroyPopupPrompt,(XtPointer)dialog);
    XtPopup(popup, XtGrabExclusive);
}
