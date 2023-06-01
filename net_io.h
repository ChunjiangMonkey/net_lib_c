#ifndef _NET_IO_H_
#define _NET_IO_H_

#include "base.h"
#include <stdio.h>

Status write_to_edge_list(PNetwork p_network, char *file_path);
Status write_to_gml(PNetwork p_network, char *file_path);
Status write_to_adjacency_list(PNetwork p_network, char *file_path);
Status output_degree(PNetwork p_network, char *file_path);

#endif // !_NET_IO_H_