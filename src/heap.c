#include "codexion.h"

static int	compare_nodes(const t_heap_node *left, const t_heap_node *right)
{
	if (left->key < right->key)
		return (1);
	if (left->key > right->key)
		return (0);
	if (left->request_id < right->request_id)
		return (1);
	if (left->request_id > right->request_id)
		return (0);
	if (left->coder_id < right->coder_id)
		return (1);
	return (0);
}

static void	heap_swap(t_heap_node *first, t_heap_node *second)
{
	t_heap_node	mp;

	mp = *first;
	*first = *second;
	*second = mp;
}

static void	heap_up(t_heap *heap, int index)
{
	int	parent;

	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!compare_nodes(&heap->items[parent], &heap->items[index]))
			break ;
		heap_swap(&heap->items[parent], &heap->items[index]);
		index = parent;
	}
}

static void	heap_down(t_heap *heap, int index)
{
	int	left;
	int	right;
	int	smallest;

	while (1)
	{
		left = index * 2 + 1;
		right = left + 1;
		smallest = index;
		if (left < heap->size && compare_nodes(&heap->items[left],
				&heap->items[smallest]))
			smallest = left;
		if (right < heap->size && compare_nodes(&heap->items[right],
				&heap->items[smallest]))
			smallest = right;
		if (smallest == index)
			break ;
		heap_swap(&heap->items[index], &heap->items[smallest]);
		index = smallest;
	}
}

int	heap_init(t_heap *heap, int capacity)
{
	if (heap == NULL || capacity <= 0)
		return (1);
	heap->items = malloc(sizeof(*heap->items) * (size_t)capacity);
	if (heap->items == NULL)
		return (1);
	heap->capacity = capacity;
	heap->size = 0;
	return (0);
}

void	heap_destroy(t_heap *heap)
{
	if (heap == NULL)
		return ;
	free(heap->items);
	heap->items = NULL;
	heap->size = 0;
	heap->capacity = 0;
}

int	heap_push(t_heap *heap, t_heap_node node, int scheduler)
{
	int	index;

	if (heap == NULL || heap->items == NULL || heap->size >= heap->capacity)
		return (1);
	if (scheduler == SCHEDULER_EDF)
		node.key = node.deadline;
	else
		node.key = (long long)node.request_id;
	index = heap->size;
	heap->items[index] = node;
	heap->size++;
	heap_up(heap, index);
	return (0);
}

int	heap_pop(t_heap *heap, t_heap_node *node)
{
	if (heap == NULL || node == NULL || heap->size <= 0)
		return (1);
	*node = heap->items[0];
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		heap_down(heap, 0);
	}
	return (0);
}

int	heap_peek(const t_heap *heap, t_heap_node *node)
{
	if (heap == NULL || node == NULL || heap->size <= 0)
		return (1);
	*node = heap->items[0];
	return (0);
}

int	heap_size(const t_heap *heap)
{
	if (heap == NULL)
		return (0);
	return (heap->size);
}

int	heap_is_empty(const t_heap *heap)
{
	if (heap == NULL)
		return (1);
	return (heap->size == 0);
}
