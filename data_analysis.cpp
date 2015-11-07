
#include "data_analysis.hpp"

void analyst::calc_degree(class graph &G, int * deg) {
	for (int i=1; i<=G.num_nodes; i++) {
		deg[i] = 0;
		for (vector <int> :: iterator it = G.edges[i]->begin(); it!=G.edges[i]->end(); it++)
			deg[i] ++;
	}
}

void analyst::analysis(class matcher &M) {

	map <int, int> correct_match;
	int * deg_a = new int[M.G_a->num_nodes+1];
	int * deg = new int[M.G->num_nodes+1];

	FILE * in = fopen("./data/100\%/pair_a_c.txt", "r");
	for (int i, j; fscanf(in, "%d%d", &i, &j) == 2; correct_match[j]=i);
	fclose(in);

	calc_degree(*(M.G_a), deg_a);
	calc_degree(*(M.G), deg);

	vector<match_edge> match_edges;
	char * flag_a = new char[MAX_NODES], * flag = new char[MAX_NODES];
	memset(flag_a, 0, MAX_NODES);
	memset(flag, 0, MAX_NODES);
	for (int i=1; i <= M.G_a->num_nodes; i++)
		for (int j=1; j <= M.G->num_nodes; j++)
			match_edges.push_back(match_edge(i, j, M.sim_nodes[i][j]));
	sort(match_edges.begin(), match_edges.end());

	vector <double> seq;

	FILE * ana = fopen("analysis.txt", "w");
	for (int i=0; i < M.ans_pairs.size(); i++) {
		if (M.ans_pairs[i].u != correct_match[M.ans_pairs[i].v])
			fprintf(ana, "@@@\t");
		fprintf(ana, "Node \t%d (a: \t%d) \tDegree \t%d (\t%d) match with a_node \t%d (deg \t%d): \t%g",
				M.ans_pairs[i].v,
				correct_match[M.ans_pairs[i].v],
				deg[M.ans_pairs[i].v],
				deg_a[correct_match[M.ans_pairs[i].v]],
				M.ans_pairs[i].u,
				deg_a[M.ans_pairs[i].u],
				M.ans_pairs[i].w
				);
		seq.clear();
		for (int j=1; j<=M.G_a->num_nodes; j++)
			if (!flag_a[j])
				seq.push_back(M.sim_nodes[j][M.ans_pairs[i].v]);
		flag_a[M.ans_pairs[i].u] = 1;
		flag[M.ans_pairs[i].v] = 1;
		sort(seq.begin(), seq.end());

		int k = 0;
		for (; k<seq.size() && seq[k] <= M.ans_pairs[i].w + 1e-6; k++);
		fprintf(ana, " (No. \t%d, score \t%g/%g)", (int)seq.size()-k+1, seq[k-1], seq[seq.size()-1]);
		fprintf(ana, "\n");
	}
	fclose(ana);

	delete []deg_a;
	delete []deg;
	delete []flag_a;
	delete []flag;
}
