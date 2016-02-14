//
//  match.cpp
//  deanonymization
//
//  Created by Shuyang Shi on 15/9/20.
//  Copyright © 2015年 Shuyang Shi. All rights reserved.
//

#include "match.hpp"
#include "match_vars.hpp"

int int_abs(int x){
	return x < 0 ? -x : x;
}

matcher::matcher(class graph * g_a, class graph * g): G_a(g_a), G(g) {
	for (int i=1; i<=g_a->num_nodes; i++)
		for (int j=1; j<=g->num_nodes; j++)
			sim_nodes[i][j] = 1;
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
#if AVERAGE_EACH_CALC
	int sum = 0;
#endif
	vector <match_edge> match_edges;
	set <int> flag_a;
	set <int> flag;

	for (int t=0; t<2; t++) {
		flag_a.clear();
		flag.clear();
		match_edges.clear();
		for (int i=0; i < level; i++) {
#if AVERAGE_EACH_CALC
			sum += min(subg_a[t].nodes_per_level[i].size(),
					subg[t].nodes_per_level[i].size());
#endif
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
#if AVERAGE_EACH_CALC
		w /= sum;
#endif
	}

#ifdef ROLE_SIMI
	int weight = min(G_a->edges[u]->size() + G_a->rev_edges[u]->size(),
			G->edges[v]->size() + G->rev_edges[v]->size());
	if (weight > 0)
		w /= weight;
#endif
	return sim_nodes[u][v] = w;
}

#if MULTITHREAD
void * calc_sim_nodes_pthread(void * arg) {
	int idx = *(int*)arg;
	for (int i=idx+1; i<=MTCR->num_nodes_G_a(); i+=THREAD_POOL_SIZE)
		for (int j=1; j<=MTCR->num_nodes_G(); j++)
			MTCR->calc_sim_nodes(i, j, 1);
	return NULL;
}
#endif

int matcher::num_nodes_G_a() const {
	return G_a->num_nodes;
}

int matcher::num_nodes_G() const {
	return G->num_nodes;
}

void matcher::calc_sim_nodes_singleth(int i, int j, bool flag){
	if (flag)
		calc_sim_nodes(i, j, 2);
	else
		calc_sim_nodes(i, j, 1);
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
		for (int j = 1; j <= G->num_nodes; j++)
			fscanf(in, "%lf", sim_nodes[i]+j);
	fclose(in);
	fprintf(stderr, "Matrix read from `%s`\n",
			filename ? filename : "matrix.txt");
}

void matcher::gen_sim_matrix_simranc() {
	clock_t time_start = clock();
	int cT;

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
		for (int i=1; i<=G_a->num_nodes; i++)
			for (int j=1; j<=G->num_nodes; j++)
				calc_sim_nodes_singleth(i, j);
#endif

		// normalization
		double max_ele_nodes = -1;
		for (int i=1; i<=G_a->num_nodes; i++)
			for (int j=1; j<=G->num_nodes; j++){
				double tmp = sim_nodes[i][j];
				max_ele_nodes = max(max_ele_nodes, tmp);
			}
		for (int i=1; i<=G_a->num_nodes; i++)
			for (int j=1; j<=G->num_nodes; j++)
				sim_nodes[i][j] /= max_ele_nodes;
		fprintf(stderr, "Round %d processed\n", cT);
	}

	fprintf(stderr, "matcher info\n\t%d rounds, %.2lf seconds.\n",
			MAX_ROUNDS, (clock()-time_start)*1.0/CLOCKS_PER_SEC);
}

void matcher::gen_ans_pairs_oldway() {
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

// Process a batch of pairs after a re-establishment of `weights`
#ifdef BTCH

void matcher::gen_ans_pairs() {
	clock_t time_start = clock();
	ans_pairs.clear();
	vector<match_edge> match_edges;
	char * flag_a = new char[MAX_NODES], * flag = new char[MAX_NODES];
	char * fake_flag_a = new char[MAX_NODES], * fake_flag = new char[MAX_NODES];
	memset(flag_a, 0, MAX_NODES);
	memset(flag, 0, MAX_NODES);
	memset(fake_flag_a, 0, MAX_NODES);
	memset(fake_flag, 0, MAX_NODES);

	int * match = new int[MAX_NODES];

	for (int i=1; i <= G_a->num_nodes; i++)
		for (int j=1; j <= G->num_nodes; j++)
			match_edges.push_back(match_edge(i, j, sim_nodes[i][j]));
	sort(match_edges.begin(), match_edges.end());

	for (vector <match_edge> :: iterator it=match_edges.begin();
			it!=match_edges.end(); it++)
		if (!fake_flag_a[it->u] && !fake_flag[it->v]) {
			fake_flag_a[it->u] = 1;
			fake_flag[it->v] = 1;
			if (ans_pairs.size() < G_a->num_nodes * PERC_THRSD
					&& !flag_a[it->u] && !flag[it->v]) {
				flag_a[it->u] = 1;
				flag[it->v] = 1;
				match[it->u] = it->v;
				ans_pairs.push_back(*it);
			}
			else if (ans_pairs.size() > G_a->num_nodes * PERC_THRSD)
				break;
		}

	fprintf(stderr, "First part: %lu pairs.\n", ans_pairs.size());

	int iterno = 0;
	double TINY = 1e20;

	for (int i=1; i<=G_a->num_nodes; i++)
		for (int j=1; j<=G->num_nodes; j++)
			if (sim_nodes[i][j] > 0 && TINY > sim_nodes[i][j])
				TINY = sim_nodes[i][j];

	fprintf(stderr, "TINY = %g\n", TINY);

iter:
//	fprintf(stderr, "iter %d @ %lu\n", iterno, ans_pairs.size());

	match_edges.clear();
	for (int i=1; i<=G_a->num_nodes; i++) {
		if (!flag_a[i]) {
			map <int, double> weight;
			for (vector <int> :: iterator j = G_a->edges[i]->begin();
					j != G_a->edges[i]->end(); j++)
				if (flag_a[*j]) {
					for (vector <int> :: iterator k = G->rev_edges[match[*j]]->begin();
							k != G->rev_edges[match[*j]]->end(); k++)
						if (!flag[*k])
							weight[*k] += max(sim_nodes[i][*k], TINY);
				}
			for (vector <int> :: iterator j = G_a->rev_edges[i]->begin();
					j != G_a->rev_edges[i]->end(); j++)
				if (flag_a[*j]) {
					for (vector <int> :: iterator k = G->edges[match[*j]]->begin();
							k != G->edges[match[*j]]->end(); k++)
						if (!flag[*k])
							weight[*k] += max(sim_nodes[i][*k], TINY);
				}
			for (map <int, double> :: iterator k = weight.begin();
					k!=weight.end(); k++)
				match_edges.push_back(match_edge(i, k->first, k->second));
		}
	}
	sort(match_edges.begin(), match_edges.end());
	for (vector <match_edge> :: iterator it=match_edges.begin();
			it!=match_edges.end(); it++) {
		if (it - match_edges.begin() >= NUM_PER_ITER){
			iterno++;
			goto iter;
		}
		if ( !flag_a[it->u] && !flag[it->v]) {
			flag_a[it->u] = 1;
			flag[it->v] = 1;
			match[it->u] = it->v;
			ans_pairs.push_back(*it);
		}
	}

	fprintf(stderr, "%lu pairs matched.\n", ans_pairs.size());

	for (vector<match_edge> :: iterator it=match_edges.begin();
			it!=match_edges.end(); it++)
			if (!flag_a[it->u] && !flag[it->v]){
				flag_a[it->u] = flag[it->v] = 1;
				match[it->u] = it->v;
				ans_pairs.push_back(*it);
			}

	fprintf(stderr, "%lu pairs processed.\n", ans_pairs.size());

	for (int i=1; i<=G_a->num_nodes; i++)
		if (!flag_a[i]) {
			for (int j=1; j<=G->num_nodes; j++)
				if (!flag[j]) {
					flag_a[i] = flag[j] = 1;
					match[i] = j;
					ans_pairs.push_back(match_edge(i, j, 0));
					break;
				}
		}

	delete []match;
	delete []flag_a;
	delete []flag;
	delete []fake_flag_a;
	delete []fake_flag;
	fprintf(stderr, "answer pairs generated.\n\t%.2lf seconds.\n",
			(clock()-time_start)*1.0/CLOCKS_PER_SEC);
}

#endif

// Dynamically maintain `weights` after each match
#ifdef DNMC
void matcher::gen_ans_pairs() {
	clock_t time_start = clock();
	ans_pairs.clear();
	vector<match_edge> match_edges;
	char * flag_a = new char[MAX_NODES], * flag = new char[MAX_NODES];
	char * fake_flag_a = new char[MAX_NODES], * fake_flag = new char[MAX_NODES];
	memset(flag_a, 0, MAX_NODES);
	memset(flag, 0, MAX_NODES);
	memset(fake_flag_a, 0, MAX_NODES);
	memset(fake_flag, 0, MAX_NODES);

	int * match = new int[MAX_NODES];

	for (int i=1; i <= G_a->num_nodes; i++)
		for (int j=1; j <= G->num_nodes; j++)
			match_edges.push_back(match_edge(i, j, sim_nodes[i][j]));
	sort(match_edges.begin(), match_edges.end());

	for (vector <match_edge> :: iterator it=match_edges.begin();
			it!=match_edges.end(); it++)
		if (!fake_flag_a[it->u] && !fake_flag[it->v]) {
			fake_flag_a[it->u] = 1;
			fake_flag[it->v] = 1;
			if (ans_pairs.size() < G_a->num_nodes * PERC_THRSD
					&& !flag_a[it->u] && !flag[it->v]) {
				flag_a[it->u] = 1;
				flag[it->v] = 1;
				match[it->u] = it->v;
				ans_pairs.push_back(*it);
			}
			if (ans_pairs.size() > G_a->num_nodes * PERC_THRSD)
				break;
		}

	fprintf(stderr, "First part: %lu pairs.\n", ans_pairs.size());

	double TINY = 1e20;
	for (size_t i = 1; i <= G_a->num_nodes; i++) {
		for (int j = 1; j <= G->num_nodes; j++)
			if (TINY > sim_nodes[i][j])
				TINY = sim_nodes[i][j];
	}

	for (int i=1; i<=G_a->num_nodes; i++) {
		if (!flag_a[i]) {
			map <int, double> weight;
			for (vector <int> :: iterator j = G_a->edges[i]->begin();
					j != G_a->edges[i]->end(); j++)
				if (flag_a[*j]) {
					for (vector <int> :: iterator k = G->rev_edges[match[*j]]->begin();
							k != G->rev_edges[match[*j]]->end(); k++)
						if (!flag[*k])
							weight[*k] += max(sim_nodes[i][*k], TINY);
				}
			for (vector <int> :: iterator j = G_a->rev_edges[i]->begin();
					j != G_a->rev_edges[i]->end(); j++)
				if (flag_a[*j]) {
					for (vector <int> :: iterator k = G->edges[match[*j]]->begin();
							k != G->edges[match[*j]]->end(); k++)
						if (!flag[*k])
							weight[*k] += max(sim_nodes[i][*k], TINY);
				}
			for (map <int, double> :: iterator k = weight.begin();
					k!=weight.end(); k++)
				weights[i][k->first] = k->second;
		}
	}

	fprintf(stderr, "init weights.\n");

	int iterno = 0;
	H = new matcher::heap(weights, G_a->num_nodes, this);

	for (int last_s = 0, u, v; H->len && ans_pairs.size() != last_s; iterno++) {
		last_s = ans_pairs.size();
		while (H->len){
			u = H->nodes[1].u, v = H->nodes[1].v;
			H->pop();
			if (!flag_a[u] && !flag[v])
				break;
		}
		if (flag_a[u] || flag[v])
			break;
		flag_a[u] = flag[v] = 1;
		match[u] = v;
		ans_pairs.push_back(match_edge(u, v, weights[u][v]));

		for (vector <int> :: iterator j = G_a->edges[u]->begin();
				j != G_a->edges[u]->end(); j++)
			if (!flag_a[*j]) {
				for (vector <int> :: iterator k = G->edges[v]->begin();
						k != G->edges[v]->end(); k++)
					if (!flag[*k]){
						weights[*j][*k] += max(sim_nodes[*j][*k], TINY)
								* DECAY(iterno);
						if (H->heap_pos[*j][*k])
							H->heap_up(H->heap_pos[*j][*k]);
						else
							H->push(*j, *k);
					}
			}

		for (vector <int> :: iterator j = G_a->rev_edges[u]->begin();
				j != G_a->rev_edges[u]->end(); j++)
			if (!flag_a[*j]) {
				for (vector <int> :: iterator k = G->rev_edges[v]->begin();
						k != G->rev_edges[v]->end(); k++)
					if (!flag[*k]){
						weights[*j][*k] += max(sim_nodes[*j][*k], TINY)
								* DECAY(iterno);
						if (H->heap_pos[*j][*k])
							H->heap_up(H->heap_pos[*j][*k]);
						else
							H->push(*j, *k);
					}
			}
	}

	fprintf(stderr, "%lu pairs matched.\n", ans_pairs.size());

	for (std::vector<match_edge> :: iterator it=match_edges.begin();
			it!=match_edges.end(); it++)
			if (!flag_a[it->u] && !flag[it->v]){
				flag_a[it->u] = flag[it->v] = 1;
				match[it->u] = it->v;
				ans_pairs.push_back(*it);
			}
	fprintf(stderr, "%lu pairs processed.\n", ans_pairs.size());

	for (int i=1; i<=G_a->num_nodes; i++)
		if (!flag_a[i]) {
			for (int j=1; j<=G->num_nodes; j++)
				if (!flag[j]) {
					flag_a[i] = flag[j] = 1;
					match[i] = j;
					ans_pairs.push_back(match_edge(i, j, 0));
					break;
				}
		}

	delete H;

	delete []match;
	delete []flag_a;
	delete []flag;
	delete []fake_flag_a;
	delete []fake_flag;
	fprintf(stderr, "answer pairs generated.\n\t%.2lf seconds.\n",
			(clock()-time_start)*1.0/CLOCKS_PER_SEC);
}
#endif

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

// ===============
// matcher::heap
// heap operations

#ifdef DNMC
matcher::heap::heap(class matcher *o) {
	len = 0;
	owner = o;
}

matcher::heap::heap(int n, int m, class matcher *o) {
	owner = o;
	len = 0;
	memset(heap_pos, 0, sizeof(heap_pos));
	for (int i=1; i<=n; i++)
		for (int j=1; j<=m; j++){
			heap_pos[i][j] = ++len;
			nodes[len] = heap_node(i, j);
		}
	for (int i=len/2; i>0; i--)
		heap_down(i);
}

matcher::heap::heap(map <int, double> *weights, int n, class matcher *o) {
	owner = o;
	len = 0;
	for (int i=1; i<=n; i++){
		for (std::map<int, double> ::iterator it=weights[i].begin();
			it != weights[i].end(); it++) {
			heap_pos[i][it->first] = ++len;
			nodes[len] = heap_node(i, it->first);
		}
	}
	for (int i=len/2; i>0; i--)
		heap_down(i);
}

void matcher::heap::push(int u, int v) {
	int pos = heap_pos[u][v];
	if (pos <= len && nodes[pos].u == u && nodes[pos].v == v)
		return;
	nodes[++len] = heap_node(u, v);
	heap_pos[u][v] = len;
	heap_up(len);
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
#endif
