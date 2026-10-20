/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:29 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/12 19:09:55 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <unistd.h>

typedef enum e_scheduler
{
	FIFO = 0,
	EDF = 1
} t_scheduler;

typedef struct s_request
{
	int coder_id;
	long long arrival_ms;
	long long deadline_ms;
} t_request;

typedef struct s_heap
{
	t_request *items;
	size_t size;
	size_t capacity;
	t_scheduler policy;
} t_heap;

typedef struct s_dongle
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int id;
	int owner;
	long long last_release_ms;
	t_heap queue;
} t_dongle;

typedef struct s_simulation t_simulation;

typedef struct s_coder
{
	pthread_t thread;
	int id;
	int compile_count;
	long long last_compile_start_ms;
	long long deadline_ms;
	bool burned_out;
	bool finished;
	t_simulation *sim;
} t_coder;

struct s_simulation
{
	int coder_count;
	long long burnout_ms;
	int compile_ms;
	int debug_ms;
	int refactor_ms;
	int required_compiles;
	int cooldown_ms;
	t_scheduler scheduler;
	pthread_mutex_t log_mutex;
	pthread_mutex_t state_mutex;
	pthread_cond_t stop_cond;
	bool stop;
	int finished_coders;
	t_dongle *dongles;
	t_coder *coders;
};

long long now_ms(void);
void log_state(t_simulation *sim, int coder_id, const char *msg);
int scheduler_from_string(const char *str);
int parse_ll_arg(const char *str, long long *value);
int parse_i_arg(const char *str, int *value);
int valid_values(const t_simulation *sim);
int parse_args(t_simulation *sim, char **argv);
void heap_reserve(t_heap *heap);
int heap_lower(const t_request *a, const t_request *b, t_scheduler policy);
void heap_swap(t_request *a, t_request *b);
void heap_sift_up(t_heap *heap, size_t index);
void heap_sift_down(t_heap *heap, size_t index);
void init_heap(t_heap *heap, t_scheduler policy);
void free_heap(t_heap *heap);
void heap_push(t_heap *heap, t_request req);
void heap_pop_top(t_heap *heap);
t_request heap_top(const t_heap *heap);
int already_waiting(const t_dongle *dongle, int coder_id);
int can_take_dongle(t_dongle *dongle, int coder_id, long long now,
				int cooldown);
void queue_waiter(t_dongle *dongle, int coder_id, long long deadline);
void wait_for_turn(struct timespec *ts);
void take_dongle_ownership(t_dongle *dongle, int coder_id);
void init_dongle(t_dongle *dongle, int id, t_scheduler policy);
void free_dongle(t_dongle *dongle);
int try_acquire_dongle(t_simulation *sim, int dongle_id, int coder_id,
				long long deadline);
void release_dongle(t_simulation *sim, int dongle_id, int coder_id);
int acquire_pair(t_coder *coder);
void set_finished(t_simulation *sim, t_coder *coder);
int is_done(const t_simulation *sim, const t_coder *coder);
void run_compile_step(t_coder *coder);
void *coder_routine(void *arg);
int monitor_burnout(t_simulation *sim, long long now);
int monitor_finished(t_simulation *sim);
void *monitor_routine(void *arg);
void init_coders(t_simulation *sim);
void init_dongles(t_simulation *sim);
void launch_threads(t_simulation *sim, pthread_t *threads);
void cleanup(t_simulation *sim, pthread_t *threads);
int prepare_simulation(t_simulation *sim, char **argv, pthread_t *threads);

#endif