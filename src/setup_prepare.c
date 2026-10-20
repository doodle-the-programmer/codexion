/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setup_prepare.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 16:01:08 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 16:01:08 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Parses input, allocates state, initializes locks, and starts workers.
int prepare_simulation(t_simulation *sim, char **argv, pthread_t *threads)
{
	if (!parse_args(sim, argv))
		return (0);
	sim->dongles = malloc(sizeof(t_dongle) * sim->coder_count);
	sim->coders = malloc(sizeof(t_coder) * sim->coder_count);
	if (!sim->dongles || !sim->coders || !threads)
	{
		free(sim->dongles);
		free(sim->coders);
		return (0);
	}
	pthread_mutex_init(&sim->log_mutex, NULL);
	pthread_mutex_init(&sim->state_mutex, NULL);
	pthread_cond_init(&sim->stop_cond, NULL);
	sim->start_ms = now_ms();
	init_dongles(sim);
	init_coders(sim);
	if (!launch_threads(sim, threads))
	{
		cleanup(sim, threads);
		return (0);
	}
	return (1);
}
