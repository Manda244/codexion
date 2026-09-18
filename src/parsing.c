#include "codexion.h"

static int	is_valid_integer(const char *str, long *value)
{
	char	*end;
	long	parsed;

	if (str == NULL || *str == '\0')
		return (0);
	errno = 0;
	parsed = strtol(str, &end, 10);
	if (errno != 0 || end == str || *end != '\0')
		return (0);
	if (parsed < INT_MIN || parsed > INT_MAX)
		return (0);
	*value = parsed;
	return (1);
}

int	parse_arguments(int argc, char **argv, t_simu *simu)
{
	long	parsed;
	int	index;

	if (argc != 9 || simu == NULL)
		return (0);
	index = 1;
	if (!is_valid_integer(argv[index], &parsed) || parsed <= 0)
		return (0);
	simu->number_of_coders = (int)parsed;
	index++;
	if (!is_valid_integer(argv[index], &parsed) || parsed < 0)
		return (0);
	simu->burnout_ms = (int)parsed;
	index++;
	if (!is_valid_integer(argv[index], &parsed) || parsed < 0)
		return (0);
	simu->compile_ms = (int)parsed;
	index++;
	if (!is_valid_integer(argv[index], &parsed) || parsed < 0)
		return (0);
	simu->debug_ms = (int)parsed;
	index++;
	if (!is_valid_integer(argv[index], &parsed) || parsed < 0)
		return (0);
	simu->refactor_ms = (int)parsed;
	index++;
	if (!is_valid_integer(argv[index], &parsed) || parsed <= 0)
		return (0);
	simu->required_compiles = (int)parsed;
	index++;
	if (!is_valid_integer(argv[index], &parsed) || parsed < 0)
		return (0);
	simu->cooldown_ms = (int)parsed;
	index++;
	if (strcmp(argv[index], "fifo") == 0)
		simu->scheduler = SCHEDULER_FIFO;
	else if (strcmp(argv[index], "edf") == 0)
		simu->scheduler = SCHEDULER_EDF;
	else
		return (0);
	return (1);
}
