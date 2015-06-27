#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define  sqr(M)  ((M)*(M))

typedef struct {
    unsigned long color;
    int verts[3];
} polygon;

typedef float vector[4];
typedef float matrix[4][4];
typedef float vertex[3];

int main(argc,argv)
int argc;
char **argv;
{
    FILE *fp,*fp1;
    int  i,j,k,vertno,num_vertices,num_polygons,temp,*vcount;
    float c,r;
    polygon *polygons;
    vertex a,b,v,*vertices;
    char filename[80];
    int reverse = 0;

    if (!strcmp(argv[1],"-r")){
        reverse = 1;
        strcpy(filename,argv[2]);
    }
    else
        strcpy(filename,argv[1]);
    if ((fp=fopen(filename,"r")) == NULL) {
        printf("File %s not found.\n",filename);
        exit(1);
    }
    fscanf(fp,"%d",&num_vertices);
    if ((vertices = (vertex *)calloc(num_vertices,sizeof(vertex))) == NULL)
        printf("vertex allocation failed!\n");
    for (i = 0; i < num_vertices; i++) {
        fscanf(fp,"%f %f %f\n",&vertices[i][0],&vertices[i][1],&vertices[i][2]);
    }
    fscanf(fp,"%d",&num_polygons);
    polygons = (polygon *)calloc(num_polygons,sizeof(polygon));
    temp = num_polygons;
    for (i = 0; i < num_polygons; i++) {
        fscanf(fp,"%d %lu",&k,&polygons[i].color);
        for (j = 0; j < 3; j++) {
            fscanf(fp,"%d",&k);
            polygons[i].verts[j] = k;
        }
    }
    fclose(fp);
    printf("File loaded: %d vertices, %d polygons.\nProcessing",num_vertices,num_polygons);
    for (i=0;i<num_polygons;i++) {
        for (j=0;j<3;j++) {
            v[0] = vertices[polygons[i].verts[j]][0];
            v[1] = vertices[polygons[i].verts[j]][1];
            v[2] = vertices[polygons[i].verts[j]][2];
            for (k=0;k<polygons[i].verts[j];k++) {
                if ((vertices[k][0] == v[0]) && (vertices[k][1] == v[1]) &&
                   (vertices[k][2] == v[2])) {
                    polygons[i].verts[j] = k;
                    break;
                }
            }
        } 
        printf(".");
    }
    printf("\nCreating new vertex list");
    vertno = 0;
    for (i=0;i<num_polygons;i++) {
        for (j=0;j<3;j++) {
            if (polygons[i].verts[j]>vertno) vertno = polygons[i].verts[j];
        }
        printf(".");
    }
    printf("\nMaximum vertex number = %d\nWriting new file...\n",vertno);
    fp = fopen("test.x","w");
    fprintf(fp,"%d\n",vertno);
    for (i=0;i<vertno;i++) {
        fprintf(fp,"%f %f %f\n",vertices[i][0],vertices[i][1],vertices[i][2]);
    }
    fprintf(fp,"%d\n",num_polygons);
    for (i=0;i<num_polygons;i++) {
        fprintf(fp,"3 %lu\n",polygons[i].color);
        for (j=0;j<3;j++)
            if (reverse ==0) fprintf(fp,"%d ",polygons[i].verts[j]);
            else fprintf(fp,"%d ",polygons[i].verts[2-j]);
        fprintf(fp,"\n");
    }
    fclose(fp);
    printf("All done\n");
}
