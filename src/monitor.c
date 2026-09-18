#include "codexion.h"

static int	check_all_done(t_simu *simu)
{
	int	index;
	int	count;

	count = 0;
	index = 0;
	while (index < simu->number_of_coders)
	{
		if (simu->coders[index].compiles >= simu->required_compiles)
			count++;
		index++;
	}
	return (count == simu->number_of_coders);
}

int	stop_simulation(t_simu *simu, int coder_id)
{
	int	index;

	if (simu == NULL)
		return (1);
	pthread_mutex_lock(&simu->sim_mutex);
	if (simu->stop)
	{
		pthread_mutex_unlock(&simu->sim_mutex);
		return (0);
	}
	simu->stop = 1;
	if (coder_id >= 0)
		simu->stop_by_burnout = 1;
	pthread_cond_broadcast(&simu->sim_cond);
	pthread_mutex_unlock(&simu->sim_mutex);
	index = 0;
	while (index < simu->number_of_coders)
	{
		pthread_mutex_lock(&simu->dongles[index].mutex);
		pthread_cond_broadcast(&simu->dongles[index].cond);
		pthread_mutex_unlock(&simu->dongles[index].mutex);
		index++;
	}
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_simu	*simu;
	int		index;
	long long	current;

	simu = (t_simu *)arg;
	while (1)
	{
		pthread_mutex_lock(&simu->sim_mutex);
		if (simu->stop)
		{
			pthread_mutex_unlock(&simu->sim_mutex);
			return (NULL);
		}
		if (check_all_done(simu))
		{
			pthread_mutex_unlock(&simu->sim_mutex);
			stop_simulation(simu, -1);
			return (NULL);
		}
		current = get_timestamp_ms();
		index = 0;
		while (index < simu->number_of_coders)
		{
			if (simu->coders[index].state != STATE_BURNOUT
				&& current >= simu->coders[index].last_compile_start +
					(long long)simu->burnout_ms)
			{
				simu->coders[index].state = STATE_BURNOUT;
				pthread_mutex_unlock(&simu->sim_mutex);
				logger_state(simu, simu->coders[index].id, "burned out");
				stop_simulation(simu, simu->coders[index].id);
				return (NULL);
			}
			index++;
		}
		pthread_mutex_unlock(&simu->sim_mutex);
		usleep(1000);
	}
	return (NULL);
}
