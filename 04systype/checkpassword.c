/**
 * 验证用户密码是否正确
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <shadow.h>
int main(int argc, char **argv)
{
    char *input_pass;
    struct spwd *showline;
    char *cry_pass;

    if(argc < 2)
    {
        fprintf(stderr,"Usage....\n");
        exit(1);
    }
    input_pass = getpass("Password :");

    showline = getspnam(argv[1]);

    cry_pass = crypt(input_pass,showline->sp_pwdp);

    if(strcmp(showline->sp_pwdp,cry_pass) == 0)
        puts("ok !");
    else
        puts("failed");
    exit(0);
}