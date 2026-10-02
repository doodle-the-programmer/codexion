/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_monitor.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:41 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/01 16:42:42 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Stops the simulation if any active coder has missed their burnout deadline.
int monitor_burnout(t_simulation *sim, long long now)
{
	int i;

	for (i = 0; i < sim->coder_count; i++)
		if (!is_done(sim, &sim->coders[i]) && now >= sim->coders[i].deadline_ms)
		{
			sim->coders[i].burned_out = true;
			sim->stop = true;
			log_state(sim, sim->coders[i].id, "burned out");
			return (1);
		}
	return (0);
}

// Ends the run once every coder has satisfied the requested compile target.
int monitor_finished(t_simulation *sim)
{
	int i;

	for (i = 0; i < sim->coder_count; i++)
		if (!is_done(sim, &sim->coders[i]))
			return (0);
	sim->stop = true;
	return (1);
}
