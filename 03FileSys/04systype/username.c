#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
int main(int argc, char **argv)
{
    struct passwd *pwdline;
    if(argc < 2)
    {
        fprintf(stderr,"Usage....\n");
        exit(1);
    }
    pwdline = getpwuid(atio(argv[1]));
    puts(pwdline->pw_name);
}