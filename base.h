// #define NULL ((void*)0)
//  author: MCJ
#ifndef _BASE_H_
#define _BASE_H_

#include "random_fun.h"
#include <math.h>
#include <stdlib.h>
#define OK 1
#define ERROR 0
#define BANNED 0
// #define NULL 0

typedef double WeightType;
typedef int Status;

typedef struct EdgeNode
{
    int node_id;
    WeightType weight;
    struct EdgeNode *next;
    struct EdgeNode *prior;

} EdgeNode, *PEdgeNode;

typedef struct Node
{
    int node_id;
    int degree;
    PEdgeNode first_edge;
    PEdgeNode last_edge;

} Node, *PNode;

typedef struct Network
{
    int node_num;
    int edge_num;
    PNode nodes;

} Network, *PNetwork;


// return an adjacency matrix for a given network
int **create_adjacency_matrix(PNetwork p_network);

int *get_nei(PNetwork p_network, int node_id);

Status delete_adjacency_matrix(PNetwork p_network, int **adjacency_matrix);

Status add_directed_edge(PNetwork p_network, int source_node_id, int target_node_id);

Status add_undirected_edge(PNetwork p_network, int source_node_id, int target_node_id);

Status remove_directed_edge(PNetwork p_network, int source_node_id, int target_node_id);

Status remove_undirected_edge(PNetwork p_network, int source_node_id, int target_node_id);

Status delete_network(PNetwork p_network);

Status add_new_node(PNetwork p_network);

Status add_new_node_connected_randomly(PNetwork p_network, int edge_num);

Status add_new_node_connected_by_degree(PNetwork p_network, int edge_num);

int *choose_node_randomly(PNetwork p_network, int chosen_num);

int *choose_by_degree(PNetwork p_network, int chosen_num);

// create a new network without edges
PNetwork create_empty_network(int node_num);

// create a new fully connected network
PNetwork create_fully_connected_network(int node_num);

PNetwork create_square_lattice_network(int L);

PNetwork create_regular_ring_network(int node_num, int average_degree);

// create a BA model Scale-free network
PNetwork create_BA_scale_free_network(int node_num, int average_degree);

// use iteration
PNetwork create_BA_scale_free_network_new(int node_num, int average_degree);

PNetwork create_ER_random_network(int node_num, double p);

PNetwork create_WS_small_world_network(int node_num, int average_degree, double beta);

#endif // !_BASE_H_