/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:39 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 15:55:44 by gpatrici         ###   ########.fr       */
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
		sim->coders[i].deadline_ms = sim->start_ms + sim->burnout_ms;
		sim->coders[i].burned_out = false;
		sim->coders[i].finished = false;
		sim->coders[i].sim = sim;
	}
}

// Creates one dongle per coder using the configured scheduler policy.
void init_dongles(t_simulation *sim)
{
	int i;

	for (i = 0; i < sim->coder_count; i++)
		init_dongle(&sim->dongles[i], i, sim->scheduler);
}

// Creates one worker thread per coder for the action loop.
int launch_threads(t_simulation *sim, pthread_t *threads)
{
	int i;
	int rc;

	sim->thread_count = 0;
	for (i = 0; i < sim->coder_count; i++)
	{
		rc = pthread_create(&threads[i], NULL, coder_routine, &sim->coders[i]);
		if (rc != 0)
		{
			fprintf(stderr, "Failed to create coder thread.\n");
			sim->stop = true;
			return (0);
		}
		sim->thread_count++;
	}
	return (1);
}

// Joins worker threads and frees simulation resources after the run.
void cleanup(t_simulation *sim, pthread_t *threads)
{
	int i;

	for (i = 0; i < sim->thread_count; i++)
		pthread_join(threads[i], NULL);
	pthread_mutex_destroy(&sim->log_mutex);
	pthread_mutex_destroy(&sim->state_mutex);
	pthread_cond_destroy(&sim->stop_cond);
	for (i = 0; i < sim->coder_count; i++)
		free_dongle(&sim->dongles[i]);
	free(sim->coders);
	free(sim->dongles);
}

