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
#define AVERAGE_EACH_CALC 0

// ============================

// ===== parameters =====
// seed percentage (using baseline)
#define PERC_THRSD 0.05

#ifdef BTCH
// number of pairs to be processed per iteration
#define NUM_PER_ITER 10
#endif

// decay function
#define DECAY(x) (pow(1-1e-4, x))
// ======================

#ifdef DNMC
#ifdef BTCH
Error: Cannot simutaneously use two versions!
#endif
#endif

#if MULTITHREAD

#include <pthread.h>

// size of the thread pool
#define THREAD_POOL_SIZE 12

#endif

using namespace std;

int int_abs(int);

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

// ===============================
#ifdef DNMC

	// `weights` (fake array)
	map <int, double> weights[MAX_NODES];


	// heap used when generating answer pairs
	struct heap {
		struct heap_node {
			int u, v;
			heap_node(int _=0, int __=0): u(_), v(__) {}
		};

#define heap_fa(x) ((x)>>1)
#define heap_lc(x) ((x)<<1)
#define heap_rc(x) (((x)<<1)+1)
#define heap_v(x) (owner->weights[nodes[x].u][nodes[x].v])
#define heap_p(x) (heap_pos[nodes[x].u][nodes[x].v])
		class matcher *owner;
		int len;
		struct heap_node nodes[MAX_NODES*MAX_NODES];
		all_node_pairs heap_pos;

		heap(class matcher *o);
		heap(int n, int m, class matcher *o);
		heap(map <int, double> *weights, int n,  class matcher *o);
		void heap_down(int x);
		void heap_up(int x);
		void pop();
		void push(int, int);

	} * H;

#endif
// ===============================

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
