#include <float.h>
#include "main.h" 
#define pi  3.141592654
extern vertex *vertices;
extern triangle *polygons;
extern Display *theDisplay;
extern vertex *vnormals;
extern GC mainGC;
extern Window mainWindow;
extern unsigned long RedPen,GreenPen,BluePen,YellowPen;
extern int trans_done;
extern vector rotdir,rotpnt;
extern matrix Nrot,Nper,Npar;
extern int view,num_polygons;
extern float zview;
extern vector cop;
extern vector ov,ov1,nv,nv1;
extern unsigned long grays[];
extern void normalize();
void zbuffer();
void view_wireframe();
float norm();
extern vertex light,light1;
int spec,culled;
float ints,kdiff,ambient;

float dot_product(a,b)
vertex a,b;
{
    return((a[0]*b[0])+(a[1]*b[1])+(a[2]*b[2]));
}

float norm(a)
vertex a;
{
    return(sqrt(sqr(a[0])+sqr(a[1])+sqr(a[2])));
}
float power(a,b)
float a;
int b;
{
    int i;
    float j;
    j = 1;
    for (i=0;i<=b;i++)
        j *= a;
    return(j);
}

void zbuffer(n,shading)
int n,shading;
{
    float zbuf[WINDOW_V][WINDOW_H];
    int scany,scanyend,i,j,k,xmin,xmid,xmax,ymin,ymax,ymid;
    float dx1,dx2,dx3,delta,delta1,dzx,dzy,zposold,zpos,temp,scanx,scanx1;
    float snorm,lnorm,sx,intensity,ph1,ph2,ph3,dintens,dintensity,intens1;
    float intens2,dzx1,dzx2,dzx3,dz,deltaz,deltaz1,scanxs,scanz,scanz1;
    float gourdx1,gourdx2,gourdx3,gourint1,gourint2,gourint3,dyint1,dyint2,
          dyint3, yint,dyint,yint1,yint2,yint3;
    FILE *fp,*fp1;
    vector q[3],p[3],v[3],q1,p1,lightvec;        
    vertex v1,v2,vbf,center,bfvec,newnormal,a,b,snormal,a1,b1,phongdx1,
           phongdx2,phongdx3,dphong1,dphong2,phong1,phong2,phongdx,pointnormal,
           halfway,iv1,iv2,lightvec1,tempvec;
    vertex lvec1,lvec2,lvec3,wp[3],dwpside,dwpside1,wpside,wpside1;
    vertex wpvec,wpdx,wpdx1,wpdx3,wpdx2;

/******************************************
* clear the z-buffer and frame buffer
*******************************************/
    for (i=0;i<WINDOW_H;i++)
        for (j=0;j<WINDOW_V;j++)
            zbuf[i][j] = FLT_MAX;
    XClearWindow(theDisplay,mainWindow);

/******************************************
* do this for all polygons 
*******************************************/  
    for (i=0;i<n;i++)  {
/********************************************
* find center of polygon
**********************************************/
        center[0] = (vertices[polygons[i].verts[0]][0] +
                    vertices[polygons[i].verts[1]][0] +
                    vertices[polygons[i].verts[2]][0]) /3.0;
        center[1] = (vertices[polygons[i].verts[0]][1] +
                    vertices[polygons[i].verts[1]][1] +
                    vertices[polygons[i].verts[2]][1]) /3.0;
        center[2] = (vertices[polygons[i].verts[0]][2] +
                    vertices[polygons[i].verts[1]][2] +
                    vertices[polygons[i].verts[2]][2]) /3.0;
        normalize(center);
/***************************************************
* find vector from center of polygon to cop
****************************************************/
        bfvec[0] = cop[0]-center[0];
        bfvec[1] = cop[1]-center[1];
        bfvec[2] = cop[2]-center[2];
        normalize(bfvec);
/******************************************************
* find polygon surface normal
******************************************************/
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
/***************************************************

* continue only if polygon is visible
*****************************************************/
        if ((dot_product(bfvec,snormal) > 0) || !culled) { 
/*****************************************************
* find vector for light source
*****************************************************/
            if (shading == FLAT) {
                lightvec[0] = light[0] - center[0];
                lightvec[1] = light[1] - center[1];
                lightvec[2] = light[2] - center[2];
                normalize(lightvec);
                lightvec1[0] = light1[0] -center[0];
                lightvec1[1] = light1[1] -center[1];
                lightvec1[2] = light1[2] -center[2] ;
                normalize(lightvec1);
            }
/*********************************************************
* if POLYGONAL shading, find intensity of surface
*********************************************************/
            if (shading == FLAT) {
                intensity = ambient + (ints*(dot_product(snormal,lightvec)));
                if (intensity < 0) intensity = 0;
                if (intensity > 1) intensity = 1;
                XSetForeground(theDisplay,mainGC,grays[(int)(127*intensity)]);
            }
/******************************************************************
* find normal of transformed polygon for z-buffer
*********************************************************/
            for (j=0;j<3;j++) {
                for (k=0;k<3;k++) {
                    q1[k] = vertices[polygons[i].verts[j]][k];
                    wp[j][k] = vertices[polygons[i].verts[j]][k];
                }
                q1[3] = 1;
                vec_mult(q1,Nper,p1);
                p[j][0]=((zview+(p1[0]/p1[3]))/(2*zview)*(float)(WINDOW_H-1.0));
                p[j][1]=((zview+(p1[1]/p1[3]))/(2*zview)*(float)(WINDOW_V-1.0)); 
                p[j][2]=p1[2];
            } 
            a1[0] = p[1][0]-p[0][0];
            a1[1] = p[1][1]-p[0][1];
            a1[2] = p[1][2]-p[0][2];
            b1[0] = p[2][0]-p[0][0];
            b1[1] = p[2][1]-p[0][1];
            b1[2] = p[2][2]-p[0][2];
            cross_product(a1,b1,newnormal);
            normalize(newnormal);
/******************************************************
* find min & max y values of polygon
********************************************************/
            ymin = 0;
            ymax = ymin;
            for (j=1;j<3;j++) {
                if (p[j][1] <
                    p[ymin][1])
                    ymin = j;
                if (p[j][1] >
                    p[ymax][1])
                    ymax = j;
            }
      
            for (j=0;j<3;j++)
                if (j != ymax && j != ymin)
                    ymid = j;
/*************************************
* compute slopes of triangle edges
**************************************/
        dx1 = (p[ymax][0]-
               p[ymin][0])/
              (round(p[ymax][1]) -
               round(p[ymin][1]));
        dx2 = (p[ymid][0]-
               p[ymin][0])/
              (round(p[ymid][1]) -
                round(p[ymin][1]));
        dx3 = (p[ymax][0]-
               p[ymid][0])/
              (round(p[ymax][1]) -
               round(p[ymid][1]));
/**********************************************
* compute delta z values and initial z position
***********************************************/

        dzx1 = (p[ymax][2]-p[ymin][2])/(round(p[ymax][1])-round(p[ymin][1]));
        dzx2 = (p[ymid][2]-p[ymin][2])/(round(p[ymid][1])-round(p[ymin][1]));
        dzx3 = (p[ymax][2]-p[ymid][2])/(round(p[ymax][1])-round(p[ymid][1]));

/***************************************************
* compute top and bottom scan lines
****************************************************/
        scany = rint(p[ymin][1]);
        scanyend = rint(p[ymax][1]);
/****************************************************
* compute Gouraud intensities at vertices and deltas
*****************************************************/
        lvec1[0] =light[0] - vertices[polygons[i].verts[ymin]][0]; 
        lvec1[1] =light[1] - vertices[polygons[i].verts[ymin]][1];
        lvec1[2] =light[2] - vertices[polygons[i].verts[ymin]][2];

        lvec2[0] =light[0] - vertices[polygons[i].verts[ymid]][0];
        lvec2[1] =light[1] - vertices[polygons[i].verts[ymid]][1];
        lvec2[2] =light[2] - vertices[polygons[i].verts[ymid]][2];

        lvec3[0] =light[0] - vertices[polygons[i].verts[ymax]][0];
        lvec3[1] =light[1] - vertices[polygons[i].verts[ymax]][1];
        lvec3[2] =light[2] - vertices[polygons[i].verts[ymax]][2];
        normalize(lvec1);
        normalize(lvec2);
        normalize(lvec3);
        gourint1 = ambient + 
          (ints*(dot_product(vnormals[polygons[i].verts[ymin]],lvec1))); 
        gourint2 = ambient +
          (ints*(dot_product(vnormals[polygons[i].verts[ymid]],lvec2)));
        gourint3 = ambient +
          (ints*(dot_product(vnormals[polygons[i].verts[ymax]],lvec3)));
        if (gourint1 <0) gourint1 = 0;
        if (gourint2 <0) gourint2 = 0;
        if (gourint3 <0) gourint3 = 0;
        if (gourint1 >1) gourint1 = 1;
        if (gourint2 >1) gourint2 = 1;
        if (gourint3 >1) gourint3 = 1;

        gourdx1 = (gourint3-gourint1)/(round(p[ymax][1])-round(p[ymin][1]));
        gourdx2 = (gourint2-gourint1)/(round(p[ymid][1])-round(p[ymin][1]));
        gourdx3 = (gourint3-gourint2)/(round(p[ymax][1])-round(p[ymid][1]));

/******************************************************
* Compute normal vector interpolations
* and set up interpolation of light vectors
*******************************************************/

        ph1 = round(p[ymax][1])-round(p[ymin][1]);
        ph2 = round(p[ymid][1])-round(p[ymin][1]);
        ph3 = round(p[ymax][1])-round(p[ymid][1]);
        if (shading == PHONG ) {
            wpdx1[0] = (wp[ymax][0]-wp[ymin][0])/ph1;
            wpdx1[1] = (wp[ymax][1]-wp[ymin][1])/ph1;
            wpdx1[2] = (wp[ymax][2]-wp[ymin][2])/ph1;

            wpdx2[0] = (wp[ymid][0]-wp[ymin][0])/ph2;
            wpdx2[1] = (wp[ymid][1]-wp[ymin][1])/ph2;
            wpdx2[2] = (wp[ymid][2]-wp[ymin][2])/ph2;

            wpdx3[0] = (wp[ymax][0]-wp[ymid][0])/ph3;
            wpdx3[1] = (wp[ymax][1]-wp[ymid][1])/ph3;
            wpdx3[2] = (wp[ymax][2]-wp[ymid][2])/ph3;
            phongdx1[0] = (vnormals[polygons[i].verts[ymax]][0]-
                        vnormals[polygons[i].verts[ymin]][0])/ph1;
            phongdx1[1] = (vnormals[polygons[i].verts[ymax]][1]-
                        vnormals[polygons[i].verts[ymin]][1])/ph1;
            phongdx1[2] = (vnormals[polygons[i].verts[ymax]][2]-
                        vnormals[polygons[i].verts[ymin]][2])/ph1;

            phongdx2[0] = (vnormals[polygons[i].verts[ymid]][0]-
                         vnormals[polygons[i].verts[ymin]][0])/ph2;
            phongdx2[1] = (vnormals[polygons[i].verts[ymid]][1]-
                        vnormals[polygons[i].verts[ymin]][1])/ph2;
            phongdx2[2] = (vnormals[polygons[i].verts[ymid]][2]-
                        vnormals[polygons[i].verts[ymin]][2])/ph2;

            phongdx3[0] = (vnormals[polygons[i].verts[ymax]][0]-
                        vnormals[polygons[i].verts[ymid]][0])/ph3;
            phongdx3[1] = (vnormals[polygons[i].verts[ymax]][1]-
                        vnormals[polygons[i].verts[ymid]][1])/ph3;
            phongdx3[2] = (vnormals[polygons[i].verts[ymax]][2]-
                        vnormals[polygons[i].verts[ymid]][2])/ph3;

        }
/****************************************************
* compute initial x positions for 1st scan line
*****************************************************/
        scanx = p[ymin][0];
        scanz = p[ymin][2];
        yint  = gourint1;
        if (shading == PHONG) {
            wpside[0] = wp[ymin][0];
            wpside[1] = wp[ymin][1];
            wpside[2] = wp[ymin][2];
            wpside1[0] = wp[ymin][0];
            wpside1[1] = wp[ymin][1];
            wpside1[2] = wp[ymin][2];
            phong1[0] = vnormals[polygons[i].verts[ymin]][0];
            phong1[1] = vnormals[polygons[i].verts[ymin]][1];
            phong1[2] = vnormals[polygons[i].verts[ymin]][2];
            phong2[0] = vnormals[polygons[i].verts[ymin]][0];
            phong2[1] = vnormals[polygons[i].verts[ymin]][1];
            phong2[2] = vnormals[polygons[i].verts[ymin]][2];
        }
        if (rint(p[ymid][1]) != rint( p[ymin][1])) { 
            scanx1 = scanx;
            scanz1 = scanz;
            yint1 = yint; 
            if (dx1 > dx2) {
                dyint = gourdx2;
                dyint1 = gourdx1;
                delta = dx2;
                delta1 = dx1;
                deltaz = dzx2;
                deltaz1 = dzx1;
                if (shading == PHONG) {
                    dwpside[0] = wpdx2[0];
                    dwpside[1] = wpdx2[1];
                    dwpside[2] = wpdx2[2];
                    dwpside1[0] = wpdx1[0];
                    dwpside1[1] = wpdx1[1];
                    dwpside1[2] = wpdx1[2];
                    dphong1[0] = phongdx2[0];
                    dphong1[1] = phongdx2[1];
                    dphong1[2] = phongdx2[2];
                    dphong2[0] = phongdx1[0];
                    dphong2[1] = phongdx1[1];
                    dphong2[2] = phongdx1[2];
                }
            }
            else {
                delta = dx1;
                delta1 = dx2;
                deltaz = dzx1;
                deltaz1 = dzx2;
                dyint = gourdx1;
                dyint1 = gourdx2;
                if (shading == PHONG) {
                    dphong1[0] = phongdx1[0];
                    dphong1[1] = phongdx1[1];
                    dphong1[2] = phongdx1[2];
                    dphong2[0] = phongdx2[0];
                    dphong2[1] = phongdx2[1];
                    dphong2[2] = phongdx2[2];
                    dwpside[0] = wpdx1[0];
                    dwpside[1] = wpdx1[1];
                    dwpside[2] = wpdx1[2];
                    dwpside1[0] = wpdx2[0];
                    dwpside1[1] = wpdx2[1];
                    dwpside1[2] = wpdx2[2];
                }
            }
        }
        else {
            if (p[ymin][0] <
                p[ymid][0]) {
                scanx1 = p[ymid][0];
                scanz1 = p[ymid][2];
                yint1 = gourint2;
                dyint = gourdx1;
                dyint1 = gourdx3;
                delta = dx1;
                delta1 = dx3;
                deltaz = dzx1;
                deltaz1 = dzx3;
                if (shading == PHONG) {
                    phong2[0] = vnormals[polygons[i].verts[ymid]][0];
                    phong2[1] = vnormals[polygons[i].verts[ymid]][1];
                    phong2[2] = vnormals[polygons[i].verts[ymid]][2];
                    dphong1[0] = phongdx1[0];
                    dphong1[1] = phongdx1[1];
                    dphong1[2] = phongdx1[2];
                    dphong2[0] = phongdx3[0];
                    dphong2[1] = phongdx3[1];
                    dphong2[2] = phongdx3[2];
                    wpside1[0] = wp[ymid][0];
                    wpside1[1] = wp[ymid][1];
                    wpside1[2] = wp[ymid][2];
                    dwpside[0] = wpdx1[0];
                    dwpside[1] = wpdx1[1];
                    dwpside[2] = wpdx1[2];
                    dwpside1[0] = wpdx3[0];
                    dwpside1[1] = wpdx3[1];
                    dwpside1[2] = wpdx3[2];
                }
            }
            else {
                yint1 = yint;
                yint = gourint2;
                scanx1 = scanx;
                scanx = p[ymid][0];
                scanz1 = scanz;
                scanz = p[ymid][2];
                delta = dx3;
                delta1 = dx1;
                deltaz = dzx3;
                deltaz1 = dzx1;
                dyint = gourdx3;
                dyint1 = gourdx1;
                zpos = p[ymid][2];
                if (shading == PHONG) {
                    phong2[0] = phong1[0];
                    phong2[1] = phong1[1];
                    phong2[2] = phong1[2];
                    phong1[0] = vnormals[polygons[i].verts[ymid]][0];
                    phong1[1] = vnormals[polygons[i].verts[ymid]][1];
                    phong1[2] = vnormals[polygons[i].verts[ymid]][2];
                    dphong1[0] = phongdx3[0];
                    dphong1[1] = phongdx3[1];
                    dphong1[2] = phongdx3[2];
                    dphong2[0] = phongdx1[0];
                    dphong2[1] = phongdx1[1];
                    dphong2[2] = phongdx1[2];
                    wpside1[0] = wpside[0]; 
                    wpside1[1] = wpside[1];
                    wpside1[2] = wpside[2];
                    wpside[0] = wp[ymid][0];
                    wpside[1] = wp[ymid][1];
                    wpside[2] = wp[ymid][2];
                    dwpside[0] = wpdx3[0];
                    dwpside[1] = wpdx3[1];
                    dwpside[2] = wpdx3[2];
                    dwpside1[0] = wpdx1[0];
                    dwpside1[1] = wpdx1[1];
                    dwpside1[2] = wpdx1[2];
                }
            }                
        }
/***************************************
* fill the z-buffer 
****************************************/
        for (k = scany; k<=scanyend;k++) {
            zpos = scanz;
            dz = (scanz1-scanz)/(round(scanx1)-round(scanx));
            dintensity = yint;
            if (shading == PHONG) {
                iv1[0] = wpside[0];
                iv1[1] = wpside[1];
                iv1[2] = wpside[2];
                iv2[0] = wpside1[0];
                iv2[1] = wpside1[1];
                iv2[2] = wpside1[2];
                wpvec[0] = iv1[0];
                wpvec[1] = iv1[1];
                wpvec[2] = iv1[2];
                ph3 = round(scanx1) - round(scanx);
                wpdx[0] = (iv2[0]-iv1[0])/ph3;
                wpdx[1] = (iv2[1]-iv1[1])/ph3;
                wpdx[2] = (iv2[2]-iv1[2])/ph3;
                iv1[0] = phong1[0];
                iv1[1] = phong1[1];
                iv1[2] = phong1[2];
                iv2[0] = phong2[0];
                iv2[1] = phong2[1];
                iv2[2] = phong2[2];
                pointnormal[0] = iv1[0];
                pointnormal[1] = iv1[1];
                pointnormal[2] = iv1[2];
                ph3 = round(scanx1) - round(scanx);
                phongdx[0] = (iv2[0] - iv1[0])/ph3;
                phongdx[1] = (iv2[1] - iv1[1])/ph3;
                phongdx[2] = (iv2[2] - iv1[2])/ph3;
            }
            if (shading == GOURAUD) {
                dintens = (yint1-yint)/(round(scanx1)-round(scanx));
            }
            for (j=rint(scanx);j<=rint(scanx1);j++) {
                if (zpos < zbuf[j][k]) {
                    zbuf[j][k] = zpos; 
                    if (shading == GOURAUD) {
                         XSetForeground(theDisplay,mainGC,
                             grays[(int)(127.0*dintensity)]);
                    }
                    if (shading == PHONG) {
                        lightvec[0] = light[0] - wpvec[0];
                        lightvec[1] = light[1] - wpvec[1];
                        lightvec[2] = light[2] - wpvec[2];
                        normalize(lightvec);
                        tempvec[0] = pointnormal[0];
                        tempvec[1] = pointnormal[1];
                        tempvec[2] = pointnormal[2];
                        normalize(tempvec);
                        halfway[0] = (bfvec[0]+pointnormal[0])/2;
                        halfway[1] = (bfvec[1]+pointnormal[1])/2;
                        halfway[2] = (bfvec[2]+pointnormal[2])/2;
                        normalize(halfway);
                        intensity = ambient +
                            (ints*((kdiff*dot_product(tempvec,lightvec)) +
                            ((1-kdiff)*
                            power(dot_product(tempvec,halfway),spec))));
                        if (intensity < 0) intensity = 0;
                        if (intensity > 1) intensity = 1;
                        XSetForeground(theDisplay,mainGC,grays[(int)(127*intensity)]);
                    }
                    XDrawPoint(theDisplay,mainWindow,mainGC,j,
                        WINDOW_V-k); 
                } 
                zpos += dz;
                if (shading == PHONG) {
                    pointnormal[0] += phongdx[0];
                    pointnormal[1] += phongdx[1];
                    pointnormal[2] += phongdx[2];
                    wpvec[0] += wpdx[0];
                    wpvec[1] += wpdx[1];
                    wpvec[2] += wpdx[2];
                }
                if (shading == GOURAUD) {
                    dintensity += dintens;
                }
            }
            scanz  +=  deltaz;
            scanz1 += deltaz1;
            scanx  += delta;
            scanx1 += delta1;
            yint   += dyint;
            yint1  += dyint1;
            if (shading == PHONG)  {
                phong1[0] += dphong1[0];
                phong1[1] += dphong1[1];
                phong1[2] += dphong1[2];
                phong2[0] += dphong2[0];
                phong2[1] += dphong2[1];
                phong2[2] += dphong2[2];
                wpside[0] += dwpside[0];
                wpside[1] += dwpside[1];
                wpside[2] += dwpside[2];
                wpside1[0] += dwpside1[0];
                wpside1[1] += dwpside1[1];
                wpside1[2] += dwpside1[2];
            }

            if ( (k+1) == rint(p[ymid][1])) {
                if (dx1>dx2) {
                    delta = dx3;
                    deltaz = dzx3;
                    dyint = gourdx3;
                    dphong1[0] = phongdx3[0];
                    dphong1[1] = phongdx3[1];
                    dphong1[2] = phongdx3[2];
                    dwpside[0] = wpdx3[0];
                    dwpside[1] = wpdx3[1];
                    dwpside[2] = wpdx3[2];
                }
                else {
                    delta1 = dx3;
                    deltaz1 = dzx3;
                    dyint1  = gourdx3;
                    dphong2[0] = phongdx3[0];
                    dphong2[1] = phongdx3[1];
                    dphong2[2] = phongdx3[2];
                    dwpside1[0] = wpdx3[0];
                    dwpside1[1] = wpdx3[1];
                    dwpside1[2] = wpdx3[2];
                }
            }
        }
        }
    }
} 

void view_wireframe() {
    int i,j,x1,x2,y1,y2;

    XClearWindow(theDisplay,mainWindow);

    for (i = 0; i < num_polygons; i++) { 
        if (polygons[i].color==1) XSetForeground(theDisplay,mainGC,RedPen);
        if (polygons[i].color==2) XSetForeground(theDisplay,mainGC,GreenPen);
        if (polygons[i].color==3) XSetForeground(theDisplay,mainGC,BluePen);
        if (polygons[i].color==4) XSetForeground(theDisplay,mainGC,YellowPen);


        ov1[0] = vertices[polygons[i].verts[2]][0]; 
        ov1[1] = vertices[polygons[i].verts[2]][1]; 
        ov1[2] = vertices[polygons[i].verts[2]][2]; 
        ov1[3] = 1; 

        if (view == PARALLEL) vec_mult(ov1, Npar, ov);
        else vec_mult(ov1, Nper, ov);

        for (j = 0; j < 3; j++) { 

            nv1[0] = vertices[polygons[i].verts[j]][0]; 
            nv1[1] = vertices[polygons[i].verts[j]][1]; 
            nv1[2] = vertices[polygons[i].verts[j]][2]; 
            nv1[3] = 1; 
            if (view == PARALLEL) {
            vec_mult(nv1, Npar, nv); 
                x1 = (int)((ov[0] / ov[3])*(WINDOW_H-1.0)); 
                y1 = (WINDOW_V-1.0) - (int)((ov[1] / ov[3])*(WINDOW_V-1.0)); 
                x2 = (int)((nv[0] / nv[3])*(WINDOW_H-1.0)); 
                y2 = (WINDOW_V-1.0) - (int)((nv[1] / nv[3])*(WINDOW_V-1.0)); 
            }
            else {
                vec_mult(nv1,Nper,nv);
                x1 = (int)( (zview+(ov[0] / ov[3]))/(2*zview) *(WINDOW_H-1.0));
                y1 = (WINDOW_V-1.0) -
                    (int)((zview+(ov[1] / ov[3]))/(2*zview)*(WINDOW_V-1.0));
                x2 =(int)((zview+(nv[0] / nv[3]))/(2*zview)*(WINDOW_H-1.0));
                y2 = (WINDOW_V-1.0) -
                    (int)((zview+(nv[1] / nv[3]))/(2*zview)*(WINDOW_V-1.0));
            }
            XDrawLine(theDisplay,mainWindow,mainGC,x1, y1, x2, y2); 

            ov[0] = nv[0]; 
            ov[1] = nv[1]; 
            ov[2] = nv[2]; 
            ov[3] = nv[3]; 
        } 
    } 
}
