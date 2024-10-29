#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE    1024

int path_noloop(const char *path) {
    // 去除 . 和 ..
    char *pos = strrchr(path, '/');
    if (pos) {
        if ((!strcmp("/.", pos))
                || (!strcmp("/..", pos))) {
            return 0;
        }
    } else if ((!strcmp(".", path)) || (!strcmp("..", path))) {
        return 0;
    }

    return 1;
}

int mydu(const char *path)
{// /a/b/c/d/f/g
    static char str[BUFSIZE] = "";
    glob_t globt;
    int i = 0, ret = 0;
    struct stat buf;

     if(lstat(path, &buf) < 0)
    {
        perror("lstat()");
        exit(1);
    }

    // path为目录文件
    if (S_ISDIR(buf.st_mode))
    {
        // 非隐藏文件
        snprintf(str, BUFSIZE, "%s/*", path);
        glob(str, 0, NULL, &globt);

        // 隐藏文件，将两次解析的结果追加到一块，所以特殊要求使用 GLOB_APPEND
        snprintf(str, BUFSIZE, "%s/.*", path);
        glob(str, GLOB_APPEND, NULL, &globt);

        ret = buf.st_blocks;

        for (i = 0; i < globt.gl_pathc; i++) {
            // 递归目录的时候要注意，目录并不是一个典型的树状结构，它是具有回路的，所以向下递归时遇到 . 和 .. 的时候不要进行递归
            if (path_noloop(globt.gl_pathv[i])) {
                ret += mydu(globt.gl_pathv[i]);
            }
        }

        // 用完了不要忘记释放资源
        globfree(&globt);
    } else { // path 为非目录文件
        ret = buf.st_blocks;
    }

    return ret;
}

int main(int argc, char **argv)
{
    printf("%d\n",mydu(argv[1]) / 2);

    exit(0);
}