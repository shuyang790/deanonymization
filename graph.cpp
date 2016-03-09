//
//  graph.cpp
//  deanonymization
//
//  Created by Shuyang Shi on 15/9/20.
//  Copyright © 2015年 Shuyang Shi. All rights reserved.
//

#include "graph.hpp"

graph::graph(const char * file_name){
	FILE * f=fopen(file_name, "r");
	if (!f) {
		exit(0);
	}
	fscanf(f, "%d%d", &num_nodes, &num_edges);
	edges.resize(num_nodes+1);
	rev_edges.resize(num_nodes+1);
	for (int i=1; i<=num_nodes; i++) {
		nodes.push_back(i);
		edges[i] = new vector<int>;
		rev_edges[i] = new vector<int>;
	}
	for (int i=0, j, k; i<num_edges; i++) {
		fscanf(f, "%d%d", &j, &k);
		edges[j]->push_back(k);
		rev_edges[k]->push_back(j);
	}
	fclose(f);
	memset(neighbors, 0, sizeof neighbors);
	memset(subgraphs, 0, sizeof subgraphs);
	fprintf(stderr, "Graph read: %d, %d\n", num_nodes, num_edges);
}

int graph::out_deg(int i) {
	return edges[i]->size();
}

graph::subgraph * graph::extract_subgraph(int node){
	if (subgraphs[node])
		return subgraphs[node];
	subgraph * re = new subgraph[2];
	re[0].center = re[1].center = node;
	re[0].num_nodes = re[1].num_nodes = 1;

	queue<int> Q;
	char * flag = new char[MAX_NODES];

	memset(flag, 0, MAX_NODES);
	flag[node]=1;
	Q.push(node);
	for (int i=0, last_cnt = 1; i<L; i++) {
		int cnt = 0;
		re[0].nodes_per_level[i].clear();
		while (last_cnt --) {
			int cur = Q.front();
			Q.pop();
			for (vector<int> :: iterator it=edges[cur]->begin();
					it!=edges[cur]->end(); it++){
				if (!flag[*it]) {
					flag[*it] = 1;
					Q.push(*it);
					re[0].nodes_per_level[i].push_back(*it);
					cnt++;
				}
			}
		}
		re[0].num_nodes_seq.push_back(cnt);
		re[0].num_nodes += cnt;
		last_cnt = cnt;
	}

	while (!Q.empty())
		Q.pop();
	memset(flag, 0, MAX_NODES);
	flag[node]=1;
	Q.push(node);
	for (int i=0, last_cnt = 1; i<L; i++) {
		int cnt = 0;
		re[1].nodes_per_level[i].clear();
		while (last_cnt --) {
			int cur = Q.front();
			Q.pop();
			for (vector<int> :: iterator it=rev_edges[cur]->begin();
					it!=rev_edges[cur]->end(); it++){
				if (!flag[*it]) {
					flag[*it] = 1;
					Q.push(*it);
					re[1].nodes_per_level[i].push_back(*it);
					cnt++;
				}
			}
		}
		re[1].num_nodes_seq.push_back(cnt);
		re[1].num_nodes += cnt;
		last_cnt = cnt;
	}

	delete []flag;
	return subgraphs[node] = re;
}
