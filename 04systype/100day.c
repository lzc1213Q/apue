#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define TIMESTRINGSIZE 1024
int main()
{
	time_t stamp;
	struct tm *tm;
	char timestr[TIMESTRINGSIZE] = "";
	stamp = time(NULL);
	tm = localtime(&stamp);
	strftime(timestr,TIMESTRINGSIZE,"NOW:%Y-%m-%d",tm);
	puts(timestr);
	
	tm->tm_mday += 100;
	(void)mktime(tm);
	strftime(timestr,TIMESTRINGSIZE,"100 days later:%Y-%m-%d",tm);
	puts(timestr);

	
	exit(0);
}
