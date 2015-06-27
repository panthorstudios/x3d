/*****************************************************************
* main.c
*
* This file contains the main function as well as functions which
* load the object data and transform the coordinates of the
* vertices to fit the 500x500 window.
*
*  Author: Patrick Joiner
*
******************************************************************/
#include "main.h"
/*****************************************************************
* these are external variables & functions, function prototypes,
* and global variables.
******************************************************************/

    extern void initX();
    extern Display *theDisplay;
    extern void view3d();
    extern int view;
    extern float norm();
    void quitX();
    int load_scene();
    void file_error();
    void transform_vertices();
    void cross_product();
    void normalize();

    XtAppContext appCon;               
    vertex *vertices;
    vertex *vnormals;
    triangle *polygons;
    int num_tris,num_vertices,num_polygons;
    float xmin,xmax,ymin,ymax;

/******************************************************************
*  main()
*
*  This is the main function of the program.  It sets up the
*  widgets, etc., loads the data file, and loops infinitely as
*  events are processed.
*******************************************************************/  
main (argc, argv)
int argc; char *argv[];
{
    XEvent  theEvent;     /* The next X event */
    char *name;

    name  = "x3d";
    if (argc != 1 ) {                          /* if no filename is */
        printf("Usage: x3d\n");   /* specified, exit   */
        exit(1);
    }
    initX(1,&name);                        /* initialize widgets, etc. */
    while (TRUE) {
        XPeekEvent( theDisplay, &theEvent );
        while (XtAppPending(appCon))             
             XtAppProcessEvent(appCon, XtIMAll);  
   }
}

/*********************************************************************
* load_scene()
*
* This function loads the data from the data file whose name is passed
* to it.  No error checking is done, so the file must be in the
* correct format.
**********************************************************************/
int load_scene(filename)
char *filename;
{
    FILE *fp;
    int  i,j,k,temp,*vcount;
    float c,r,snorm,det,neg,m[3][3];
    struct polygon *poly_pntr,*poly_head,*polynext,*tri;
    vertex a,b,snormal;
    poly_head = NULL;
    polynext = NULL;

    if ((fp=fopen(filename,"r")) == NULL) {
        printf("File %s not found.\n",filename);
        return(-1);
    }
    fscanf(fp,"%d",&num_vertices);
    if ((vertices = (vertex *)calloc(num_vertices,sizeof(vertex))) == NULL)
        printf("vertex allocation failed!\n");
    if ((vnormals = (vertex *)calloc(num_vertices,sizeof(vertex))) == NULL)
        printf("vertex normal allocation failed!\n");
    vcount = (int *)calloc(num_vertices,sizeof(int));
    for (i = 0; i < num_vertices; i++) {
        fscanf(fp,"%f %f %f\n",&vertices[i][0],&vertices[i][1],&vertices[i][2]);
        vnormals[i][0] = 0;
        vnormals[i][1] = 0;
        vnormals[i][2] = 0;
        vcount[i] = 0;
    }
    fscanf(fp,"%d",&num_polygons);
    temp = num_polygons;
    for (i = 0; i < num_polygons; i++) {
        poly_pntr = (struct polygon *)malloc(sizeof(struct polygon));
        fscanf(fp,"%d %d",&poly_pntr->num_verts,&poly_pntr->color);
        poly_pntr->verts = (int *)calloc(poly_pntr->num_verts,sizeof(int));
        for (j = 0; j < poly_pntr->num_verts; j++) {
            fscanf(fp,"%d",&k);
            poly_pntr->verts[j] = k;
        }
        if (poly_pntr->num_verts >3) {
            k = 2;
            while (k < poly_pntr->num_verts) {
                tri = (struct polygon *)malloc(sizeof(struct polygon));
                tri->color = poly_pntr->color;
                tri->verts = (int *)calloc(3,sizeof(int));
                tri->verts[0] =  poly_pntr->verts[0];
                tri->verts[1] =  poly_pntr->verts[k-1];
                tri->verts[2] =  poly_pntr->verts[k];
                poly_pntr->next = NULL;
                if (polynext != NULL)
                    polynext->next = tri;
                else 
                    poly_head = tri;
                polynext = tri;
                k++;
            }
            temp += poly_pntr->num_verts-3;
        }
        else {
            if (polynext != NULL)
                polynext->next = poly_pntr;
            else
                poly_head = poly_pntr;
             polynext = poly_pntr;
        }
    }
    fclose(fp);
    num_polygons = temp;
    polygons = (triangle *)calloc(num_polygons,sizeof(triangle)); 

    i = 0;
    for (i=0;i<num_polygons;i++) {
        polygons[i].color = poly_head->color;
        polygons[i].verts[0] = poly_head->verts[0];
        polygons[i].verts[1] = poly_head->verts[1];
        polygons[i].verts[2] = poly_head->verts[2];
        
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
/*        vcount[polygons[i].verts[0]] += 1;
        vcount[polygons[i].verts[1]] += 1;
        vcount[polygons[i].verts[2]] += 1;
*/
        for (j=0;j<3;j++) {
            vnormals[polygons[i].verts[j]][0] += snormal[0];
            vnormals[polygons[i].verts[j]][1] += snormal[1];
            vnormals[polygons[i].verts[j]][2] += snormal[2];
        }
        polynext = poly_head;
        poly_head = poly_head->next;
        free(polynext);
    }
    for (i=0;i<num_vertices;i++) {
/*        vnormals[i][0] /= (float)vcount[i];
        vnormals[i][1] /= (float)vcount[i];
        vnormals[i][2] /= (float)vcount[i];
*/
        normalize(vnormals[i]);
    }
    printf("total memory allocation %d bytes\n",num_vertices*2*sizeof(vertex)+
                (num_polygons*sizeof(triangle)));
    return(1);    
}

/*****************************************************************
* file_error()
*
* This function prints an error and exits the program.  It is
* called when a file read error occurs.
******************************************************************/
void file_error() {
    printf("Read error occurred.\n");
    exit(1);
}

/******************************************************************
* quitX()
*
* This function closes down the connection to the X server.  If
* there was a fatal error, an error message is printed.
*******************************************************************/
void quitX( errorStr )
   char *errorStr;
{
  FILE *fp;
  int i,j;

    if ( errorStr != NULL)
        fprintf(stderr, "FATAL ERROR: %s\n", errorStr);
    XCloseDisplay( theDisplay );
/*    fp = fopen("tie.dat","w");
      fprintf(fp,"[ COLOR 0 1.0 1.0 1.0 ]\n");
    for (i=0;i<num_vertices;i++) {
      fprintf(fp,"[ VERTEX VV%d [ NORMAL %f %f %f ] %f %f %f ]\n",i,vnormals[i][0],
          vnormals[i][1],vnormals[i][2],vertices[i][0],vertices[i][1],
          vertices[i][2]);
  }
  for (i=0;i<num_polygons;i++) {
      fprintf(fp,"[ POLYGON P%d [ COLOR 0 ] VV%d VV%d VV%d ]\n",i,
        polygons[i].verts[0],polygons[i].verts[1],polygons[i].verts[2]);
  }
  fprintf(fp,"[ OBJECT tie_inst\n");
  j=0;
  for(i=0;i<num_polygons;i++) {
      fprintf(fp,"P%d ",i);
      if (++j>14) {
          j=0;
          fprintf(fp,"\n");
     }
  }
  fprintf(fp," ]\n[ INSTANCE tie_inst1 tie_inst\n    1 0 0 0\n   0 1 0 0\n");
  fprintf(fp,"   0 0 1 0\n   0 0 0 1 ]\n");
  fclose(fp);  
*/
  exit(0);
}
void normalize(a)
vector a;
{
    float snorm;

    snorm = norm(a);
    a[0] /=snorm;
    a[1] /=snorm;
    a[2] /=snorm;
} 
void cross_product(a,b,c)
vertex a,b,c;
{
    float amag,bmag,cmag;

    c[0] = ((a[1]*b[2])-(a[2]*b[1]));
    c[1] = -((a[0]*b[2])-(b[0]*a[2]));
    c[2] = ((a[0]*b[1])-(b[0]*a[1]));
}
