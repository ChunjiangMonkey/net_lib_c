#include "base.h"
#include "net_io.h"
#include <stdlib.h>
#include <stdio.h>
#define ASYNCHRONOUSLY 1
#define K 0.1
#define MC_STEPS 10000
#define REC_STEPS 5000
#define REFRESH_PRE 100
#define L 100

double b = 0;
double payoff_matrix[2][2] = {{1, 0},
                              {0, 0}};

#define update_matrix(b) payoff_matrix[1][0] = b

int cooperator_num = 0, defector_num = 0;

typedef struct NodeProperties
{
    int strategy;
    int degree;
    int *nei;

} NetNode, *PNetNode;

NetNode *create_nodes(Network *net)
{
    NetNode *nodes = (NetNode *)calloc(net->node_num, sizeof(NetNode));
    for (int i = 0; i < net->node_num; i++)
    {
        nodes[i].nei = get_nei(net, i);
        nodes[i].degree = net->nodes[i].degree;
    }
    return nodes;
}

void init_nodes(NetNode *nodes, int node_num)
{
    for (int i = 0; i < node_num; i++)
    {
        nodes[i].strategy = (int)randi(2);
    }
}

double get_payoff(NetNode *nodes, int node_id)
{
    double payoff = 0;
    int local_s = nodes[node_id].strategy;
    for (int i = 0; i < nodes[node_id].degree; i++)
    {
        int nei_s = nodes[nodes[node_id].nei[i]].strategy;
        payoff += payoff_matrix[local_s][nei_s];
    }
    return payoff;
}

void update_stra(NetNode *nodes, int node_num)
{
    int player1, player2;
    double payoff1, payoff2;
    if (ASYNCHRONOUSLY)
    {
        for (int i = 0; i < node_num; i++)
        {

            player1 = randi((int)node_num);
            player2 = nodes[player1].nei[(int)randi(nodes[player1].degree)];

            payoff1 = get_payoff(nodes, player1);
            payoff2 = get_payoff(nodes, player2);

            if (nodes[player1].strategy != nodes[player2].strategy)
            {
                if (randf() < 1 / (1 + exp((payoff1 - payoff2) / K)))
                {
                    nodes[player1].strategy = nodes[player2].strategy;
                }
            }
        }
    }
    else
    {
        double *tmp_payoff = (double *)calloc(node_num, sizeof(double));
        double *tmp_strategy = (double *)calloc(node_num, sizeof(double));

        for (int i = 0; i < node_num; i++)
        {
            tmp_payoff[i] = get_payoff(nodes, i);
            tmp_strategy[i] = nodes[i].strategy;
        }
        for (int i = 0; i < node_num; i++)
        {
            player1 = randi((int)node_num);
            player2 = nodes[player1].nei[(int)randi(nodes[player1].degree)];
            if (tmp_strategy[player1] != tmp_strategy[player2])
            {
                if (randf() < 1 / (1 + exp((tmp_payoff[player1] - tmp_payoff[player2]) / K)))
                {
                    nodes[player1].strategy = tmp_strategy[player2];
                }
            }
        }
        free(tmp_payoff);
        free(tmp_strategy);
    }
}

void update_data(NetNode *nodes, int node_num)
{
    cooperator_num = defector_num = 0;
    for (int i = 0; i < node_num; i++)
    {
        if (nodes[i].strategy == 0)
            cooperator_num++;
        else
            defector_num++;
    }
}

int main()
{
    int step;
    double fc, afc = 0;
    Network *net = create_square_lattice_network(L);
    NetNode *nodes = create_nodes(net);
    int node_num = net->node_num;

    for (b = 1; b < 2; b += 0.1)
    {
        afc = 0;
        update_matrix(b);
        init_nodes(nodes, node_num);
        for (step = 0; step < MC_STEPS; step++)
        {

            update_stra(nodes, node_num);
            update_data(nodes, node_num);
            fc = (double)cooperator_num / node_num;
            if (step > MC_STEPS - REC_STEPS - 1)
            {
                afc += fc;
            }
            if (step % REFRESH_PRE == 0 && step != 0)
            {
                fflush(stdout);
                printf("\rStep:%d\tC: %lf%%", step, fc * 100);
            }
            if ((!cooperator_num) || (!defector_num))
            {
                if (step++ < MC_STEPS - REC_STEPS)
                    afc = cooperator_num ? 1 : 0;
                break;
            }
        }
        if (step > MC_STEPS - REC_STEPS)
            afc /= step + REC_STEPS - MC_STEPS;
        printf("\rb:%lf\taverage C: %f%%\n", b, afc * 100);
    }
    return 0;
}