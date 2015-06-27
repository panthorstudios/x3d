#include <stdio.h>
#include <stdlib.h>

int main(argc,argv)
int argc;
char **argv;

{

    FILE *fp,*fp1;
    int  i,j,cnt,verts;
    char msg[80];
    float v1,v2,v3;

    fp=fopen("mig.asc","r");
    fp1 = fopen("mig.x","w");
    fscanf(fp,"%d",&verts);
    fprintf(fp1,"%d\n",verts);
    for (i=0;i<verts;i++) {
        fscanf(fp,"%f %f %f",&v1,&v2,&v3);
        fprintf(fp1,"%f %f %f\n",v1,v2,v3);
    }
    fscanf(fp,"%d",&verts);
    fprintf(fp1,"%d\n",verts);
    cnt = 0;
    for (i=0;i<verts;i++) {
        fprintf(fp1,"3 1\n");
        for (j=0;j<3;j++)
            fprintf(fp1,"%d ",cnt++);
        fprintf(fp1,"\n");
    }
    fclose(fp);
    fclose(fp1);
} 
