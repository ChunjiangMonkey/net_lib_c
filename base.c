#include "base.h"

PNetwork create_empty_network(int node_num)
{
	if (node_num == 0)
		return NULL;

	PNetwork p_network = (PNetwork)malloc(sizeof(Network));
	if (!p_network)
		return NULL;
	p_network->node_num = node_num;
	p_network->edge_num = 0;
	p_network->nodes = (PNode)calloc(node_num, sizeof(Node));

	if (!p_network->nodes)
		return NULL;
	for (int i = 0; i < node_num; i++)
	{
		p_network->nodes[i].node_id = i;
		p_network->nodes[i].degree = 0;
		p_network->nodes[i].first_edge = NULL;
		p_network->nodes[i].last_edge = NULL;
	}

	return p_network;
}

int *get_nei(PNetwork p_network, int node_id)
{
	if (!p_network)
		return ERROR;
	if (node_id >= p_network->node_num || node_id < 0)
		return ERROR;
	int i = 0;
	PEdgeNode p = p_network->nodes[node_id].first_edge;
	int *neis = (int *)calloc(p_network->nodes[node_id].degree, sizeof(int));
	for (i = 0; p; p = p->next, i++)
		neis[i] = p->node_id;
	return neis;
}

// 创建邻接矩阵

int **create_adjacency_matrix(PNetwork p_network)
{
	if (!p_network)
		return NULL;

	int i = 0, j = 0;
	int **adjacency_matrix = (int **)calloc(p_network->node_num, sizeof(int *));
	if (!adjacency_matrix)
		return NULL;
	for (i = 0; i < p_network->node_num; i++)
	{
		adjacency_matrix[i] = (int *)calloc(p_network->node_num, sizeof(int));
		if (!adjacency_matrix[i])
			return NULL;
	}
	for (i = 0; i < p_network->node_num; i++)
	{
		PEdgeNode p = p_network->nodes[i].first_edge;
		while (p)
		{
			adjacency_matrix[i][p->node_id] = 1;
			p = p->next;
		}
	}
	return adjacency_matrix;
}

Status delete_adjacency_matrix(PNetwork p_network, int **adjacency_matrix)
{
	if (!adjacency_matrix)
		return OK;
	int i = 0, j = 0;
	for (i = 0; i < p_network->node_num; i++)
	{
		if (!adjacency_matrix[i])
			continue;
		else
			free(adjacency_matrix[i]);
	}
	free(adjacency_matrix);
	adjacency_matrix = NULL;
	return OK;
}

// add edge, self connection or repeated connection are not allowed
Status add_directed_edge(PNetwork p_network, int source_node_id, int target_node_id)
{
	// printf("\n%d-%d", source_node_id, target_node_id);
	if (source_node_id == target_node_id)
	{
		return BANNED;
	}

	if (!p_network)
		return ERROR;
	if (source_node_id >= p_network->node_num || target_node_id >= p_network->node_num)
		return ERROR;

	PEdgeNode p_test = p_network->nodes[source_node_id].first_edge;
	while (p_test)
	{
		if (p_test->node_id == target_node_id)
		{
			return BANNED;
		}

		p_test = p_test->next;
	}

	PEdgeNode p_edge_node = (PEdgeNode)malloc(sizeof(EdgeNode));

	// inital the edge
	if (!p_edge_node)
		return ERROR;
	p_edge_node->node_id = target_node_id;
	p_edge_node->weight = 0;
	p_edge_node->next = NULL;
	p_edge_node->prior = NULL;

	// the first edge
	if (NULL == p_network->nodes[source_node_id].first_edge)
	{
		// the first edge has no piror
		p_network->nodes[source_node_id].first_edge = p_edge_node;
	}

	else
	{
		// add this edge to the tail, so the prior is the original edge
		p_edge_node->prior = p_network->nodes[source_node_id].last_edge;
		p_network->nodes[source_node_id].last_edge->next = p_edge_node;
	}

	// anyway, move the last_edge pointer to this edge
	p_network->nodes[source_node_id].last_edge = p_edge_node;
	p_network->nodes[source_node_id].degree++;
	p_network->edge_num++;
	return OK;
}

Status add_undirected_edge(PNetwork p_network, int source_node_id, int target_node_id)
{
	if (add_directed_edge(p_network, source_node_id, target_node_id) && add_directed_edge(p_network, target_node_id, source_node_id))
	{
		p_network->edge_num--;
		return OK;
	}

	else
		return ERROR;
}

Status remove_directed_edge(PNetwork p_network, int source_node_id, int target_node_id)
{
	if (!p_network)
		return ERROR;
	if (source_node_id == target_node_id)
		return BANNED;
	if (source_node_id >= p_network->node_num || target_node_id >= p_network->node_num)
		return ERROR;
	if (source_node_id <= 0 || target_node_id <= 0)
		return ERROR;
	// use a pointer to find edge
	PEdgeNode p = p_network->nodes[source_node_id].first_edge;
	while (p)
	{
		if (p->node_id == target_node_id)
			break;
		else
		{
			p = p->next;
		}
	}
	if (NULL == p)
		return ERROR;

	// more than one neis and the first nei
	if (p_network->nodes[source_node_id].first_edge == p && p_network->nodes[source_node_id].last_edge != p)
	{
		p_network->nodes[source_node_id].first_edge = p->next;
		p_network->nodes[source_node_id].first_edge->prior = NULL;
	}

	else if (p_network->nodes[source_node_id].first_edge != p && p_network->nodes[source_node_id].last_edge == p)
	{
		p_network->nodes[source_node_id].last_edge = p->prior;
		p_network->nodes[source_node_id].last_edge->next = NULL;
	}

	else if (p_network->nodes[source_node_id].first_edge == p && p_network->nodes[source_node_id].last_edge == p)
	{
		p_network->nodes[source_node_id].first_edge = NULL;
		p_network->nodes[source_node_id].last_edge = NULL;
	}

	else
	{
		p->prior->next = p->next;
		p->next->prior = p->prior;
	}
	free(p);
	p = NULL;
	p_network->nodes[source_node_id].degree--;
	p_network->edge_num--;
	return OK;
}

Status remove_undirected_edge(PNetwork p_network, int source_node_id, int target_node_id)
{
	if (remove_directed_edge(p_network, source_node_id, target_node_id) && remove_directed_edge(p_network, target_node_id, source_node_id))
	{
		p_network->edge_num++;
		return OK;
	}
	else
		return ERROR;
}

Status delete_network(PNetwork p_network)
{
	if (!p_network)
		return OK;
	int i = 0;
	PEdgeNode p, q;
	for (i = 0; i < p_network->node_num; i++)
	{
		for (p = p_network->nodes[i].first_edge; p; p = q)
		{
			q = p->next;
			free(p);
		}
	}
	free(p_network->nodes);
	free(p_network);
	p_network = NULL;
	return OK;
}

Status add_new_node(PNetwork p_network)
{
	if (!p_network)
		return ERROR;
	PNode tem_nodes = (PNode)realloc(p_network->nodes, sizeof(Node) * ((p_network->node_num) + 1));
	if (!tem_nodes)
		return ERROR;

	p_network->nodes = tem_nodes;
	p_network->nodes[p_network->node_num].node_id = p_network->node_num;
	p_network->nodes[p_network->node_num].degree = 0;
	p_network->nodes[p_network->node_num].first_edge = NULL;
	p_network->nodes[p_network->node_num].last_edge = NULL;
	p_network->node_num++;
	return OK;
}

Status add_new_nodes(PNetwork p_network, int new_node_num)
{
	for (int i = 0; i < new_node_num; i++)
	{
		if (!add_new_node(p_network))
			return ERROR;
	}
	return OK;
}

int *choose_node_randomly(PNetwork p_network, int chosen_num)
{
	if (chosen_num >= p_network->node_num)
		return NULL;
	int chosen_node_num = p_network->node_num - 1;
	int *chosen_node = (int *)calloc(chosen_num, sizeof(int));
	if (!chosen_node)
		return NULL;
	int *node_pool = (int *)calloc(chosen_node_num, sizeof(int));
	if (!node_pool)
		return NULL;

	int i = 0;
	for (i = 0; i < chosen_node_num; i++)
	{
		node_pool[i] = p_network->nodes[i].node_id;
	}
	if (chosen_num == chosen_node_num)
		return node_pool;
	int current_pool_len = chosen_node_num;
	int temp = 0, random_index = 0;
	for (i = 0; i < chosen_num; i++)
	{
		random_index = randi(current_pool_len);
		chosen_node[i] = node_pool[random_index];

		node_pool[random_index] += node_pool[current_pool_len - 1];
		node_pool[current_pool_len - 1] = node_pool[random_index] - node_pool[current_pool_len - 1];
		node_pool[random_index] = node_pool[random_index] - node_pool[current_pool_len - 1];

		current_pool_len--;
	}
	return chosen_node;
}

int *choose_by_degree(PNetwork p_network, int chosen_num)
{
	if (chosen_num >= p_network->node_num)
		return NULL;

	int chosen_node_num = p_network->node_num - 1;

	int *chosen_node = (int *)calloc(chosen_num, sizeof(int));
	if (!chosen_node)
		return NULL;
	double *degree = (double *)calloc(chosen_node_num, sizeof(double));
	if (!degree)
		return NULL;
	int *node_pool = (int *)calloc(chosen_node_num, sizeof(int));
	if (!node_pool)
		return NULL;
	double *cumulative_prob = (double *)calloc(chosen_node_num + 1, sizeof(double));
	if (!cumulative_prob)
		return NULL;

	int i = 0, j = 0;
	// initial the node pool
	for (i = 0; i < chosen_node_num; i++)
	{
		node_pool[i] = p_network->nodes[i].node_id;
	}
	if (chosen_num == chosen_node_num)
		return node_pool;

	for (i = 0; i < chosen_node_num; i++)
	{
		degree[i] = p_network->nodes[i].degree;
	}

	double sum_degree = 0;
	int current_len = chosen_node_num;
	double random_num = 0;
	int chosen_index = 0;

	for (i = 0; i < chosen_num; i++)
	{
		sum_degree = 0;
		for (j = 0; j < current_len; j++)
		{
			sum_degree += degree[j];
		}

		for (j = 0; j < current_len + 1; j++)
		{
			cumulative_prob[j] = 0;
		}

		for (j = 1; j < current_len + 1; j++)
		{
			cumulative_prob[j] = cumulative_prob[j - 1] + degree[j - 1] / sum_degree;
		}

		random_num = randf();

		if (random_num == 0)
			chosen_index = 0;
		else if (random_num == 1)
			chosen_index = current_len - 1;
		else
		{
			for (j = 0; j < current_len - 1; j++)
			{
				if (random_num >= cumulative_prob[j] && random_num < cumulative_prob[j + 1])
				{
					chosen_index = j;
				}
			}
		}

		chosen_node[i] = node_pool[chosen_index];

		node_pool[chosen_index] += node_pool[current_len - 1];
		node_pool[current_len - 1] = node_pool[chosen_index] - node_pool[current_len - 1];
		node_pool[chosen_index] = node_pool[chosen_index] - node_pool[current_len - 1];

		degree[chosen_index] += degree[current_len - 1];
		degree[current_len - 1] = degree[chosen_index] - degree[current_len - 1];
		degree[chosen_index] = degree[chosen_index] - degree[current_len - 1];

		current_len--;
	}
	free(degree);
	free(node_pool);
	free(cumulative_prob);
	return chosen_node;
}

Status add_new_node_connected_randomly(PNetwork p_network, int edge_num)
{
	if (!p_network)
		return ERROR;
	if (!add_new_node(p_network))
		return ERROR;

	int new_node_id = p_network->node_num - 1;
	int i = 0;
	int *connected_node_array = choose_node_randomly(p_network, edge_num);
	if (!connected_node_array)
	{
		return ERROR;
	}
	for (i = 0; i < edge_num; i++)
	{
		if (!add_undirected_edge(p_network, new_node_id, connected_node_array[i]))
			return ERROR;
	}
	free(connected_node_array);
	connected_node_array = NULL;
	return OK;
}

Status add_new_node_connected_by_degree(PNetwork p_network, int edge_num)
{
	if (!p_network)
		return ERROR;
	if (!add_new_node(p_network))
	{
		return ERROR;
	}

	int new_node_id = p_network->node_num - 1;
	int i = 0;
	int *connected_node_array = choose_by_degree(p_network, edge_num);
	if (!connected_node_array)
	{
		return ERROR;
	}
	for (i = 0; i < edge_num; i++)
	{
		if (!add_undirected_edge(p_network, new_node_id, connected_node_array[i]))
		{
			return ERROR;
		}
	}
	free(connected_node_array);
	connected_node_array = NULL;
	return OK;
}

Status add_new_nodes_connected_randomly(PNetwork p_network, int new_node_num, int edge_num)
{
	for (int i = 0; i < new_node_num; i++)
	{
		if (!add_new_node_connected_randomly(p_network, edge_num))
			return ERROR;
	}
	return OK;
}

Status add_new_nodes_connected_by_degree(PNetwork p_network, int new_node_num, int edge_num)
{
	for (int i = 0; i < new_node_num; i++)
	{
		if (!add_new_node_connected_by_degree(p_network, edge_num))
			return ERROR;
	}
	return OK;
}

PNetwork create_fully_connected_network(int node_num)
{
	PNetwork fully_connected_network = create_empty_network(node_num);
	if (!fully_connected_network)
		return NULL;
	for (int i = 0; i < node_num; i++)
	{
		for (int j = 0; j < node_num; j++)
		{
			add_undirected_edge(fully_connected_network, i, j);
		}
	}
	return fully_connected_network;
}

PNetwork create_square_lattice_network(int L)
{
	if (L <= 0)
		return NULL;
	PNetwork square_lattice_net = create_empty_network(L * L);
	if (!square_lattice_net)
		return NULL;
	int i = 0, j = 0, x = 0;
	for (i = 0; i < L; i++)
	{
		for (j = 0; j < L; j++)
		{
			x = i * L + j;

			add_undirected_edge(square_lattice_net, x, i * L + ((j - 1 + L) % L));
			add_undirected_edge(square_lattice_net, x, ((i - 1 + L) % L) * L + j);
			add_undirected_edge(square_lattice_net, x, ((i + 1) % L) * L + j);
			add_undirected_edge(square_lattice_net, x, i * L + ((j + 1) % L));
		}
	}
	return square_lattice_net;
}

PNetwork create_regular_ring_network(int node_num, int average_degree)
{
	if (node_num <= 0 || average_degree <= 0 || average_degree % 2 != 0)
		return NULL;
	if (!(node_num > average_degree && average_degree > log(node_num)))
		return NULL;
	PNetwork regular_net = create_empty_network(node_num);
	if (!regular_net)
		return NULL;
	int i = 0, j = 0;
	int num1 = average_degree / 2;
	int num2 = node_num - 1 - num1;
	for (i = 0; i < regular_net->node_num; i++)
	{
		for (j = 0; j < regular_net->node_num; j++)
		{
			if (abs(i - j) % num2 <= num1 && abs(i - j) % num2 > 0)
				add_undirected_edge(regular_net, i, j);
		}
	}
	return regular_net;
}

PNetwork create_BA_scale_free_network(int node_num, int average_degree)
{
	if (node_num <= 0 || node_num <= average_degree || average_degree <= 0)
		return NULL;

	int m0 = 0, m = 0;
	double compared_seed = (double)average_degree / 2 - floor((double)average_degree / 2);
	int up = (int)ceil((double)average_degree / 2);
	int low = (int)floor((double)average_degree / 2);
	if (randf() < compared_seed)
		m0 = up;
	else
		m0 = low;

	PNetwork BA_scale_free_net = create_fully_connected_network(m0 + 1);
	if (!BA_scale_free_net)
		return NULL;
	int unconnected_node_num = node_num - (average_degree + 1);
	int i = 0;

	for (; unconnected_node_num > 0; unconnected_node_num--)
	{
		if (randf() < compared_seed)
			m = up;
		else
			m = low;
		if (!add_new_node_connected_by_degree(BA_scale_free_net, m))
		{
			return NULL;
		}
	}
	return BA_scale_free_net;
}

PNetwork create_BA_scale_free_network_new(int node_num, int average_degree)
{
	if (node_num <= 0 || node_num <= average_degree || average_degree <= 0)
		return NULL;

	int m0 = 0, m = 0;
	double compared_seed = (double)average_degree / 2 - floor((double)average_degree / 2);
	int up = (int)ceil((double)average_degree / 2);
	int low = (int)floor((double)average_degree / 2);
	if (randf() < compared_seed)
		m0 = up;
	else
		m0 = low;

	PNetwork BA_scale_free_net = create_fully_connected_network(m0 + 1);
	if (!BA_scale_free_net)
		return NULL;
	int unconnected_node_num = node_num - (average_degree + 1);
	int i = 0;

	for (; unconnected_node_num > 0; unconnected_node_num--)
	{
		if (randf() < compared_seed)
			m = up;
		else
			m = low;
		if (!add_new_node_connected_by_degree(BA_scale_free_net, m))
		{
			return NULL;
		}
	}
	return BA_scale_free_net;
}

PNetwork create_ER_random_network(int node_num, double p)
{
	if (node_num <= 0)
		return NULL;

	PNetwork ER_random_net = create_empty_network(node_num);
	if (!ER_random_net)
		return NULL;
	int i = 0, j = 0;
	for (i = 0; i < ER_random_net->node_num; i++)
	{
		for (j = 0; j < ER_random_net->node_num; j++)
		{
			if (randf() < p)
			{
				// printf("add%d-%d\n", i, j);
				add_undirected_edge(ER_random_net, i, j);
			}
		}
	}
	return ER_random_net;
}

PNetwork create_WS_small_world_network(int node_num, int average_degree, double beta)
{
	if (node_num <= 0 || node_num <= average_degree || average_degree <= 0 || beta > 1 || beta < 0)
		return NULL;

	PNetwork WS_small_world_net = create_regular_ring_network(node_num, average_degree);
	if (!WS_small_world_net)
		return NULL;

	int i = 0, j = 0;
	int add_new_connection_successfully = 0;
	for (i = 0; i < WS_small_world_net->node_num; i++)
	{
		for (j = i + 1; j < i + average_degree / 2; j++)
		{
			if (randf() < beta)
			{
				remove_undirected_edge(WS_small_world_net, i, j);
				add_new_connection_successfully = 0;
				while (!add_new_connection_successfully)
					add_new_connection_successfully = add_undirected_edge(WS_small_world_net, i, randi(WS_small_world_net->node_num));
			}
		}
	}
	return WS_small_world_net;
}
