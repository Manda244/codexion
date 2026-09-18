#include "codexion.h"

static int	start_threads(t_simu *simu)
{
	int	index;

	if (pthread_create(&simu->monitor_thread, NULL, monitor_routine, simu) != 0)
		return (1);
	index = 0;
	while (index < simu->number_of_coders)
	{
		if (pthread_create(&simu->coders[index].thread, NULL,
				coder_routine, &simu->coders[index]) != 0)
			return (1);
		index++;
	}
	pthread_mutex_lock(&simu->sim_mutex);
	while (!simu->stop)
		pthread_cond_wait(&simu->sim_cond, &simu->sim_mutex);
	pthread_mutex_unlock(&simu->sim_mutex);
	pthread_join(simu->monitor_thread, NULL);
	index = 0;
	while (index < simu->number_of_coders)
	{
		pthread_join(simu->coders[index].thread, NULL);
		index++;
	}
	return (0);
}

int	main(int argc, char **argv)
{
	t_simu	*simu;
	int		ret;

	simu = simu_init_from_args(argc, argv);
	if (simu == NULL)
		return (1);
	ret = start_threads(simu);
	if (ret != 0)
	{
		cleanup_simulation(simu);
		return (1);
	}
	cleanup_simulation(simu);
	return (0);
}
