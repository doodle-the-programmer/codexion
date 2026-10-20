/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 16:00:48 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 16:00:48 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Returns whether this coder should stop before starting another compile.
static int coder_should_stop(t_simulation *sim, t_coder *coder)
{
	int stop;

	pthread_mutex_lock(&sim->state_mutex);
	stop = sim->stop || coder->compile_count >= sim->required_compiles;
	pthread_mutex_unlock(&sim->state_mutex);
	return (stop);
}

// Records the start and burnout deadline for the coder's next compile.
static void set_coder_deadline(t_coder *coder)
{
	pthread_mutex_lock(&coder->sim->state_mutex);
	coder->last_compile_start_ms = now_ms();
	coder->deadline_ms = coder->last_compile_start_ms
		+ coder->sim->burnout_ms;
	pthread_mutex_unlock(&coder->sim->state_mutex);
}

// Worker loop that repeatedly acquires dongles and completes compile cycles.
void *coder_routine(void *arg)
{
	t_coder *coder;
	t_simulation *sim;

	coder = (t_coder *)arg;
	sim = coder->sim;
	while (!coder_should_stop(sim, coder))
	{
		set_coder_deadline(coder);
		if (acquire_pair(coder))
		{
			run_compile_step(coder);
			pthread_mutex_lock(&sim->state_mutex);
			coder->compile_count++;
			pthread_mutex_unlock(&sim->state_mutex);
			set_finished(sim, coder);
		}
	}
	return (NULL);
}
