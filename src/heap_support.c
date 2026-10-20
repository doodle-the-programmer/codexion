/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_support.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:34 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 15:52:06 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Expands the heap storage when it reaches capacity so new requests can be queued.
void heap_reserve(t_heap *heap)
{
	t_request *new_items;
	size_t cap;
	size_t i;

	cap = heap->capacity == 0 ? 8 : heap->capacity * 2;
	new_items = malloc(cap * sizeof(*new_items));
	if (!new_items)
		return ;
	if (heap->items)
	{
		i = 0;
		while (i < heap->size)
		{
			new_items[i] = heap->items[i];
			i++;
		}
		free(heap->items);
	}
	heap->items = new_items;
	heap->capacity = cap;
}

// Compares two requests using FIFO or EDF ordering depending on the active policy.
int heap_lower(const t_request *a, const t_request *b, t_scheduler policy)
{
	if (policy == FIFO)
		return (a->arrival_ms < b->arrival_ms
			|| (a->arrival_ms == b->arrival_ms && a->coder_id < b->coder_id));
	return (a->deadline_ms < b->deadline_ms
		|| (a->deadline_ms == b->deadline_ms
			&& (a->arrival_ms < b->arrival_ms
				|| (a->arrival_ms == b->arrival_ms && a->coder_id < b->coder_id))));
}

// Swaps two request records in place to restore the heap ordering.
void heap_swap(t_request *a, t_request *b)
{
	t_request tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

// Moves a newly inserted entry upward until the heap invariant is satisfied.
void heap_sift_up(t_heap *heap, size_t index)
{
	size_t parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!heap_lower(&heap->items[parent], &heap->items[index], heap->policy))
		{
			heap_swap(&heap->items[parent], &heap->items[index]);
			index = parent;
		}
		else
			break ;
	}
}

// Pulls a lower-priority node downward so the heap remains balanced and ordered.
void heap_sift_down(t_heap *heap, size_t index)
{
	size_t left;
	size_t right;
	size_t chosen;
	t_request tmp;

	while (1)
	{
		left = index * 2 + 1;
		right = index * 2 + 2;
		chosen = index;
		if (left < heap->size && heap_lower(&heap->items[left],
				&heap->items[chosen], heap->policy))
			chosen = left;
		if (right < heap->size && heap_lower(&heap->items[right],
				&heap->items[chosen], heap->policy))
			chosen = right;
		if (chosen == index)
			break ;
		tmp = heap->items[index];
		heap->items[index] = heap->items[chosen];
		heap->items[chosen] = tmp;
		index = chosen;
	}
}
