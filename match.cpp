//
//  match.cpp
//  deanonymization
//
//  Created by Shuyang Shi on 15/9/20.
//  Copyright © 2015年 Shuyang Shi. All rights reserved.
//

#include "match.hpp"

int int_abs(int x){
	return x < 0 ? -x : x;
}

matcher::matcher(class graph * g_a, class graph * g): G_a(g_a), G(g) {
	for (int i=1; i<=g_a->num_nodes; i++)
		for (int j=1; j<=g->num_nodes; j++){
			sim_nodes[i][j] = 1;
		}
}

double matcher::calc_sim_nodes(int u, int v) {
	graph::subgraph * subg_a = G_a->extract_subgraph(u);
	graph::subgraph * subg = G->extract_subgraph(v);

	double w = 0; // sum(sum_per_level(sim_nodes))
	vector <match_edge> match_edges;
	char * flag_a = new char[MAX_NODES], * flag = new char[MAX_NODES];
	memset(flag_a, 0, MAX_NODES);
	memset(flag, 0, MAX_NODES);
	for (int i=0; i<L; i++) {
		for (vector<int> :: iterator j = subg_a->nodes_per_level[i].begin(); j!=subg_a->nodes_per_level[i].end(); j++) 
			for (vector<int> :: iterator k = subg->nodes_per_level[i].begin(); k!=subg->nodes_per_level[i].end(); k++) 
				match_edges.push_back(match_edge(*j, *k, last_round[*j][*k]));
	}
	sort(match_edges.begin(), match_edges.end());
	for (vector<match_edge> :: iterator it=match_edges.begin(); it!=match_edges.end(); it++) {
		if (!flag_a[it->u] && !flag[it->v]) {
			flag_a[it->u] = 1;
			flag[it->v] = 1;
			w += it->w;
		}
	}
	delete []flag_a;
	delete []flag;
	if (u%1000==0 && v%1000 == 0)
		fprintf(stderr, "#sim_nodes(%d, %d) = %g#", u, v, w);
	return sim_nodes[u][v] = w;
}

#if MULTITHREAD
void * calc_sim_nodes_pthread(void * args) {
	int * t = ((int**)args)[0];
	((struct matcher *)(((int**)args)[1]))->calc_sim_nodes(t[0], t[1]);
	return NULL;
}

#endif

void matcher::match() {
	clock_t time_start = clock();
	int cT;

#if MULTITHREAD
	thpool = thpool_init(THREAD_POOL_SIZE);
	for (int i=1; i<=G_a->num_nodes; i++)
		G_a->extract_subgraph(i);
	for (int i=1; i<=G->num_nodes; i++)
		G->extract_subgraph(i);
#endif

	for (cT=0; ++cT < MAX_ROUNDS; ) {

		// update node similarities for every pair of nodes
		memcpy(last_round, sim_nodes, sizeof(sim_nodes));
		for (int i=1; i<=G_a->num_nodes; i++)
			for (int j=1; j<=G->num_nodes; j++){
#if MULTITHREAD
				int ** t = new int* [2];
				t[0] = new int[2];
				t[0][0] = i, t[0][1] = j;
				t[1] = (int*)this;
				thpool_add_work(thpool, (calc_sim_nodes_pthread), (void *)t);
#else
				calc_sim_nodes(i, j);
#endif
			}
#if MULTITHREAD
		thpool_wait(thpool);
#endif

		// normalization
		double max_ele_nodes = -1;
		for (int i=1; i<=G_a->num_nodes; i++)
			for (int j=1; j<=G->num_nodes; j++){
				double tmp = sim_nodes[i][j];
				max_ele_nodes = max(max_ele_nodes, tmp);
			}
		/*
	for (int i=1; i<=5; i++, putchar(10))
		for (int j=1; j<=4; j++)
			printf("\t\tsim[%d][%d]=%g\t", i, j, sim_nodes[i][j]);
			*/
		for (int i=1; i<=G_a->num_nodes; i++)
			for (int j=1; j<=G->num_nodes; j++){
				sim_nodes[i][j] /= max_ele_nodes;
			}
		fprintf(stderr, "cT = %d\n", cT);
	}
	
	fprintf(stderr, "matcher info\n\t%d rounds, %.2lf seconds.\n", 
			cT, (clock()-time_start)*1.0/CLOCKS_PER_SEC);
}

matcher::heap::heap(int n, int m, struct matcher *o) {
	owner = o;
	len = 0;
	for (int i=1; i<=n; i++)
		for (int j=1; j<=m; j++){
			heap_pos[i][j] = ++len;
			nodes[len] = heap_node(i, j);
		}
	for (int i=len/2; i>0; i--)
		heap_down(i);
}

void matcher::heap::heap_down(int x) {
	for (int ma; heap_lc(x) <= len; x = ma){
		ma = heap_rc(x) <= len
			? (heap_v(heap_lc(x)) > heap_v(heap_rc(x))
				? heap_lc(x) : heap_rc(x))
			: heap_lc(x);
		if (heap_v(x) >= heap_v(ma))
			break;
		swap(nodes[x], nodes[ma]);
		swap(heap_p(x), heap_p(ma));
	}
}

void matcher::heap::heap_up(int x){
	for (; heap_fa(x)>0 && heap_v(x) > heap_v(heap_fa(x)); ){
		swap(nodes[x], nodes[heap_fa(x)]);
		swap(heap_p(x), heap_p(heap_fa(x)));
	}
}

void matcher::heap::pop(){
	if (len <= 0)
		return;
	swap(nodes[1], nodes[len]);
	swap(heap_p(1), heap_p(len));
	len--;
	heap_down(1);
}

void matcher::gen_ans_pairs() {
	clock_t time_start = clock();
	int cU=0;

	vector<match_edge> match_edges;
	char * flag_a = new char[MAX_NODES], * flag = new char[MAX_NODES];
	memset(flag_a, 0, MAX_NODES);
	memset(flag, 0, MAX_NODES);

	memcpy(last_round, sim_nodes, sizeof(sim_nodes));
	H = new heap(G_a->num_nodes, G->num_nodes, this);
//	for (int i=1; i<=5; i++, putchar(10))
//		for (int j=1; j<=4; j++)
//			printf("\t\tsim[%d][%d]=%g\t", i, j, sim_nodes[i][j]);
//	for (int i=1; i<=H->len; i++)
//		printf("\tHeap[%d]: sim(%d, %d) = %g\n",
//				i, H->nodes[i].u, H->nodes[i].v, sim_nodes[H->nodes[i].u][H->nodes[i].v]);

	for (int u, v; H->len > 0; ) {
		vector <int> nbs_a;
		vector <int> nbs;
		
		// get heap.top and check conflicts
		u = H->nodes[1].u, v = H->nodes[1].v;
		if (flag_a[u] || flag[v]) 
			goto next_round;
		
		// add as an answer pair
		ans_pairs.push_back(match_edge(u, v, sim_nodes[u][v]));
		flag_a[u] = 1;
		flag[v] = 1;
		last_round[u][v] = sim_nodes[u][v] = 1;

		// update matrix
		nbs_a.clear(), nbs.clear();
		for (vector <int>::iterator i=G_a->rev_edges[u]->begin(); i!=G_a->rev_edges[u]->end(); i++)
			if (!flag_a[*i])
				nbs_a.push_back(*i);
		for (vector <int>::iterator j=G->rev_edges[v]->begin(); j!=G->rev_edges[v]->end(); j++)
			if (!flag[*j])
				nbs.push_back(*j);
		for (vector <int> :: iterator i=nbs_a.begin(); i!=nbs_a.end(); i++)
			for (vector <int> :: iterator j=nbs.begin(); j!=nbs.end(); j++){
				cU ++;
#if MULTITHREAD
				int ** t = new int* [2];
				t[0] = new int[2];
				t[0][0] = *i, t[0][1] = *j;
				t[1] = (int*)this;
				thpool_add_work(thpool, (calc_sim_nodes_pthread), (void *)t);
#else
				calc_sim_nodes(*i, *j);
#endif
			}
#if MULTITHREAD
		thpool_wait(thpool);
#endif

		// apply changes to matrix
		for (vector <int> :: iterator i=nbs_a.begin(); i!=nbs_a.end(); i++)
			for (vector <int> :: iterator j=nbs.begin(); j!=nbs.end(); j++)
				last_round[*i][*j] = sim_nodes[*i][*j];
next_round:
		H->pop();
	}

	delete []flag_a;
	delete []flag;
	
	fprintf(stderr, "answer pairs generated.\n\t%d updates, %.2lf seconds.\n", 
			cU, (clock()-time_start)*1.0/CLOCKS_PER_SEC);
}

void matcher::print(FILE *ou) {
	for (vector<match_edge> :: iterator it=ans_pairs.begin(); it!=ans_pairs.end(); ++it) {
#if PRINT_SIMI
		fprintf(ou, "%d %d %g\n", it->u, it->v, it->w);
#else
		fprintf(ou, "%d %d\n", it->u, it->v);
#endif

	}
}
