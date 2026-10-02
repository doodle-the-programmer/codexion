/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:24 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/01 16:42:25 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Returns the current monotonic time in milliseconds for deadlines and logging.
long long now_ms(void)
{
	struct timespec ts;

	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
		return (0);
	return ((long long)ts.tv_sec * 1000LL + (long long)ts.tv_nsec / 1000000LL);
}

// Prints a timestamped status line for a coder under the shared log mutex.
void log_state(t_simulation *sim, int coder_id, const char *msg)
{
	pthread_mutex_lock(&sim->log_mutex);
	printf("%lld %d %s\n", now_ms(), coder_id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}

// Shows the exact CLI format required by the project when arguments are wrong.
static int usage_error(const char *prog_name)
{
	fprintf(stderr,
		"Usage: %s <num_coders> <time_to_burnout> <time_to_compile> "
		"<time_to_debug> <time_to_refactor> <num_compiles_required> "
		"<dongle_cooldown> <scheduler>\n", prog_name);
	return (1);
}

// Starts the monitor thread that enforces burnout and completion checks.
static int start_monitor(t_simulation *sim, pthread_t *monitor_thread)
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

// Entry point that validates arguments, prepares the simulation, and launches it.
int main(int argc, char **argv)
{
	t_simulation sim;
	pthread_t *threads;
	pthread_t monitor_thread;

	if (argc != 9)
		return (usage_error(argv[0]));
	memset(&sim, 0, sizeof(sim));
	threads = malloc(sizeof(pthread_t) * (size_t)atoi(argv[1]));
	if (!threads)
	{
		fprintf(stderr, "Allocation failure.\n");
		return (1);
	}
	if (!prepare_simulation(&sim, argv, threads))
	{
		fprintf(stderr, "Invalid parameters or allocation failure.\n");
		free(threads);
		return (1);
	}
	if (start_monitor(&sim, &monitor_thread))
		return (1);
	cleanup(&sim, threads);
	pthread_join(monitor_thread, NULL);
	return (0);
}
