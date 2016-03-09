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
#include <time.h>
#include <math.h>
#include <map>
#include <time.h>
#include <algorithm>
#include <queue>

// ===== running settings =====

//#define DNMC // Dynamically maintain `weights` each time
#define BTCH // Process a batch of pairs after a re-establishment of `weights`

// MAX rounds of updating each pair of nodes
#define MAX_ROUNDS 5

// whether use multithread
#define MULTITHREAD 1

// whether to print the similarity values
#define PRINT_SIMI 0

#define USE_ONLY_NEIGHBORS

#define ROLE_SIMI

// ============================

// ===== parameters =====
// seed percentage (using baseline)
#define PERC_THRSD 0.05

#define BETA 0.15

// ======================

#if MULTITHREAD

#include <pthread.h>

// size of the thread pool
#define THREAD_POOL_SIZE 3

#endif

using namespace std;


#if MULTITHREAD
static pthread_t threads[THREAD_POOL_SIZE];
static int args[THREAD_POOL_SIZE];
static class matcher * MTCR;
#endif

// match_edge structure
struct match_edge {
	int u, v;
	double w;
	match_edge(int _u=0, int _v=0, double _w=0): u(_u), v(_v), w(_w) {}
	bool operator < (const match_edge & b) const {
		return w > b.w;
	}
};

class matcher {
private:

	// graphs given
	class graph * G_a;
	class graph * G;

	typedef double all_node_pairs[MAX_NODES][MAX_NODES]; // x_a, x

	// matrix for sim_nodes
	all_node_pairs sim_nodes;
	all_node_pairs last_round;

	// sorted sim_nodes
	struct node_pair {
		int u, v;
		all_node_pairs * sims;
		node_pair(int U=0, int V=0, all_node_pairs *s=0):
			u(U), v(V), sims(s) {}
		bool operator < (const struct node_pair &b) const {
			return sims[u][v] > sims[b.u][b.v];
		}
	} sim_pairs[MAX_NODES * MAX_NODES];

	// answer sequence
	vector<match_edge> ans_pairs;

	// `weights`
	all_node_pairs weights;

	// priority queues to maintain `weights`
	priority_queue < pair<double, int> > tops[MAX_NODES];

public:

	int num_nodes_G_a() const;
	int num_nodes_G() const;

	// initialize
	matcher(class graph *g_a, class graph *g);

	// calculate sim_nodes
	double calc_sim_nodes(int u, int v, int level); // u from G_a, v from G
	void calc_sim_nodes_singleth(int u, int v, bool flag=0);

	// match and generate answer pairs
	void gen_sim_matrix_simranc();
	void gen_ans_pairs();
	void gen_ans_pairs_oldway();

	// print answer pairs
	void print(FILE *ou);

	// debug
	void debug_print();
	void record_matrix(char * filename = NULL);
	void load_matrix(char * filename = NULL);

	friend class analyst;
};

#if MULTITHREAD
void * calc_sim_nodes_pthread(void * args);
#endif

#endif /* match_hpp */
