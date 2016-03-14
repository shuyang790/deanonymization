//
//  graph.hpp
//  deanonymization
//
//  Created by Shuyang Shi on 15/9/20.
//  Copyright © 2015年 Shuyang Shi. All rights reserved.
//

#ifndef graph_hpp
#define graph_hpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <set>
#include <queue>
#include <algorithm>

#define MAX_NODES 10010

#define L 2

using namespace std;

class graph {
public:
	int num_nodes;
	int num_edges;
	vector <int> nodes;
	vector < vector<int> * > edges;
	vector < vector<int> * > rev_edges;

	struct subgraph {
		int num_nodes;
		int center;
		vector<int> num_nodes_seq;
		vector<int> nodes_per_level[L];
	};

	vector <int> * neighbors[MAX_NODES];
	subgraph * subgraphs[MAX_NODES];

public:
	graph(const char * file_name);

	subgraph * extract_subgraph(int node);

	friend class matcher;
	friend class analyst;
};

#endif /* graph_hpp */
