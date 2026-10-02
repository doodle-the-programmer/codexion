/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:33 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/01 16:42:34 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Sets up a dongle with its queue, mutex, and condition variable for locking logic.
void init_dongle(t_dongle *dongle, int id, t_scheduler policy)
{
	memset(dongle, 0, sizeof(*dongle));
	dongle->id = id;
	dongle->owner = -1;
	dongle->last_release_ms = 0;
	init_heap(&dongle->queue, policy);
	pthread_mutex_init(&dongle->mutex, NULL);
	pthread_cond_init(&dongle->cond, NULL);
}

// Destroys the synchronization primitives used by a dongle and releases its queue.
void free_dongle(t_dongle *dongle)
{
	pthread_cond_destroy(&dongle->cond);
	pthread_mutex_destroy(&dongle->mutex);
	free_heap(&dongle->queue);
}

// Tries to claim this dongle while respecting queue order, cooldowns, and waiting.
int try_acquire_dongle(t_simulation *sim, int dongle_id, int coder_id,
				long long deadline)
{
	t_dongle *dongle;
	struct timespec ts;
	long long now;

	dongle = &sim->dongles[dongle_id];
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->owner == coder_id)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (1);
	}
	if (!already_waiting(dongle, coder_id))
		queue_waiter(dongle, coder_id, deadline);
	while (dongle->owner != coder_id)
	{
		now = now_ms();
		if (can_take_dongle(dongle, coder_id, now, sim->cooldown_ms))
		{
			take_dongle_ownership(dongle, coder_id);
			pthread_mutex_unlock(&dongle->mutex);
			return (1);
		}
		wait_for_turn(&ts);
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
	}
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}

// Releases a dongle back to the pool and wakes any waiting coders.
void release_dongle(t_simulation *sim, int dongle_id, int coder_id)
{
	t_dongle *dongle;

	dongle = &sim->dongles[dongle_id];
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->owner == coder_id)
	{
		dongle->owner = -1;
		dongle->last_release_ms = now_ms();
	}
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

// Attempts to secure the left and right dongles needed for one compile cycle.
int acquire_pair(t_coder *coder)
{
	t_simulation *sim;
	int left;
	int right;

	sim = coder->sim;
	left = (coder->id - 1 + sim->coder_count) % sim->coder_count;
	right = coder->id % sim->coder_count;
	if (sim->coder_count == 1)
		return (try_acquire_dongle(sim, 0, coder->id, coder->deadline_ms));
	if (!try_acquire_dongle(sim, left, coder->id, coder->deadline_ms))
		return (0);
	if (!try_acquire_dongle(sim, right, coder->id, coder->deadline_ms))
	{
		release_dongle(sim, left, coder->id);
		return (0);
	}
	return (1);
}

