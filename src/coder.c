#include "codexion.h"

static int	all_done(t_simu *simu)
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

void	acquire_two_dongles(t_coder *coder)
{
	t_simu	*simu;
	int		left;
	int		right;

	simu = coder->simu;
	pthread_mutex_lock(&simu->resources_mutex);
	if (simu->number_of_coders == 1)
	{
		if (dongle_acquire(&simu->dongles[0], coder, simu) != 0)
		{
			pthread_mutex_unlock(&simu->resources_mutex);
			return ;
		}
		coder->left_taken = 1;
		coder->right_taken = 1;
		pthread_mutex_unlock(&simu->resources_mutex);
		return ;
	}
	left = coder->left_dongle;
	right = coder->right_dongle;
	if (left > right)
	{
		left = coder->right_dongle;
		right = coder->left_dongle;
	}
	if (dongle_acquire(&simu->dongles[left], coder, simu) != 0)
	{
		pthread_mutex_unlock(&simu->resources_mutex);
		return ;
	}
	coder->left_taken = 1;
	if (dongle_acquire(&simu->dongles[right], coder, simu) != 0)
	{
		dongle_release(&simu->dongles[left], coder, simu);
		coder->left_taken = 0;
		pthread_mutex_unlock(&simu->resources_mutex);
		return ;
	}
	coder->right_taken = 1;
	pthread_mutex_unlock(&simu->resources_mutex);
}

void	release_two_dongles(t_coder *coder)
{
	t_simu	*simu;

	simu = coder->simu;
	pthread_mutex_lock(&simu->resources_mutex);
	if (simu->number_of_coders == 1)
	{
		dongle_release(&simu->dongles[0], coder, simu);
		coder->left_taken = 0;
		coder->right_taken = 0;
		pthread_mutex_unlock(&simu->resources_mutex);
		return ;
	}
	if (coder->left_taken)
		dongle_release(&simu->dongles[coder->left_dongle], coder, simu);
	if (coder->right_taken)
		dongle_release(&simu->dongles[coder->right_dongle], coder, simu);
	coder->left_taken = 0;
	coder->right_taken = 0;
	pthread_mutex_unlock(&simu->resources_mutex);
}

static void	set_coder_state(t_coder *coder, int state)
{
	pthread_mutex_lock(&coder->simu->sim_mutex);
	coder->state = state;
	pthread_mutex_unlock(&coder->simu->sim_mutex);
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_simu	*simu;

	coder = (t_coder *)arg;
	simu = coder->simu;
	while (1)
	{
		pthread_mutex_lock(&simu->sim_mutex);
		if (simu->stop)
		{
			pthread_mutex_unlock(&simu->sim_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&simu->sim_mutex);
		acquire_two_dongles(coder);
		pthread_mutex_lock(&simu->sim_mutex);
		if (simu->stop)
		{
			pthread_mutex_unlock(&simu->sim_mutex);
			release_two_dongles(coder);
			return (NULL);
		}
		coder->state = STATE_COMPILE;
		coder->last_compile_start = get_timestamp_ms();
		coder->deadline = coder->last_compile_start + (long long)simu->burnout_ms;
		pthread_mutex_unlock(&simu->sim_mutex);
		logger_state(simu, coder->id, "has taken a dongle");
		logger_state(simu, coder->id, "is compiling");
		usleep(simu->compile_ms * 1000);
		pthread_mutex_lock(&simu->sim_mutex);
		if (simu->stop)
		{
			pthread_mutex_unlock(&simu->sim_mutex);
			release_two_dongles(coder);
			return (NULL);
		}
		coder->compiles++;
		pthread_mutex_unlock(&simu->sim_mutex);
		release_two_dongles(coder);
		set_coder_state(coder, STATE_DEBUG);
		logger_state(simu, coder->id, "is debugging");
		usleep(simu->debug_ms * 1000);
		set_coder_state(coder, STATE_REFACTOR);
		logger_state(simu, coder->id, "is refactoring");
		usleep(simu->refactor_ms * 1000);
		pthread_mutex_lock(&simu->sim_mutex);
		if (all_done(simu))
		{
			stop_simulation(simu, -1);
			pthread_mutex_unlock(&simu->sim_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&simu->sim_mutex);
	}
	return (NULL);
}
