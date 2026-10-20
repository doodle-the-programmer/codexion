/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_wait.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:31 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 15:51:25 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Checks whether this coder is already queued for the dongle.
// Prevents duplicate requests from entering the priority queue.
int already_waiting(const t_dongle *dongle, int coder_id)
{
	size_t i;

	for (i = 0; i < dongle->queue.size; ++i)
		if (dongle->queue.items[i].coder_id == coder_id)
			return (1);
	return (0);
}

// Returns true when the dongle is available and this coder is next in the queue.
int can_take_dongle(t_dongle *dongle, int coder_id, long long now, int cooldown)
{
	t_request top;

	if (dongle->owner != -1)
		return (0);
	if (now < dongle->last_release_ms + cooldown)
		return (0);
	if (dongle->queue.size == 0)
		return (0);
	top = heap_top(&dongle->queue);
	return (top.coder_id == coder_id);
}

// Appends a waiting request to the priority queue with the current timestamp.
void queue_waiter(t_dongle *dongle, int coder_id, long long deadline)
{
	t_request req;

	req.coder_id = coder_id;
	req.arrival_ms = now_ms();
	req.deadline_ms = deadline;
	heap_push(&dongle->queue, req);
}

// Advances the timing window by one millisecond for the next retry.
void wait_for_turn(struct timespec *ts)
{
	clock_gettime(CLOCK_REALTIME, ts);
	ts->tv_nsec += 1000000L;
	if (ts->tv_nsec >= 1000000000L)
	{
		ts->tv_sec += 1;
		ts->tv_nsec -= 1000000000L;
	}
}

// Transfers ownership to the waiting coder and removes the request from the queue.
void take_dongle_ownership(t_dongle *dongle, int coder_id)
{
	dongle->owner = coder_id;
	heap_pop_top(&dongle->queue);
}
