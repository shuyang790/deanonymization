//
//  match.cpp
//  deanonymization
//
//  Created by Shuyang Shi on 15/9/20.
//  Copyright © 2015年 Shuyang Shi. All rights reserved.
//

#include "match.hpp"
#include "match_vars.hpp"

matcher::matcher(class graph * g_a, class graph * g): G_a(g_a), G(g) {
	for (int i=1; i<=g_a->num_nodes; i++)
		for (int j=1; j<=g->num_nodes; j++)
			sim_nodes[i][j] = 1;
}

void matcher::print_top_simi() {
	FILE *ou = fopen("most_simi.log", "w");
	for (int i=1, j, k; i<=G_a->num_nodes; i++) {
#ifdef BASELINE
			for (j=k=1; k<=G->num_nodes; k++)
				if (sim_nodes[i][k] > sim_nodes[i][j])
					j = k;
#else
        for (j=k=1; k<=G->num_nodes; k++)
            if (sim_nodes[i][k] > sim_nodes[i][j])
                j = k;

#endif
		fprintf(ou, "%d %d\n", i, j);
	}
	fclose(ou);
}

void matcher::debug_print() {
	FILE *ou = fopen("debug.info", "w");
	fprintf(ou, "G_a: %d nodes,\t G:%d nodes.\n",
			G_a->num_nodes, G->num_nodes);
	int sum[2], S=0;
	sum[0]=sum[1]=0;
	for (int i=1; i<=G_a->num_nodes; i++){
		graph::subgraph *subg = G_a->extract_subgraph(i);
		sum[0] += subg->num_nodes_seq[0];
		sum[1] += subg->num_nodes_seq[1];
		S += subg->num_nodes_seq[0] * subg->num_nodes_seq[0];
		S += subg->num_nodes_seq[1] * subg->num_nodes_seq[1];
	}
	fprintf(ou, "Average neighbor number (G_a): %d %d\t#%d\n",
			sum[0] / G_a->num_nodes, sum[1] / G->num_nodes, S);
	fclose(ou);
}

double matcher::calc_sim_nodes(int u, int v, int level) {
#ifdef USE_ONLY_NEIGHBORS
	level = 1;
#endif

	graph::subgraph * subg_a = G_a->extract_subgraph(u);
	graph::subgraph * subg = G->extract_subgraph(v);

	double w = 0;
	vector <match_edge> match_edges;
	set <int> flag_a;
	set <int> flag;

	for (int t=0; t<2; t++) {
		flag_a.clear();
		flag.clear();
		match_edges.clear();
		for (int i=0; i < level; i++) {
			for (vector<int> :: iterator j = subg_a[t].nodes_per_level[i].begin();
					j!=subg_a[t].nodes_per_level[i].end(); j++)
				for (vector<int> :: iterator k = subg[t].nodes_per_level[i].begin();
						k!=subg[t].nodes_per_level[i].end(); k++)
					match_edges.push_back(match_edge(*j, *k, last_round[*j][*k]));
		}
		sort(match_edges.begin(), match_edges.end());
		for (vector<match_edge> :: iterator it=match_edges.begin();
				it!=match_edges.end(); it++) {
			if (flag_a.find(it->u) == flag_a.end()
					&& flag.find(it->v) == flag.end()) {
				flag_a.insert(it->u);
				flag.insert(it->v);
                    w += it->w;
			}
		}
	}

#ifndef BASELINE
		// RoleSim
		int weight = (int) max(G_a->edges[u]->size(), G->edges[v]->size())
                + (int) max(G_a->rev_edges[u]->size(), G->rev_edges[v]->size());
		if (weight > 0)
			w /= weight;
		return sim_nodes[u][v] = w * (1 - BETA) + BETA;
#else
		return sim_nodes[u][v] = w;
#endif
}

#ifdef ALPHA_R
void matcher::calc_sim_nodes_alpha(int i) {
    int top_idx = 1;
    for (int j=1; j<=G->num_nodes; j++)
        if (last_round[i][j] >= ALPHA)
            calc_sim_nodes(i, j, 1);
        else
            sim_nodes[i][j] = last_round[i][j];
}
#endif

#if MULTITHREAD
void * calc_sim_nodes_pthread(void * arg) {
	int idx = *(int*)arg;
	for (int i=idx+1; i<=MTCR->num_nodes_G_a(); i+=THREAD_POOL_SIZE){
#ifdef ALPHA_R
			MTCR->calc_sim_nodes_alpha(i);
#else
        for (int j=1; j<=MTCR->num_nodes_G(); j++)
            MTCR->calc_sim_nodes(i, j, 1);
#endif
	}
	return NULL;
}
#endif

int matcher::num_nodes_G_a() const {
	return G_a->num_nodes;
}

int matcher::num_nodes_G() const {
	return G->num_nodes;
}

void matcher::record_matrix(char *filename) {
	FILE *ou;
	if (filename)
	 	ou = fopen(filename, "w");
	else
		ou = fopen("matrix.txt", "w");
	fprintf(ou, "%d %d\n", G_a->num_nodes, G->num_nodes);
	for (int i=1; i<=G_a->num_nodes; i++){
		for (int j=1; j<=G->num_nodes; j++)
            fprintf(ou, "%g\t", sim_nodes[i][j]);
		fprintf(ou, "\n");
	}
	fclose(ou);
	fprintf(stderr, "matrix saved in `%s`.\n",
			filename ? filename : "matrix.txt");
}

void matcher::load_matrix(char *filename) {
	FILE *in;
	if (filename)
		in = fopen(filename, "r");
	else
	 	in = fopen("matrix.txt", "r");
	fscanf(in, "%d%d", &(G_a->num_nodes), &(G->num_nodes));
	for (int i = 1; i <= G_a->num_nodes; i++)
		for (int j = 1; j <= G->num_nodes; j++) {
            fscanf(in, "%lf", sim_nodes[i] + j);
        }
	fclose(in);
	fprintf(stderr, "Matrix read from `%s`\n",
			filename ? filename : "matrix.txt");
}

void matcher::gen_sim_matrix_simranc() {
	clock_t time_start = clock();

    for (int i=1; i<=G_a->num_nodes; i++)
        for (int j=1; j<=G->num_nodes; j++)
            sim_nodes[i][j] = 1;

#if MULTITHREAD
	for (int i=1; i<=G_a->num_nodes; i++)
		G_a->extract_subgraph(i);
	for (int i=1; i<=G->num_nodes; i++)
		G->extract_subgraph(i);
#endif

	for (cT=0; cT++ < MAX_ROUNDS; ) {

		fprintf(stderr, "Processing round %d ... \r", cT);

		// update node similarities for every pair of nodes
		memcpy(last_round, sim_nodes, sizeof(sim_nodes));

#if MULTITHREAD
		MTCR = this;
		for (int i=0; i<THREAD_POOL_SIZE; i++){
			args[i] = i;
			pthread_create(threads+i, NULL, calc_sim_nodes_pthread, args+i);
		}
		for (int i=0; i<THREAD_POOL_SIZE; i++)
			pthread_join(threads[i], NULL);
#else
#ifndef BASELINE
			for (int i=1; i<=G_a->num_nodes; i++)
				calc_sim_nodes_alpha(i);
#else
			for (int i=1; i<=G_a->num_nodes; i++)
				for (int j=1; j<-=G->num_nodes; j++)
					calc_sim_nodes(i, j, 1);
#endif
#endif

		fprintf(stderr, "Round %d processed\n", cT);
	}

	fprintf(stderr, "matcher info:\t%d rounds, %.2lf seconds.\n",
			MAX_ROUNDS, (clock()-time_start)*1.0/CLOCKS_PER_SEC);
}

void matcher::gen_ans_pairs_oldway() {
	fprintf(stderr, "gen ans pairs old way\n");
	ans_pairs.clear();
	vector<match_edge> match_edges;
	char * flag_a = new char[MAX_NODES], * flag = new char[MAX_NODES];
	memset(flag_a, 0, MAX_NODES);
	memset(flag, 0, MAX_NODES);

	for (int i=1; i <= G_a->num_nodes; i++)
		for (int j=1; j <= G->num_nodes; j++)
			match_edges.push_back(match_edge(i, j, sim_nodes[i][j]));
	sort(match_edges.begin(), match_edges.end());

	for (vector <match_edge> :: iterator it=match_edges.begin();
			it!=match_edges.end(); it++) {
		if (!flag_a[it->u] && !flag[it->v]) {
			flag_a[it->u] = 1;
			flag[it->v] = 1;
			ans_pairs.push_back(*it);
		}
	}

	delete []flag_a;
	delete []flag;
}

void matcher::gen_ans_pairs() {
	clock_t time_start = clock();
	ans_pairs.clear();
	char * flag_a = new char[MAX_NODES], * flag = new char[MAX_NODES];
	char * fake_flag_a = new char[MAX_NODES], * fake_flag = new char[MAX_NODES];
	memset(fake_flag_a, 0, MAX_NODES);
	memset(fake_flag, 0, MAX_NODES);
	memset(flag_a, 0, MAX_NODES);
	memset(flag, 0, MAX_NODES);

	int * match = new int[MAX_NODES];

	double TINY = 1e20;

	// find TINY
	for (int i=1; i<=G_a->num_nodes; i++)
		for (int j=1; j<=G->num_nodes; j++)
			if (sim_nodes[i][j] > 0 && TINY > sim_nodes[i][j])
				TINY = sim_nodes[i][j];
	fprintf(stderr, "TINY = %g\n", TINY);

	// initialization
	for (int i=1; i<=G_a->num_nodes; i++)
		for (int j=1; j<=G->num_nodes; j++) {
			weights[i][j] = sim_nodes[i][j];
			tops[i].push(make_pair(sim_nodes[i][j], j));
		}

	// matching process
	for (int idx, matched=0; matched < G_a->num_nodes; ) {

        idx=-1;
        for (int i=1; i<=G_a->num_nodes; i++) {
            while (!tops[i].empty() && flag[tops[i].top().second])
                tops[i].pop();
            if (!flag_a[i] && !tops[i].empty()) {
                if (idx == -1)
                    idx = i;
                else if (!tops[i].empty() && tops[i].top().first > tops[idx].top().first)
                    idx = i;
            }
        }

        if (idx < 0)
            break;

		int u = idx, v = tops[idx].top().second;
		tops[idx].pop();
		if (flag[v] || flag_a[u])
			continue;

        matched++;
		flag_a[u] = 1;
		flag[v] = 1;
		match[u] = v;
		ans_pairs.push_back(match_edge(u, v, weights[u][v]));

		for (vector <int> ::iterator i = G_a->edges[u]->begin();
				i != G_a->edges[u]->end(); i++)
			if (!flag_a[*i])
				for (vector <int> ::iterator j = G->edges[v]->begin();
						j != G->edges[v]->end(); j++)
					if (!flag[*j]) {
						weights[*i][*j] += sim_nodes[*i][*j];//max(sim_nodes[*i][*j], TINY);
						tops[*i].push(make_pair(weights[*i][*j], *j));
					}

		for (vector <int> ::iterator i = G_a->rev_edges[u]->begin();
				i != G_a->rev_edges[u]->end(); i++)
			if (!flag_a[*i])
				for (vector <int> ::iterator j = G->rev_edges[v]->begin();
						j != G->rev_edges[v]->end(); j++)
					if (!flag[*j]) {
						weights[*i][*j] += sim_nodes[*i][*j];//max(sim_nodes[*i][*j], TINY);
						tops[*i].push(make_pair(weights[*i][*j], *j));
					}
	}

	fprintf(stderr, "%lu pairs matched.\n", ans_pairs.size());

	delete []match;
	delete []flag_a;
	delete []flag;
	delete []fake_flag_a;
	delete []fake_flag;
	fprintf(stderr, "answer pairs generated.\n\t%.2lf seconds.\n",
			(clock()-time_start)*1.0/CLOCKS_PER_SEC);
}

void matcher::print(FILE *ou) {
	for (vector<match_edge> :: iterator it=ans_pairs.begin();
			it!=ans_pairs.end(); ++it) {
#if PRINT_SIMI
		fprintf(ou, "%d %d %g\n", it->u, it->v, it->w);
#else
		fprintf(ou, "%d %d\n", it->u, it->v);
#endif

	}
}

