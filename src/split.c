/*************************************************************
* split.c
*
* This file contains all of the functions which are necessary
* in order to split a polygon into triangles using the
* algorithm given in class.
*
* Author: Patrick Joiner
***************************************************************/
#include "main.h"

extern void quitX();
extern int scene_vertices,scene_polygons;
extern float xmin,ymin,xmax,ymax;
extern polygon polygons[];
extern vertex vertices[];
extern Display *theDisplay;
extern Window mainWindow;
extern GC     mainGC;
extern unsigned long  LightBluePen, YellowPen,RedPen,GreenPen,BluePen;
extern void draw_polygon(int);
extern void draw_split_polygon();
extern struct triangle *tri_head;
extern int num_tris;

int inside();
void split_polygon();
void get_split_vertices();
float get_minmax();
void file_error();

/*******************************************************************
* split_polygon()
*
* This function splits the polygon whose number is sent to it into
* triangles and draws the result to the main window.
*******************************************************************/
void split_polygon(polynum,tri_pntr)
int polynum;
struct triangle *tri_pntr;
{
    int split_area[MAX_VERTICES*3];
    int stack[MAX_VERTICES];
    int i,j,t,l0,l1,end,top,work;
    int r1,r2,p[3];
    struct triangle *tri;

    top = MAX_VERTICES-1;         /* initialize top of stack */
    stack[top--] = -1;            /* push -1 on stack */
    stack[top] = (polygons[polynum].num_verts)-1;  /* push # of polygon */
                                                  /* vertices on stack */

    i = polygons[polynum].num_verts;    
    for (j=0;j < (i-1);j++)              /* initialize split_area array */
        split_area[j] = polygons[polynum].verts[j+1];
    split_area[i-1] =polygons[polynum].verts[0];         

    work = top;                   /* set working top of stack */
    while (stack[work]!=-1) {     /* repeat until all of polygon done */
        end = stack[top];         /* save end of current polygon */
        l1 = stack[work];         /* get end of polygon */
        l0 = stack[work+1]+1;     /* get start of polygon */
        if ((l1-l0) > 2) {        /* continue if not a triangle */
            if (l1 != end) {      /* if this polygon is not at the end */
                                  /*   of the array, move it to end */
                for (j=0;j<=l1-l0;j++)
                    split_area[end+j+1]=split_area[l0+j];

                for (j=0;j<(end-l0+1);j++)
                    split_area[l0+j] = split_area[l1+1+j];

                j = work;         /* adjust stack to put polygon at top */
                do {
                    stack[j] = stack[j-1]-(l1-l0+1);
                    j--;
                }
                while (j!=top);
                
                work = top;             /* reset all of these just in case */
                end=stack[top];
                work = top;
                l1 = stack[top];
                l0 = stack[top+1]+1;
            }
            else {
                top++;
                get_split_vertices(l0,l1,split_area,&i,&t);
                if (i>t) {
                    j=t; t=i;i=j;
                }
                for ( j=0; j<(t-i+1);j++)
                    split_area[j+l1+3] = split_area[j+i];
                for (j=(l1-t);j>=0;j--)
                    split_area[j+t+2] = split_area[j+t];
                for (j=i-l0;j>=0;j--)
                    split_area[j+t+1-(i-l0)] = split_area[l0+j];
                for (j=0;j<(t-i+1);j++)
                    split_area[l0+j] = split_area[j+l1+3];
   
                stack[--top] = l0+t-i;
                stack[--top] = l1+2;
                end = l1+3;
                work = top;
            }
        }
        else work++;
    }
    while (stack[top]!=-1) {

        r1 = stack[top++];
        r2 = stack[top]+1;

        for (j=r2;j<(r1+1); j++) p[j-r2]=split_area[j];
        tri = (struct triangle *)malloc(sizeof(struct triangle));
        tri->p1 = p[0];
        tri->p2 = p[1];
        tri->p3 = p[2];
        tri->color = polygons[polynum].color;
        tri->next = NULL;
        tri_pntr->next = tri;
        tri_pntr = tri;
        num_tris++;
    }     
}
/********************************************************************
* get_split_vertices()
*
* This function returns the two positions in the array split_area
* which sigify the two vertices at which a split should be made.
*********************************************************************/
void get_split_vertices(pstart,pend,split_area,split1,split2)
int pstart,pend;
int *split_area;
int *split1,*split2;
{
int i,a,b,c,pred,succ,save,lv;
float m1,m2,m3,b1,b2,b3,x1,x2,x3,y1,y2,y3;

    lv = pstart;
    for (i=pstart+1;i<=pend;i++)
        if (vertices[split_area[i]][0] <= vertices[split_area[lv]][0])
            lv = i;
    if ((pred=lv-1)<pstart) pred = pend;
    if ((succ=lv+1)>pend) succ = pstart;

    x1 = vertices[split_area[pred]][0];    /* get x & y values for points */
    y1 = vertices[split_area[pred]][1];    /* of the proposed triangle */
    x2 = vertices[split_area[lv]][0];
    y2 = vertices[split_area[lv]][1];
    x3 = vertices[split_area[succ]][0];
    y3 = vertices[split_area[succ]][1];
    
    if (fabs(x2-x1) > 0.0001)
        m1 = (y2-y1)/(x2-x1);   /* find slopes of 3 lines of triangle */
    else m1 = 1E10;                     
    if (fabs(x3-x2) > 0.0001)
        m2= (y3-y2)/(x3-x2);
    else m2 = 1E10;
    if (fabs(x3-x1) > 0.0001)
        m3 = (y3-y1)/(x3-x1);
    else m3 = 1E10;
    
    b1 = y1-(m1*x1);       /* find y-intercepts of 3 lines of triangle */
    b2 = y2-(m2*x2);
    b3 = y3-(m3*x3);
        
    a = lv;          /* set b & c to point to successor or predecessor */
    if (vertices[split_area[pred]][0] <= vertices[split_area[succ]][0]) {
        b = succ;
        c = pred;
    }
    else {
        c = succ;
        b = pred;
    }
    save = b;

    for (i=pstart;i<=pend;i++) {   /* find all inside vertices */           
        if (i!=a && i!=b && i!= c) {
            if (inside(i,a,b,c,m1,m2,m3,b1,b2,b3,split_area)) {
                if (vertices[split_area[i]][0]
                    < vertices[split_area[save]][0])
                    save = i;      /* save any important vertices */
            }
        }
    } 
    if (save == b) {
        *split1 = b;  /* return vertices of the split */
        *split2 = c;
    }
    else {
        *split1 = a;
        *split2 = save;
    }
}
/********************************************************************
* inside()
*
* This function returns TRUE or FALSE if the point sent to it is
* inside the triangle formed by the three points sent to it.  The
* slopes and y-intercepts, which are also sent, are used to make
* this decision.
********************************************************************/
int inside(p,a,b,c,m1,m2,m3,b1,b2,b3,split_area)
int p,a,b,c;
float m1,m2,m3,b1,b2,b3;
int *split_area;
{
float xmin,xmax,ymin,ymax;
float x,y;

    xmin = get_minmax(a,b,c,1,1,split_area); /* find box left side */
    ymin = get_minmax(a,b,c,0,1,split_area); /* find box right side */
    xmax = get_minmax(a,b,c,1,0,split_area); /* find box bottom side */
    ymax = get_minmax(a,b,c,0,0,split_area); /* find box top side */

    x = vertices[split_area[p]][0];
    y = vertices[split_area[p]][1];

    if ((x >= xmin) && (x <= xmax) && (y >= ymin) && (y <= ymax)) {
        if ( (((m3<=0) && ((y-(m1*x)-b1)<=0) && ((y-(m2*x)-b2)>=0)
           && ((y-(m3*x)-b3)<=0)))
           ||  (((m3>=0) &&((y-(m1*x)-b1)<=0) && ((y-(m2*x)-b2)>=0)
           && ((y-(m3*x)-b3)>=0))) 
           ||  (((m3<=0) &&((y-(m1*x)-b1)>=0) && ((y-(m2*x)-b2)<=0)
           && ((y-(m3*x)-b3)<=0)))
           ||  (((m3>=0) && ((y-(m1*x)-b1)>=0) && ((y-(m2*x)-b2)<=0)
           && ((y-(m3*x)-b3)>=0)))) {
             return(TRUE);
        }
        else {
            return(FALSE);
        }
    }
    else {
        return(FALSE);
    }
}

/******************************************************************
* get_minmax()
*
* This function determines the minimum or maximum x or y value
* for the three points passed to it.
*******************************************************************/
float get_minmax(p,p1,p2,is_x,is_min,split_area)
int p,p1,p2,is_x,is_min;
int *split_area;
{
    float xyval;

    if (is_x) xyval = vertices[split_area[p]][0];
    else xyval = vertices[split_area[p]][1];
    
    if (is_x) {
        if ((is_min && vertices[split_area[p1]][0] < xyval) ||
            (!is_min && vertices[split_area[p1]][0] > xyval))
            xyval = vertices[split_area[p1]][0];
    }
    else {
        if ((is_min && vertices[split_area[p1]][1] < xyval) ||
            (!is_min && vertices[split_area[p1]][1] > xyval))
            xyval = vertices[split_area[p1]][1];
    }    
    if (is_x) {
        if ((is_min && vertices[split_area[p2]][0] < xyval) ||
            (!is_min && vertices[split_area[p2]][0] > xyval))
            xyval = vertices[split_area[p2]][0];
    }
    else {
        if ((is_min && vertices[split_area[p2]][1] < xyval) ||
            (!is_min && vertices[split_area[p2]][1] > xyval))
            xyval = vertices[split_area[p2]][0];
    }
    return(xyval);
}
