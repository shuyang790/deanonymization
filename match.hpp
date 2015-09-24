//
//  match.hpp
//  deanonymization
//
//  Created by Shuyang Shi on 15/9/20.
//  Copyright © 2015年 Shuyang Shi. All rights reserved.
//

#ifndef match_hpp
#define match_hpp

#include "graph.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <map>
#include <time.h>
#include <algorithm>

#define ALPHA1 1
#define BETA1 1
#define ALPHA2 1
#define BETA2 1

using namespace std;

const double eps = 1;

int abs(int);

class matcher {
private:
	int num_ans_pairs;
	class graph & G_a;
	class graph & G;
	map< pair<int, int>, int> sim_nodes; // x_a, x, sim
	map< pair<int, int>, int> sim_nodes_last;
	map< pair<int, int>, int> sim_subgraphs; // x_a, x, sim(subgraph(x_a), subgraph(x))
	
	struct match_edge {
		int u, v, w;
		match_edge(int _u=0, int _v=0, int _w=0): u(_u), v(_v), w(_w) {}
		bool operator < (const match_edge & b) const {
			return w > b.w;
		}
	};
	
	vector<match_edge> ans_pairs;
	
public:
	matcher(class graph &g_a, class graph &g, int _num_ans_pairs = 1000);
	
	int calc_sim_nodes(int u, int v); // u from G_a, v from G
	int calc_sim_subgraphs(int u, int v); // u, v are centers and from G_a, G perspectively
	
	void match();
	
	void print(FILE *ou);
};

#endif /* match_hpp */
