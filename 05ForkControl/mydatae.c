#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#define TIMESTRINGSIZE 1024
#define FMTSTRSIZE 1024
/*
 *	-y year
 *	-m month
 *	-d day
 *	-H hour
 *	-M minute
 *	-S second
 *
 *
 *
 *
 * */
int main(int argc, char **argv)
{
	time_t stamp;
	struct tm *tm;
	char timestr[TIMESTRINGSIZE] = "";
	stamp = time(NULL);
	tm = localtime(&stamp);
	int c;
	char fmtstr[FMTSTRSIZE] = "";
	fmtstr[0] = '\0';

	while(1)
	{
		c = getopt(argc, argv,"HMSymd");
		if(c < 0)
		break;
		switch(c)
		{
			case 'H':
				break;
				strncat(fmtstr,"%H ",FMTSTRSIZE);
			case 'M':
				strncat(fmtstr,"%M ",FMTSTRSIZE);
				break;
			case 'S':
				strncat(fmtstr,"%S ",FMTSTRSIZE);
				break;
			case 'y':
				strncat(fmtstr,"%y ",FMTSTRSIZE);
				break;
			case 'm':
				strncat(fmtstr,"%m ",FMTSTRSIZE);
				break;
			case 'd':
				strncat(fmtstr,"%d ",FMTSTRSIZE);
				break;
			default:
				break;

		}

	}
		
	strftime(timestr,TIMESTRINGSIZE,fmtstr,tm);
	puts(timestr);
	
	
	
	exit(0);
}
