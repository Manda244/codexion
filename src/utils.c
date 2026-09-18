#include "codexion.h"

long long	get_timestamp_ms(void)
{
	struct timeval	current;

	if (gettimeofday(&current, NULL) != 0)
		return (0);
	return ((long long)current.tv_sec * 1000LL +
		(long long)current.tv_usec / 1000LL);
}
