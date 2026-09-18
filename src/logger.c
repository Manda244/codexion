#include "codexion.h"

int	logger_state(t_simu *simu, int coder_id, const char *message)
{
	long long	timestamp;

	if (simu == NULL || message == NULL)
		return (1);
	timestamp = get_timestamp_ms();
	pthread_mutex_lock(&simu->log_mutex);
	printf("%lld %d %s\n", timestamp, coder_id, message);
	fflush(stdout);
	pthread_mutex_unlock(&simu->log_mutex);
	return (0);
}
