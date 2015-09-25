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

matcher::matcher(class graph &g_a, class graph &g, int _num_ans_pairs): G_a(g_a), G(g), num_ans_pairs(_num_ans_pairs){
	for (vector<int> ::iterator i=g_a.nodes.begin(); i!=g_a.nodes.end(); i++) {
		for (vector<int> ::iterator j=g.nodes.begin(); j!=g.nodes.end(); j++) {
			sim_nodes[*i][*j] = 1;
			sim_subgraphs[*i][*j] = 1;
		}
	}
}

int matcher::calc_sim_nodes(int u, int v) {
	int w1 = 0;
	vector<int> * nb_a = G_a.extract_neighbors(u);
	vector<int> * nb = G.extract_neighbors(v);
	
	vector <match_edge> match_edges;
	char * flag_a = new char[MAX_NODES], * flag = new char[MAX_NODES];
	memset(flag_a, 0, MAX_NODES);
	memset(flag, 0, MAX_NODES);
	for (vector<int> :: iterator i = nb_a->begin(); i!=nb_a->end(); i++)
		for (vector<int> :: iterator j = nb->begin(); j!=nb->end(); j++)
			match_edges.push_back(match_edge(*i, *j, sim_nodes[*i][*j]));
	sort(match_edges.begin(), match_edges.end());
	for (vector<match_edge> :: iterator i = match_edges.begin(); i!=match_edges.end(); i++) {
		if (!flag_a[i->u] && !flag[i->v]) {
			flag_a[i->u] = 1;
			flag[i->v] = 1;
			w1 += i->w;
		}
	}
	delete []flag_a;
	delete []flag;
	int w2 = sim_subgraphs[u][v];
	return  sim_nodes[u][v] = ALPHA1 * w1 + BETA1 * w2;
}

int matcher::calc_sim_subgraphs(int u, int v) {
	graph::subgraph subg_a = G_a.extract_subgraph(u);
	graph::subgraph subg = G.extract_subgraph(v);
	
	int w1 = 0; // node sequence
	for (int i=0; i<L; i++) {
		w1 += - int_abs(subg_a.num_nodes_seq[i] - subg.num_nodes_seq[i]);
	}
	
	int w2 = 0; // sum(level * sim_nodes)
	vector <match_edge> match_edges;
	char * flag_a = new char[MAX_NODES], * flag = new char[MAX_NODES];
	memset(flag_a, 0, MAX_NODES);
	memset(flag, 0, MAX_NODES);
	for (int i=0; i<L; i++) {
		for (vector<int> :: iterator j = subg_a.nodes_per_level[0].begin(); j!=subg_a.nodes_per_level[0].end(); j++) {
			for (vector<int> :: iterator k = subg.nodes_per_level[1].begin(); k!=subg.nodes_per_level[1].end(); k++) {
				match_edges.push_back(match_edge(*j, *k, sim_nodes[*j][*k]));
			}
		}
	}
	for (vector<match_edge> :: iterator it=match_edges.begin(); it!=match_edges.end(); it++) {
		if (!flag_a[it->u] && !flag[it->v]) {
			flag_a[it->u] = 1;
			flag[it->v] = 1;
			w2 += it->w;
		}
	}
	delete []flag_a;
	delete []flag;
	return sim_subgraphs[u][v] = ALPHA2 * w1 + BETA2 * w2;
}

void * calc_sim_nodes_pthread(void * args) {
	int * t = ((int**)args)[0];
	((struct matcher *)(((int**)args)[1]))->calc_sim_nodes(t[0], t[1]);
	return NULL;
}

void  * calc_sim_subgraphs_pthread(void * args) {
	int * t = ((int**)args)[0];
	((struct matcher *)(((int**)args)[1]))->calc_sim_subgraphs(t[0], t[1]);
	return NULL;
}

void matcher::match() {
	clock_t time_start = clock();
	int cT;
	double sum=G_a.num_nodes*G.num_nodes, sum_last;
	
	thpool = thpool_init(THREAD_POOL_SIZE);
	
	for (cT=0; ++cT; ) {
		// copy to sim_nodes_last
		memcpy(sim_nodes_last, sim_nodes, sizeof(sim_nodes));
		sum_last = sum;

		// update similarities for every pair of nodes
		for (int i=1; i<=G_a.num_nodes; i++)
			for (int j=1; j<=G.num_nodes; j++){
				int ** t = new int* [2];
				t[0] = new int[2];
				t[0][0] = i, t[0][1] = j;
				t[1] = (int*)this;
				thpool_add_work(thpool, (calc_sim_nodes_pthread), (void *)t);
			}
		thpool_wait(thpool);

		for (int i=1; i<=G_a.num_nodes; i++)
			for (int j=1; j<=G.num_nodes; j++){
				int ** t = new int* [2];
				t[0] = new int[2];
				t[0][0] = i, t[0][1] = j;
				t[1] = (int*)this;
				thpool_add_work(thpool, calc_sim_subgraphs_pthread, (void *)t);
			}
		thpool_wait(thpool);

		sum = 0;
		for (vector<int> ::iterator i=G_a.nodes.begin(); i!=G_a.nodes.end(); i++) {
			for (vector<int> ::iterator j=G.nodes.begin(); j!=G.nodes.end(); j++) {
				sum += int_abs(sim_nodes[*i][*j]);
			}
		}
		// check if converge(normalized)
		// TODO: normalization
		for (vector<int> ::iterator i=G_a.nodes.begin(); i!=G_a.nodes.end(); i++) {
			for (vector<int> ::iterator j=G.nodes.begin(); j!=G.nodes.end(); j++) {
				if (fabs(sim_nodes_last[*i][*j]/sum_last - sim_nodes[*i][*j]/sum) > eps) {
					goto next_round;
				}
			}
		}
		break;
	next_round:;
		printf("cT = %d\n", cT);
	}
	
	vector<match_edge> match_edges;
	char * flag_a = new char[MAX_NODES], * flag = new char[MAX_NODES];
	memset(flag_a, 0, MAX_NODES);
	memset(flag, 0, MAX_NODES);
	for (vector<int> ::iterator i=G_a.nodes.begin(); i!=G_a.nodes.end(); i++) {
		for (vector<int> ::iterator j=G.nodes.begin(); j!=G.nodes.end(); j++) {
			match_edges.push_back(match_edge(*i, *j, sim_nodes[*i][*j]));
		}
	}
	sort(match_edges.begin(), match_edges.end());
	for (vector<match_edge>::iterator it=match_edges.begin(); it!=match_edges.end(); ++it) {
		if (!flag_a[it->u] && !flag[it->v]) {
			flag_a[it->u] = 1;
			flag[it->v] = 1;
			ans_pairs.push_back(*it);
		}
	}
	delete []flag_a;
	delete []flag;
	
	fprintf(stderr, "matcher info\n\t%d rounds, %.2lf seconds.\n", cT, (clock()-time_start)*1.0/CLOCKS_PER_SEC);
}

void matcher::print(FILE *ou) {
	for (vector<match_edge> :: iterator it=ans_pairs.begin(); it!=ans_pairs.end(); ++it) {
		fprintf(ou, "%d %d\n", it->u, it->v);
	}
}
