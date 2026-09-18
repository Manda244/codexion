#include "codexion.h"

int	scheduler_fifo_priority(const t_coder *coder, const t_coder *other)
{
	if (coder == NULL || other == NULL)
		return (0);
	return (coder->request_id < other->request_id);
}
