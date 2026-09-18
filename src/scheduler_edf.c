#include "codexion.h"

int	scheduler_edf_priority(const t_coder *coder, const t_coder *other)
{
	if (coder == NULL || other == NULL)
		return (0);
	if (coder->deadline < other->deadline)
		return (1);
	if (coder->deadline > other->deadline)
		return (0);
	return (coder->id < other->id);
}
