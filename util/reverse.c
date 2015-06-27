#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define  sqr(M)    ((M)*(M))

typedef struct {
    unsigned long color;
    int verts[4];
} polygon;

typedef float vector[4];
typedef float matrix[4][4];
typedef float vertex[3];

void main(argc,argv)
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
    fp = fopen("test.x","w");
    fprintf(fp,"%d\n",num_vertices);
    for (i=0;i<num_vertices;i++) {
        fprintf(fp,"%f %f %f\n",vertices[i][0],vertices[i][1],vertices[i][2]);
    }
    fprintf(fp,"%d\n",num_polygons);
    for (i=0;i<num_polygons;i++) {
        fprintf(fp,"3 %lu\n",polygons[i].color);
        for (j=0;j<3;j++)
            fprintf(fp,"%d ",polygons[i].verts[2-j]);
        fprintf(fp,"\n");
    }
    fclose(fp);
    printf("All done\n");
}
