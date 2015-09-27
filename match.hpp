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
#include <string.h>
#include <math.h>
#include <map>
#include <time.h>
#include <algorithm>

// MAX rounds of updating each pair of nodes
#define MAX_ROUNDS 5

// parameters for calculating similarities
#define ALPHA1 1
#define BETA1 1
#define ALPHA2 1
#define BETA2 1

// whether use multithread
#define MULTITHREAD 1

#if MULTITHREAD

#include "thpool.hpp"

// size of the thread pool
#define THREAD_POOL_SIZE 12

#endif

using namespace std;

int int_abs(int);

#if MULTITHREAD
static threadpool thpool;
#endif

class matcher {
private:
	int num_ans_pairs;
	class graph * G_a;
	class graph * G;

	typedef double all_node_pairs[MAX_NODES][MAX_NODES]; // x_a, x

	all_node_pairs sim_nodes;
	all_node_pairs sim_subgraphs;
	
	struct match_edge {
		int u, v;
		double w;
		match_edge(int _u=0, int _v=0, double _w=0): u(_u), v(_v), w(_w) {}
		bool operator < (const match_edge & b) const {
			return w > b.w;
		}
	};
	
	vector<match_edge> ans_pairs;

public:
	matcher(class graph *g_a, class graph *g);
	
	void match();
	
	double calc_sim_nodes(int u, int v); // u from G_a, v from G
	double calc_sim_subgraphs(int u, int v); // u, v are centers and from G_a, G perspectively
	
	void print(FILE *ou);
};

#if MULTITHREAD
void * calc_sim_nodes_pthread(void * args);
void * calc_sim_subgraphs_pthread(void * args);
#endif

#endif /* match_hpp */
