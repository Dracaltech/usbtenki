#include <time.h>
#include <sys/time.h>
#include <stdio.h>

// yyyy-mm-dd hh:mm:ss.000

void printTimeStamp(FILE *stream)
{
	time_t t;
	struct timeval tv_now;
	struct tm *tm;

//	t = time(NULL);
	
	gettimeofday(&tv_now, NULL);
	t = tv_now.tv_sec;

	tm = localtime(&t);

	fprintf(stream, "%d-%02d-%02d %02d:%02d:%02d.%03d", 
		tm->tm_year + 1900, tm->tm_mon, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec,
				tv_now.tv_usec / 1000);

}
