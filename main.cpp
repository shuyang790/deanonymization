//
//  main.cpp
//  deanonymization
//
//  Created by Shuyang Shi on 15/9/20.
//  Copyright © 2015年 Shuyang Shi. All rights reserved.
//

#include <cstdio>
#include "graph.hpp"
#include "match.hpp"
#include "data_analysis.hpp"

#define DEBUG 1

#define STR_LEN 128

int main(int argc, const char * argv[]) {

	char str[STR_LEN], tmp[STR_LEN];
	graph *G_a, *G;

	if (argc == 3){
		strcpy(str, "data/");
		strcat(str, argv[1]);
		strcat(str, "\%");
		strcat(str, argv[2]);
		strcpy(tmp, str);
		strcat(tmp, "/anonymized.txt");
		G_a = new graph(tmp);
		strcpy(tmp, str);
		strcat(tmp, "/crawled.txt");
		G = new graph(tmp);
	}
	else {
		//graph G_a("anonymized.txt");
		//graph G("crawled.txt");

		G_a = new graph("data/100\%naive/anonymized.txt");
		G = new graph("data/100\%naive/crawled.txt");

//		G_a = new graph("data/50\%naive/anonymized.txt");
//		G = new graph("data/50\%naive/crawled.txt");
	}
	matcher *M = new matcher(G_a, G);
#if DEBUG
	M->debug_print();
#endif
	M->gen_sim_matrix_simranc();

	//M->load_matrix();
#if DEBUG
	M->record_matrix();
	M->gen_ans_pairs_oldway();
	FILE * backup_res = fopen("bak_res.txt", "w");
	M->print(backup_res);
	fclose(backup_res);
	fprintf(stderr, "old ways result generated in `bak_res.txt`.\n");
#endif
	M->gen_ans_pairs();
	M->print(stdout);

    return 0;
}
