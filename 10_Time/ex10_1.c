/******************************************************************************\
* Exercise 10-1:                                                               *
\******************************************************************************/

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************\
* Converts seconds to years days hours minutes seconds                         *
\******************************************************************************/

char *convert_secs_to_yydddhhmmss(long double total_seconds)
{
    const long double seconds_per_day  = 24.0 * 3600.0;
    const long double seconds_per_year = 365.2422 * seconds_per_day;
    int years = (int)(total_seconds / seconds_per_year);
    int days  = (int)((total_seconds - seconds_per_year * years)/seconds_per_day);
    long double remain_seconds = total_seconds - seconds_per_year * years - seconds_per_day * days;
    int hours   = (int)(remain_seconds / 3600.0);
    int minutes = (int)((remain_seconds - 3600.0 * hours) / 60.0);
    int seconds = (int)(remain_seconds - 3600.0 * hours - 60.0 * minutes);
    
    char *result = (char *)malloc(256);
    if (result == NULL) return NULL;
    
    sprintf(result, "%d.%03d %02d:%02d:%02d", years, days, hours, minutes, seconds);
    return result;
}

/******************************************************************************\
* Calculate time to return clock to zero (0).                                  *
\******************************************************************************/

int main(int argc, char *argv[])
{
    const long double max_clock = (long double)UINT_MAX;
    
    printf("100 cycles per second: ");
    char *result = convert_secs_to_yydddhhmmss(max_clock/100.0);
    if (result == NULL)
    {
        fprintf(stderr, "%m\n");
        exit(1);
    }
    printf("%s\n", result);
    free(result);
    printf("1,000,000 cycles per second: ");
    result = convert_secs_to_yydddhhmmss(max_clock/1000000.0);
    if (result == NULL)
    {
        fprintf(stderr, "%m\n");
        exit(1);
    }
    printf("%s\n", result);
    free(result);
    exit(0);
}
