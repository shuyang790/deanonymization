//
//  match.cpp
//  deanonymization
//
//  Created by Shuyang Shi on 15/9/20.
//  Copyright © 2015年 Shuyang Shi. All rights reserved.
//

#include "match.hpp"
#include "C-Thread-Pool/thpool.hpp"

int abs(int x){
	return x < 0 ? -x : x;
}

matcher::matcher(class graph &g_a, class graph &g, int _num_ans_pairs): G_a(g_a), G(g), num_ans_pairs(_num_ans_pairs){
	for (vector<int> ::iterator i=g_a.nodes.begin(); i!=g_a.nodes.end(); i++) {
		for (vector<int> ::iterator j=g.nodes.begin(); j!=g.nodes.end(); j++) {
			sim_nodes[make_pair(*i, *j)] = 1;
			sim_subgraphs[make_pair(*i, *j)] = 1;
		}
	}
}

int matcher::calc_sim_nodes(int u, int v) {
	int w1 = 0;
	vector<int> * nb_a = G_a.extract_neighbors(u);
	vector<int> * nb = G.extract_neighbors(v);
	
	vector <match_edge> match_edges;
	set<int> flag_a, flag;
	for (vector<int> :: iterator i = nb_a->begin(); i!=nb_a->end(); i++)
		for (vector<int> :: iterator j = nb->begin(); j!=nb->end(); j++)
			match_edges.push_back(match_edge(*i, *j, sim_nodes[make_pair(*i, *j)]));
	sort(match_edges.begin(), match_edges.end());
	for (vector<match_edge> :: iterator i = match_edges.begin(); i!=match_edges.end(); i++) {
		if (flag_a.find(i->u)==flag_a.end() && flag.find(i->v) == flag.end()) {
			flag_a.insert(i->u);
			flag.insert(i->v);
			w1 += i->w;
		}
	}
	
	int w2 = sim_subgraphs[make_pair(u, v)];
	return ALPHA1 * w1 + BETA1 * w2;
}

int matcher::calc_sim_subgraphs(int u, int v) {
	graph::subgraph subg_a = G_a.extract_subgraph(u);
	graph::subgraph subg = G.extract_subgraph(v);
	
	int w1 = 0; // node sequence
	for (int i=0; i<L; i++) {
		w1 += - abs(subg_a.num_nodes_seq[i] - subg.num_nodes_seq[i]);
	}
	
	int w2 = 0; // sum(level * sim_nodes)
	vector <match_edge> match_edges;
	set<int> flag_a, flag;
	for (int i=0; i<L; i++) {
		for (vector<int> :: iterator j = subg_a.nodes_per_level[0].begin(); j!=subg_a.nodes_per_level[0].end(); j++) {
			for (vector<int> :: iterator k = subg.nodes_per_level[1].begin(); k!=subg.nodes_per_level[1].end(); k++) {
				match_edges.push_back(match_edge(*j, *k, sim_nodes[make_pair(*j, *k)]));
			}
		}
	}
	for (vector<match_edge> :: iterator it=match_edges.begin(); it!=match_edges.end(); it++) {
		if (flag_a.find(it->u)==flag_a.end() && flag.find(it->v)==flag.end()) {
			flag_a.insert(it->u);
			flag.insert(it->v);
			w2 += it->w;
		}
	}
	return ALPHA2 * w1 + BETA2 * w2;
}

void matcher::match() {
	clock_t time_start = clock();
	int cT;
	double sum=G_a.num_nodes*G.num_nodes, sum_last;
	//TODO: complete the function
	for (cT=0; ++cT; ) {
		// copy to sim_nodes_last
		for (map< pair<int, int>, int> :: iterator it=sim_nodes.begin(); it!=sim_nodes.end(); it++)
			sim_nodes_last.insert(*it);
		sum_last = sum;

		// update similarities for every pair of nodes
		for (vector<int> ::iterator i=G_a.nodes.begin(); i!=G_a.nodes.end(); i++) {
			for (vector<int> ::iterator j=G.nodes.begin(); j!=G.nodes.end(); j++) {
				int t = calc_sim_nodes(*i, *j);
				sim_nodes[make_pair(*i, *j)] = t;
				sum += abs(t);
			}
		}
		for (vector<int> ::iterator i=G_a.nodes.begin(); i!=G_a.nodes.end(); i++) {
			for (vector<int> ::iterator j=G.nodes.begin(); j!=G.nodes.end(); j++) {
				sim_subgraphs[make_pair(*i, *j)] = calc_sim_subgraphs(*i, *j);
			}
		}
		
		// check if converge(normalized)
		// TODO: normalization
		for (vector<int> ::iterator i=G_a.nodes.begin(); i!=G_a.nodes.end(); i++) {
			for (vector<int> ::iterator j=G.nodes.begin(); j!=G.nodes.end(); j++) {
				if (fabs(sim_nodes_last[make_pair(*i, *j)]/sum_last - sim_nodes[make_pair(*i, *j)]/sum) > eps) {
					goto next_round;
				}
			}
		}
		break;
	next_round:;
		printf("cT = %d\n", cT);
	}
	
	vector<match_edge> match_edges;
	set<int> flag_a, flag;
	for (vector<int> ::iterator i=G_a.nodes.begin(); i!=G_a.nodes.end(); i++) {
		for (vector<int> ::iterator j=G.nodes.begin(); j!=G.nodes.end(); j++) {
			match_edges.push_back(match_edge(*i, *j, sim_nodes[make_pair(*i, *j)]));
		}
	}
	sort(match_edges.begin(), match_edges.end());
	for (vector<match_edge>::iterator it=match_edges.begin(); it!=match_edges.end(); ++it) {
		if (flag_a.find(it->u)==flag_a.end() && flag.find(it->v)==flag.end()) {
			flag_a.insert(it->u);
			flag.insert(it->v);
			ans_pairs.push_back(*it);
		}
	}
	
	fprintf(stderr, "matcher info\n\t%d rounds, %.2lf seconds.\n", cT, (clock()-time_start)*1.0/CLOCKS_PER_SEC);
}

void matcher::print(FILE *ou) {
	for (vector<match_edge> :: iterator it=ans_pairs.begin(); it!=ans_pairs.end(); ++it) {
		fprintf(ou, "%d %d\n", it->u, it->v);
	}
}
