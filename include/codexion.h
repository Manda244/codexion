#ifndef CODEXION_H
# define CODEXION_H

# include <errno.h>
# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

# define STATE_IDLE 0
# define STATE_COMPILE 1
# define STATE_DEBUG 2
# define STATE_REFACTOR 3
# define STATE_BURNOUT 4
# define SCHEDULER_FIFO 1
# define SCHEDULER_EDF 2
# define DONGLE_FREE -1

struct s_simu;

typedef struct s_heap_node
{
	int		coder_id;
	int		request_id;
	long long	deadline;
	long long	key;
} t_heap_node;

typedef struct s_heap
{
	t_heap_node	*items;
	int		size;
	int		capacity;
} t_heap;

typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int			id;
	int			owner;
	int			available;
	long long		last_release_ms;
	int			cooldown_ms;
	t_heap			requests;
} t_dongle;

typedef struct s_coder
{
	int		id;
	pthread_t	thread;
	struct s_simu	*simu;
	int		left_dongle;
	int		right_dongle;
	long long	last_compile_start;
	int		compiles;
	int		state;
	long long	deadline;
	int		request_id;
	int		left_taken;
	int		right_taken;
} t_coder;

typedef struct s_simu
{
	int			number_of_coders;
	int			burnout_ms;
	int			compile_ms;
	int			debug_ms;
	int			refactor_ms;
	int			required_compiles;
	int			cooldown_ms;
	int			scheduler;
	int			stop;
	int			stop_by_burnout;
	int			request_counter;
	long long		start_time_ms;
	pthread_mutex_t	sim_mutex;
	pthread_cond_t	sim_cond;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	scheduler_mutex;
	pthread_mutex_t	resources_mutex;
	t_dongle		*dongles;
	t_coder		*coders;
	pthread_t		monitor_thread;
} t_simu;

int		parse_arguments(int argc, char **argv, t_simu *simu);
long long	get_timestamp_ms(void);
int		logger_state(t_simu *simu, int coder_id, const char *message);
int		stop_simulation(t_simu *simu, int coder_id);
void		cleanup_simulation(t_simu *simu);
t_simu		*simu_init_from_args(int argc, char **argv);
void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);
int		heap_init(t_heap *heap, int capacity);
void		heap_destroy(t_heap *heap);
int		heap_push(t_heap *heap, t_heap_node node, int scheduler);
int		heap_pop(t_heap *heap, t_heap_node *node);
int		heap_peek(const t_heap *heap, t_heap_node *node);
int		heap_size(const t_heap *heap);
int		heap_is_empty(const t_heap *heap);
int		dongle_init(t_dongle *dongle, int id, int cooldown_ms, int coders_count);
void		dongle_destroy(t_dongle *dongle);
int		dongle_acquire(t_dongle *dongle, t_coder *coder, t_simu *simu);
void		dongle_release(t_dongle *dongle, t_coder *coder, t_simu *simu);
void		acquire_two_dongles(t_coder *coder);
void		release_two_dongles(t_coder *coder);

#endif
