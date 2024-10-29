#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
static off_t flen(const char *fname)
{
    struct stat startres;
    if(stat(fname,&startres) < 0)
    {
        perror("stat()");
        exit(1);
    }
    return startres.st_size;


}
int main(int argc,   char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr,"Usage....\n");
        exit(1);
    }

    printf("%lld\n", (long long)flen(argv[1]));
    exit(0);
}