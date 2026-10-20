/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_acquire.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 16:00:53 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 16:00:54 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Waits until the queued coder can claim the dongle or the simulation stops.
static int claim_dongle(t_simulation *sim, t_dongle *dongle,
		int coder_id, struct timespec *ts)
{
	long long now;

	while (!sim->stop && dongle->owner != coder_id)
	{
		now = now_ms();
		if (can_take_dongle(dongle, coder_id, now, sim->cooldown_ms))
		{
			take_dongle_ownership(dongle, coder_id);
			return (1);
		}
		wait_for_turn(ts);
		pthread_cond_timedwait(&dongle->cond, &dongle->mutex, ts);
	}
	return (dongle->owner == coder_id);
}

// Queues a coder and waits for its turn while holding the dongle mutex.
int try_acquire_dongle(t_simulation *sim, int dongle_id, int coder_id,
		long long deadline)
{
	t_dongle *dongle;
	struct timespec ts;
	int acquired;

	dongle = &sim->dongles[dongle_id];
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->owner == coder_id)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return (1);
	}
	if (!already_waiting(dongle, coder_id))
		queue_waiter(dongle, coder_id, deadline);
	acquired = claim_dongle(sim, dongle, coder_id, &ts);
	pthread_mutex_unlock(&dongle->mutex);
	return (acquired);
}
