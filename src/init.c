#include "codexion.h"

static void	set_coder_dongles(t_simu *simu, int index)
{
	if (simu->number_of_coders == 1)
	{
		simu->coders[index].left_dongle = 0;
		simu->coders[index].right_dongle = 0;
		return ;
	}
	simu->coders[index].left_dongle = index;
	simu->coders[index].right_dongle = (index + 1) % simu->number_of_coders;
}

static int	init_mutexes(t_simu *simu)
{
	if (pthread_mutex_init(&simu->sim_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&simu->log_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&simu->scheduler_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&simu->resources_mutex, NULL) != 0)
		return (1);
	if (pthread_cond_init(&simu->sim_cond, NULL) != 0)
		return (1);
	return (0);
}

static void	init_coders(t_simu *simu)
{
	int	index;

	index = 0;
	while (index < simu->number_of_coders)
	{
		simu->coders[index].id = index + 1;
		simu->coders[index].simu = simu;
		simu->coders[index].state = STATE_IDLE;
		simu->coders[index].compiles = 0;
		simu->coders[index].last_compile_start = get_timestamp_ms();
		simu->coders[index].deadline = simu->coders[index].last_compile_start +
			(long long)simu->burnout_ms;
		simu->coders[index].request_id = 0;
		simu->coders[index].left_taken = 0;
		simu->coders[index].right_taken = 0;
		set_coder_dongles(simu, index);
		index++;
	}
}

t_simu	*simu_init_from_args(int argc, char **argv)
{
	t_simu	*simu;
	int		index;

	simu = calloc(1, sizeof(*simu));
	if (simu == NULL)
		return (NULL);
	if (!parse_arguments(argc, argv, simu))
	{
		free(simu);
		return (NULL);
	}
	if (init_mutexes(simu) != 0)
	{
		free(simu);
		return (NULL);
	}
	simu->start_time_ms = get_timestamp_ms();
	simu->dongles = calloc((size_t)simu->number_of_coders, sizeof(*simu->dongles));
	simu->coders = calloc((size_t)simu->number_of_coders, sizeof(*simu->coders));
	if (simu->dongles == NULL || simu->coders == NULL)
	{
		cleanup_simulation(simu);
		return (NULL);
	}
	index = 0;
	while (index < simu->number_of_coders)
	{
		if (dongle_init(&simu->dongles[index], index, simu->cooldown_ms,
				simu->number_of_coders) != 0)
		{
			cleanup_simulation(simu);
			return (NULL);
		}
		index++;
	}
	init_coders(simu);
	return (simu);
}
