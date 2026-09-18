#include "codexion.h"

void	cleanup_simulation(t_simu *simu)
{
	int	index;

	if (simu == NULL)
		return ;
	if (simu->dongles != NULL)
	{
		index = 0;
		while (index < simu->number_of_coders)
		{
			dongle_destroy(&simu->dongles[index]);
			index++;
		}
		free(simu->dongles);
		simu->dongles = NULL;
	}
	if (simu->coders != NULL)
	{
		free(simu->coders);
		simu->coders = NULL;
	}
	pthread_mutex_destroy(&simu->sim_mutex);
	pthread_mutex_destroy(&simu->log_mutex);
	pthread_mutex_destroy(&simu->scheduler_mutex);
	pthread_mutex_destroy(&simu->resources_mutex);
	pthread_cond_destroy(&simu->sim_cond);
	free(simu);
}
