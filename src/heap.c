/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gpatrici <gpatrici@student.42lisboa.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 16:42:35 by gpatrici          #+#    #+#             */
/*   Updated: 2026/10/20 17:36:36 by gpatrici         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

// Initializes an empty heap and stores the scheduling policy it should respect.
void init_heap(t_heap *heap, t_scheduler policy)
{
	heap->items = NULL;
	heap->size = 0;
	heap->capacity = 0;
	heap->policy = policy;
}

// Releases all heap storage and resets bookkeeping to a clean empty state.
void free_heap(t_heap *heap)
{
	free(heap->items);
	heap->items = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

// Inserts a request and reorders the heap so the next item has the highest priority.
void heap_push(t_heap *heap, t_request req)
{
	size_t index;

	if (heap->size == heap->capacity)
		heap_reserve(heap);
	index = heap->size;
	heap->items[index] = req;
	heap->size++;
	heap_sift_up(heap, index);
}

// Removes the front item, fills the hole from the end, and restores heap order.
void heap_pop_top(t_heap *heap)
{
	t_request last;

	if (heap->size == 0)
		return ;
	if (heap->size == 1)
	{
		heap->size = 0;
		return ;
	}
	last = heap->items[heap->size - 1];
	heap->items[0] = last;
	heap->size--;
	heap_sift_down(heap, 0);
}

// Returns the current highest-priority request or a sentinel when the heap is empty.
t_request heap_top(const t_heap *heap)
{
	t_request empty;

	empty.coder_id = -1;
	empty.arrival_ms = 0;
	empty.deadline_ms = 0;
	if (heap->size == 0)
		return (empty);
	return (heap->items[0]);
}

