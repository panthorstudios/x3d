#include "main.h"
#define pi  3.141592654
extern vertex *vertices;
extern triangle *polygons;
extern Display *theDisplay;
extern GC mainGC;
extern Window mainWindow;
extern unsigned long RedPen,GreenPen,BluePen,YellowPen;
extern int shading,trans_done;
extern vector rotdir,rotpnt;
extern view_wireframe();

vector  p1,p2,p3,vrp,vup,dov,vpn,dop,cop,ov,ov1,nv,nv1;
matrix  Nper,Npar,Nrot,tmp;
int     view,num_vertices,num_polygons;
float   rotangle,umax,umin,vmax,vmin,back,front,zview;

void clear_matrix(c) 
float (*c)[4];
{
    int i,j;
 
    for (i = 0; i < 4; i++) 
        for (j = 0; j < 4; j++) 
            c[i][j] = 0; 
} 

void set_diag(c) 
float (*c)[4];
{
    int i;

    for (i = 0; i < 4; i++) 
        c[i][i] = 1; 
} 

void make_rot_matrix(rot, sn, cs, c) 
int rot; 
float sn; 
float cs; 
float (*c)[4]; 
{ 
    clear_matrix(c); 
    set_diag(c); 
    if (rot == XROT) {  
        c[1][1] = cs; c[2][2] = cs; 
        c[2][1] = sn; c[1][2] = -sn; 
    } 
    else if (rot == YROT) {  
        c[0][0] = cs; c[2][2] = cs; 
        c[2][0] = sn; c[0][2] = -sn; 
    } 
    else if (rot == ZROT ) {  
        c[0][0] = cs; c[1][1] = cs; 
        c[0][1] = sn; c[1][0] = -sn; 
    } 
} 

void make_transl_matrix(x, y, z, c) 
float x; 
float y; 
float z; 
float (*c)[4]; 
{
    clear_matrix(c); 
    set_diag(c); 
    c[3][0] = x; 
    c[3][1] = y; 
    c[3][2] = z; 
} 

void make_scale_matrix(x, y, z, c) 
float x; 
float y; 
float z;
float (*c)[4];
{ 
    clear_matrix(c); 
    set_diag(c); 
    c[0][0] = x; 
    c[1][1] = y; 
    c[2][2] = z; 
} 

void mat_mult(a, b, c)
float (*a)[4]; 
float (*b)[4]; 
float (*c)[4]; 
{
    int i,j,k;

    clear_matrix(c); 
    for (i = 0; i < 4; i++) {  
        for (j = 0; j < 4; j++) {  
            for (k = 0; k < 4; k++) {  
                c[i][j] = c[i][j] + (a[i][k] * b[k][j]); 
            } 
        } 
    } 
} 

void vec_mult(a, b, c) 
float a[4]; 
float (*b)[4]; 
float c[4]; 
{
    int i, j, k;
 
    for (i = 0; i < 4; i++) { ; 
        c[i] = 0; 
        for (j = 0; j < 4; j++) { ; 
            c[i] = c[i] + (a[j] * b[j][i]); 
        } 
    } 
} 

void get_transf_matrix(tr,tr1,view) 
float (*tr)[4];
float (*tr1)[4]; 
int view;
{
    matrix  m1a,m1,m2,m3,m4,m5,m6,m7,m8,m6a,m8a,m9,tmp;
    vector  p2pr,p2dpr,p2tpr,p3pr,p3dpr,p3tpr,doppr,doppr1,vrppr,vrppr1,
            coppr,coppr1;
    float  d,d1,d2,d3,a,b,sinephi1,sinephi2,sinephi3,cosinephi1,cosinephi2,
            cosinephi3, vpn_norm;
    int i,j;

    make_transl_matrix(-(vrp[0]+cop[0]),-(vrp[1]+cop[1]),
                       -(vrp[2]+cop[2]), m1a); 

    make_transl_matrix(-vrp[0], -vrp[1], -vrp[2], m1); 

    vec_mult(p2,m1,p2pr);
    d1 = (float)sqrt(sqr(p2pr[0]) + sqr(p2pr[2])); 
    sinephi1 = p2pr[0] / d1; 
    cosinephi1 = -p2pr[2] / d1; 
    make_rot_matrix(YROT, sinephi1, cosinephi1, m2); 

    vec_mult(p2pr, m2, p2dpr); 
    vpn_norm = sqrt(sqr(vpn[0]) + sqr(vpn[1]) + sqr(vpn[2])); 
    cosinephi2 = -p2dpr[2] / vpn_norm; 
    sinephi2 = p2dpr[1] / vpn_norm; 
    make_rot_matrix(XROT, sinephi2, cosinephi2, m3); 

    vec_mult(p3, m1, p3pr); 
    vec_mult(p3pr, m2, p3dpr); 
    vec_mult(p3dpr, m3, p3tpr); 
    d3 = (float)sqrt(sqr(p3tpr[0]) + sqr(p3tpr[1])); 
    cosinephi3 = p3tpr[1] / d3; 
    sinephi3 = p3tpr[0] / d3; 
    make_rot_matrix(ZROT, sinephi3, cosinephi3, m4); 

    clear_matrix(m5); 
    set_diag(m5); 
    m5[2][2] = -1.0; 

    vec_mult(dop, m2, doppr1); 
    vec_mult(doppr1, m3, doppr); 
    vec_mult(doppr, m4, doppr1); 
    vec_mult(doppr1, m5, doppr); 
    a = -doppr[0] / doppr[2]; 
    b = -doppr[1] / doppr[2]; 
    clear_matrix(m6); 
    set_diag(m6); 
    m6[2][0] = a; m6[2][1] = b; 

    vec_mult(vrp,m1a,vrppr); 
    vec_mult(vrppr, m2, vrppr1); 
    vec_mult(vrppr1, m3, vrppr); 
    vec_mult(vrppr, m4, vrppr1); 
    vec_mult(vrppr1, m5, vrppr); 

    clear_matrix(m6a); 
    set_diag(m6a); 

    a = -(vrppr[0] + (0.5 * (umax + umin))) / vrppr[2]; 
    b = -(vrppr[1] + (0.5 * (vmax + vmin))) / vrppr[2]; 
    m6a[2][0] = a; m6a[2][1] = b; 

    make_transl_matrix(-umin,-vmin,-front,m7); 
    make_scale_matrix((1/(umax-umin)),(1/(vmax-vmin)),(1/(back-front)),m8); 

    make_scale_matrix((2.0*vrppr[2]/((vrppr[2]+back)*(umax-umin)) ),
                      (2.0*vrppr[2]/((vrppr[2]+back)*(vmax-vmin)) ),
                      (1.0/(vrppr[2]+back)),m8a); 

    mat_mult(m1, m2, tr); 
    mat_mult(tr, m3, tmp); 
    mat_mult(tmp, m4, tr); 
    mat_mult(tr, m5, tmp); 
    mat_mult(tmp, m6, tr); 
    mat_mult(tr, m7, tmp); 
    mat_mult(tmp, m8, tr); 
    clear_matrix(m9); 
    set_diag(m9); 
    m9[2][2] = 0.0; 
    mat_mult(tr, m9, tmp);

    mat_mult(m1a, m2, tr1);
    mat_mult(tr1, m3, tmp);
    mat_mult(tmp, m4, tr1);
    mat_mult(tr1, m5, tmp);
    mat_mult(tmp, m6a, tr1); 
    mat_mult(tr1, m8a, tmp); 
    zview = vrppr[2]/(vrppr[2]+back); 
    clear_matrix(m9); 
    set_diag(m9); 
    m9[3][3] = 0; 
    m9[2][3] = 1.0/zview; 
    mat_mult(tmp, m9, tr1); 
} 

void make_axisrot_matrix(theta,c)
float theta;
float (*c)[4];
{
    vector n;
    float norm,n1,n2,n3;
    matrix m1,m2,m3,tmp;
    int i;

    theta *= pi/180;
    norm = sqrt(sqr(rotdir[0])+sqr(rotdir[1])+sqr(rotdir[2]));
    n1 = rotdir[0]/norm; n2 = rotdir[1]/norm; n3 = rotdir[2]/norm;
    make_transl_matrix(-rotpnt[0],-rotpnt[1],-rotpnt[2],m1);
    make_transl_matrix(rotpnt[0],rotpnt[1],rotpnt[2],m3);
    clear_matrix(m2);
    m2[0][0] = sqr(n1)+((1-sqr(n1))*cos(theta));
    m2[0][1] = (n1*n2*(1-cos(theta))) + (n3*sin(theta));
    m2[0][2] = (n1*n3*(1-cos(theta))) - (n2*sin(theta));

    m2[1][0] = (n1*n2*(1-cos(theta))) - (n3*sin(theta));
    m2[1][1] = sqr(n2) + ((1-sqr(n2))*cos(theta));
    m2[1][2] = (n2*n3*(1-cos(theta))) + (n1*sin(theta));

    m2[2][0] = (n1*n3*(1-cos(theta))) + (n2*sin(theta));
    m2[2][1] = (n2*n3*(1-cos(theta))) - (n1*sin(theta));
    m2[2][2] = sqr(n3) + ((1-sqr(n3))*cos(theta));

    m2[3][3] = 1;
    mat_mult(m1,m2,tmp);
    mat_mult(tmp,m3,c);
}

view3d(view)
int view;
{ 
    int x1,y1,x2,y2,i,j;

    if (!trans_done) {
        p2[0] = vpn[0] + vrp[0]; 
        p2[1] = vpn[1] + vrp[1]; 
        p2[2] = vpn[2] + vrp[2]; 
        p2[3] = 1; 

        p3[0] = vup[0] + vrp[0]; 
        p3[1] = vup[1] + vrp[1]; 
        p3[2] = vup[2] + vrp[2]; 
        p3[3] = 1; 

        get_transf_matrix(Npar,Nper,view); 
/*        for (i=0;i<4;i++) {
            for(j=0;j<4;j++)
                printf("%f ",Nper[i][j]);
            printf("\n");
        }
*/
        trans_done = TRUE;
    }
    if (shading == WIREFRAME) view_wireframe();
    else zbuffer(num_polygons,shading);
}
