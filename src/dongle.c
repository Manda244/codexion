#include "codexion.h"

static int	dongle_can_grant(const t_dongle *dongle, const t_coder *coder,
		const t_simu *simu)
{
	long long	current;
	t_heap_node	node;

	if (dongle == NULL || coder == NULL || simu == NULL)
		return (0);
	current = get_timestamp_ms();
	if (!dongle->available || dongle->owner != DONGLE_FREE)
		return (0);
	if (current < dongle->last_release_ms + (long long)simu->cooldown_ms)
		return (0);
	if (heap_is_empty(&dongle->requests))
		return (0);
	if (heap_peek(&dongle->requests, &node) != 0)
		return (0);
	return (node.coder_id == coder->id);
}

static int	queue_request(t_dongle *dongle, t_coder *coder, t_simu *simu)
{
	t_heap_node	node;

	node.coder_id = coder->id;
	pthread_mutex_lock(&simu->scheduler_mutex);
	node.request_id = simu->request_counter;
	simu->request_counter++;
	pthread_mutex_unlock(&simu->scheduler_mutex);
	node.deadline = coder->last_compile_start + (long long)simu->burnout_ms;
	node.key = (long long)node.request_id;
	if (simu->scheduler == SCHEDULER_EDF)
		node.key = node.deadline;
	coder->request_id = node.request_id;
	return (heap_push(&dongle->requests, node, simu->scheduler));
}

int	dongle_init(t_dongle *dongle, int id, int cooldown_ms, int coders_count)
{
	if (dongle == NULL)
		return (1);
	if (pthread_mutex_init(&dongle->mutex, NULL) != 0)
		return (1);
	if (pthread_cond_init(&dongle->cond, NULL) != 0)
		return (1);
	dongle->id = id;
	dongle->owner = DONGLE_FREE;
	dongle->available = 1;
	dongle->last_release_ms = 0;
	dongle->cooldown_ms = cooldown_ms;
	if (heap_init(&dongle->requests, coders_count + 2) != 0)
		return (1);
	return (0);
}

void	dongle_destroy(t_dongle *dongle)
{
	if (dongle == NULL)
		return ;
	pthread_mutex_destroy(&dongle->mutex);
	pthread_cond_destroy(&dongle->cond);
	heap_destroy(&dongle->requests);
}

int	dongle_acquire(t_dongle *dongle, t_coder *coder, t_simu *simu)
{
	t_heap_node	node;
	int		result;

	if (dongle == NULL || coder == NULL || simu == NULL)
		return (1);
	pthread_mutex_lock(&dongle->mutex);
	result = queue_request(dongle, coder, simu);
	if (result != 0)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (1);
	}
	while (!simu->stop)
	{
		if (dongle_can_grant(dongle, coder, simu))
		{
			if (heap_peek(&dongle->requests, &node) == 0 && node.coder_id == coder->id)
			{
				heap_pop(&dongle->requests, &node);
				dongle->owner = coder->id;
				dongle->available = 0;
				pthread_mutex_unlock(&dongle->mutex);
				return (0);
			}
		}
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}

void	dongle_release(t_dongle *dongle, t_coder *coder, t_simu *simu)
{
	long long	now;

	if (dongle == NULL || coder == NULL || simu == NULL)
		return ;
	pthread_mutex_lock(&dongle->mutex);
	now = get_timestamp_ms();
	dongle->owner = DONGLE_FREE;
	dongle->available = 1;
	dongle->last_release_ms = now;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
	(void)coder;
	(void)simu;
}
