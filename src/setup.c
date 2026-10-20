/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:39 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 14:06:03 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Initializes each coder record with ID and runtime state before any threads start.
void init_coders(t_simulation *sim)
{
	int i;

	for (i = 0; i < sim->coder_count; i++)
	{
		sim->coders[i].id = i + 1;
		sim->coders[i].compile_count = 0;
		sim->coders[i].last_compile_start_ms = 0;
		sim->coders[i].deadline_ms = 0;
		sim->coders[i].burned_out = false;
		sim->coders[i].finished = false;
		sim->coders[i].sim = sim;
	}
}

// Creates a dongle instance for each coder slot using the simulation's scheduler policy.
void init_dongles(t_simulation *sim)
{
	int i;

	for (i = 0; i < sim->coder_count; i++)
		init_dongle(&sim->dongles[i], i, sim->scheduler);
}

// Creates one worker thread per coder to run the action loop concurrently.
void launch_threads(t_simulation *sim, pthread_t *threads)
{
	int i;
	int rc;

	for (i = 0; i < sim->coder_count; i++)
	{
		rc = pthread_create(&threads[i], NULL, coder_routine, &sim->coders[i]);
		if (rc != 0)
		{
			fprintf(stderr, "Failed to create coder thread.\n");
			exit(1);
		}
	}
}

// Joins all worker threads and frees all simulation resources after the run ends.
void cleanup(t_simulation *sim, pthread_t *threads)
{
	int i;

	for (i = 0; i < sim->coder_count; i++)
		pthread_join(threads[i], NULL);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->state_mutex);
	pthread_cond_destroy(&sim->stop_cond);
	for (i = 0; i < sim->coder_count; i++)
		free_dongle(&sim->dongles[i]);
	free(threads);
	free(sim->coders);
	free(sim->dongles);
}

// Parses command-line input, allocates arrays, sets up locks, and starts the workers.
int prepare_simulation(t_simulation *sim, char **argv, pthread_t *threads)
{
	if (!parse_args(sim, argv))
		return (0);
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->state_mutex, NULL);
	pthread_cond_init(&sim->stop_cond, NULL);
	sim->dongles = malloc(sizeof(t_dongle) * sim->coder_count);
	sim->coders = malloc(sizeof(t_coder) * sim->coder_count);
	if (!sim->dongles || !sim->coders || !threads)
		return (0);
	init_dongles(sim);
	init_coders(sim);
	launch_threads(sim, threads);
	return (1);
}
