/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:24 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 15:57:06 by gpatrici         ###   ########.fr       */
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
	printf("%lld %d %s\n", now_ms() - sim->start_ms, coder_id, msg);
	pthread_mutex_unlock(&sim->log_mutex);
}

// Entry point that validates arguments, prepares the simulation, and launches it.
int main(int argc, char **argv)
{
	t_simulation sim;
	pthread_t *threads;

	if (argc != 9)
		return (usage_error(argv[0]));
	memset(&sim, 0, sizeof(sim));
	threads = malloc(sizeof(pthread_t) * (size_t)atoi(argv[1]));
	if (!threads)
		return (allocation_error());
	if (!prepare_simulation(&sim, argv, threads))
		return (preparation_error(threads));
	if (run_simulation(&sim, threads))
	{
		free(threads);
		return (1);
	}
	free(threads);
	return (0);
}
