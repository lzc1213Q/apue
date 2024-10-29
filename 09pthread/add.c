#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#define THRNUM 20
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static void *fun(void *p)
{
    int fd;
    long long n = 0;
    char buf[THRNUM] = "";
    fd = open(p, O_RDWR | O_CREAT, 0664);
    if (fd < 0)
    {
        perror("fopen()");
        exit(1);
    }
    // 临界区
    pthread_mutex_lock(&mutex);

    //    read(fd, buf, THRNUM);
    //    lseek(fd, 0, SEEK_SET);
    //    n = atoi(buf);
    //    snprintf(buf, THRNUM, "%lld\n", ++n);
    //    write(fd, buf, strlen(buf));
    fgets(linebuf, LINESIZE, fp);
    fseek(fp, 0, SEEK_SET);

    // sleep(1);

    fprintf(fp, "%d\n", atoi(linebuf) + 1); // 全缓冲，需要fflush 或者 fclose 刷新 才能写到目标文件
    fclose(fp);                             // fprintf(fp  全缓冲，需要fflush 或者 fclose 刷新 才能写到目标文件，所以也在临界区

    close(fd);

    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    pthread_t tid[20];
    int i = 0;
    int err;
    if (argc < 2)
    {
        fprintf(stderr, "Usage %s <filename\n>", argv[0]);
        exit(1);
    }
    for (i = 0; i < THRNUM; i++)
    {
        err = pthread_create(&tid[i], NULL, fun, argv[1]);
        if (err)
        {
            fprintf(stderr, "pthread_create(): %s\n", strerror(err));
            exit(1);
        }
    }
    for (i = 0; i < THRNUM; i++)
    {
        pthread_join(tid[i], NULL);
    }
    pthread_mutex_destroy(&mutex);
    // pthread_mutex_destory(&mutex);

    exit(0);
}