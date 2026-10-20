/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_support.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 15:55:05 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 15:56:19 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Shows the required command-line format after invalid arguments.
int usage_error(const char *prog_name)
{
	fprintf(stderr,
		"Usage: %s <num_coders> <time_to_burnout> <time_to_compile> "
		"<time_to_debug> <time_to_refactor> <num_compiles_required> "
		"<dongle_cooldown> <scheduler>\n", prog_name);
	return (1);
}

// Starts the monitor thread that checks burnout and completion.
int start_monitor(t_simulation *sim, pthread_t *monitor_thread)
{
	int rc;

	rc = pthread_create(monitor_thread, NULL, monitor_routine, sim);
	if (rc != 0)
	{
		fprintf(stderr, "Failed to create monitor thread.\n");
		return (1);
	}
	return (0);
}

// Runs cleanup after starting the monitor and joins that monitor before return.
int run_simulation(t_simulation *sim, pthread_t *threads)
{
	pthread_t monitor_thread;

	if (start_monitor(sim, &monitor_thread))
	{
		cleanup(sim, threads);
		return (1);
	}
	cleanup(sim, threads);
	pthread_join(monitor_thread, NULL);
	return (0);
}

// Reports an allocation failure and returns the program error status.
int allocation_error(void)
{
	fprintf(stderr, "Allocation failure.\n");
	return (1);
}

// Reports failed preparation, releases the thread array, and returns an error.
int preparation_error(pthread_t *threads)
{
	fprintf(stderr, "Invalid parameters or allocation failure.\n");
	free(threads);
	return (1);
}
