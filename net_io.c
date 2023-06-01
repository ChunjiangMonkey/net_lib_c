#include "net_io.h"

Status write_to_edge_list(PNetwork p_network, char *file_path)
{
    FILE *fp;
    if ((fp = fopen(file_path, "wt+")) == NULL)
    {
        return ERROR;
    }
    int i = 0, j = 0;
    int node_id = 0;
    for (; i < p_network->node_num; i++)
    {
        int *nei = get_nei(p_network, i);
        for (j = 0; j < p_network->nodes[i].degree; j++)
        {
            fprintf(fp, "%d,%d\n", i, nei[j]);
        }
    }
    fclose(fp);
    return OK;
}

Status output_degree(PNetwork p_network, char *file_path)
{
    FILE *fp;
    if ((fp = fopen(file_path, "wt+")) == NULL)
    {
        return ERROR;
    }
    int i = 0;
    int node_id = 0;
    for (; i < p_network->node_num; i++)
    {
        fprintf(fp, "%d\n", p_network->nodes[i].degree);
    }
    fclose(fp);
    return OK;
}
