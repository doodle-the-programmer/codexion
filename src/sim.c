/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:42 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 16:01:57 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Marks a coder finished at the required compile count and updates stop state.
void set_finished(t_simulation *sim, t_coder *coder)
{
	pthread_mutex_lock(&sim->state_mutex);
	if (coder->compile_count >= sim->required_compiles)
	{
		coder->finished = true;
		sim->finished_coders++;
		if (sim->finished_coders >= sim->coder_count)
			sim->stop = true;
	}
	pthread_mutex_unlock(&sim->state_mutex);
}

// Returns true when a coder reaches the required compile count.
int is_done(const t_simulation *sim, const t_coder *coder)
{
	return (coder->compile_count >= sim->required_compiles);
}

// Simulates one full compile/debug/refactor cycle while a coder holds both dongles.
void run_compile_step(t_coder *coder)
{
	t_simulation *sim;
	int left;
	int right;

	sim = coder->sim;
	left = (coder->id - 1 + sim->coder_count) % sim->coder_count;
	right = coder->id % sim->coder_count;
	log_state(sim, coder->id, "has taken a dongle");
	log_state(sim, coder->id, "has taken a dongle");
	log_state(sim, coder->id, "is compiling");
	usleep((useconds_t)sim->compile_ms * 1000U);
	release_dongle(sim, left, coder->id);
	release_dongle(sim, right, coder->id);
	log_state(sim, coder->id, "is debugging");
	usleep((useconds_t)sim->debug_ms * 1000U);
	log_state(sim, coder->id, "is refactoring");
	usleep((useconds_t)sim->refactor_ms * 1000U);
}

// Monitoring loop that stops the simulation when a coder burns out or all finish.
void *monitor_routine(void *arg)
{
	t_simulation *sim;
	long long now;

	sim = (t_simulation *)arg;
	while (1)
	{
		usleep(1000);
		pthread_mutex_lock(&sim->state_mutex);
		if (sim->stop)
		{
			pthread_mutex_unlock(&sim->state_mutex);
			return (NULL);
		}
		now = now_ms();
		if (monitor_burnout(sim, now) || monitor_finished(sim))
		{
			pthread_mutex_unlock(&sim->state_mutex);
			return (NULL);
		}
		pthread_mutex_unlock(&sim->state_mutex);
	}
	return (NULL);
}
