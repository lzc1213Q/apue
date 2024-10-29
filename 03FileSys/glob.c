#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <glob.h>
#include <string.h>


#define PATHSIZE 1024

int path_noloop(const char *path)
{
    char *pos =strrchr(path,'/');
    if(pos) 
    {
        if ((!strcmp("/.", pos)) || (!strcmp("/..", pos))) 
            {
                 return 0;
            }
        else if ((!strcmp(".", path)) || (!strcmp("..", path))) 
            {
                 return 0;
            }
       return 1;
    }
}
int mydu(const char *path)
{
    struct stat startres;
    glob_t globres;
    char nextpath[PATHSIZE] ="";
    int sum=0;
    int i;
   
    if(lstat(path, &startres) < 0)
    {
        perror("lstat()");
        exit(1);
    }
    if(!S_ISDIR(startres.st_mode))
        return startres.st_blocks;

        strncpy(nextpath,path,PATHSIZE);
        strncat(nextpath,"/*",PATHSIZE);
        glob(nextpath,0,NULL,&globres);
    
        strncpy(nextpath,path,PATHSIZE);
        strncat(nextpath,"/.*",PATHSIZE);
        glob(nextpath,GLOB_APPEND,NULL,&globres);
        sum = startres.st_blocks;
        for(i = 0; i < globres.gl_pathc; i++)
        {
            if (path_noloop(globres.gl_pathv[i])) 
                {
                    sum += mydu(globres.gl_pathv[i]);
                }
        }

      
        globfree(&globres);
      
        return sum;
}
int main(int argc, char **argv)
{
    if(argc < 2)
    {
        fprintf(stderr,"Usage.....\n");
        exit(1);
    }
    printf("%d\n",mydu(argv[1]) / 2);
    exit(0);
}