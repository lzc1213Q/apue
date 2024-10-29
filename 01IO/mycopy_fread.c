#include <stdio.h>
#include <stdlib.h>
#define BUFSIZE 1024
int main(int argc, char **argv)
{
    FILE *fps,*fpd;
    char buf[BUFSIZE];
    int n;
    if (argc < 3)
    {
        fprintf(stderr,"Using:%s <src_file> <dis_file>\n",argv[0]);
        exit(1);
    }
    fps = fopen(argv[1],"r");
    if(fps == NULL)
    {
        perror("fopens()");
        exit(1);
    }
   fpd = fopen(argv[2],"w");
    if(fpd == NULL)
    {
        fclose(fps);
        perror("fopend()");
        exit(1);
    }
    //fread在读写文件的时候每次读取若干分字节，当文件字符不是该文件的整倍数就会导致读取文件有丢失。
    while((n = fread(buf, 1, BUFSIZE,fps)) > 0)
    {
      fwrite(buf, 1, n, fpd);
    }
    fclose(fpd);
    fclose(fps);
}