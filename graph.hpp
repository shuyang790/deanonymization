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

#define MAX_NODES 7700

#define L 2

using namespace std;

class graph {
private:
	int num_nodes;
	int num_edges;
	vector <int> nodes;
	vector < vector<int> * > edges;
	
	struct subgraph {
		int num_nodes;
		int center;
		vector<int> num_nodes_seq;
		vector<int> nodes_per_level[L];
	};
	
public:
	graph(const char * file_name);
	
	subgraph extract_subgraph(int node);
	vector<int> * extract_neighbors(int node);
	
	friend class matcher;
};

#endif /* graph_hpp */
