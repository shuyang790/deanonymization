//
//  main.cpp
//  deanonymization
//
//  Created by Shuyang Shi on 15/9/20.
//  Copyright © 2015年 Shuyang Shi. All rights reserved.
//

#include <iostream>
#include "graph.hpp"
#include "match.hpp"

int main(int argc, const char * argv[]) {
	
	//graph G_a("anonymized.txt");
	//graph G("crawled.txt");

	graph G_a("data/50\%/anonymized.txt");
	graph G("data/50\%/crawled.txt");
	
	matcher *M = new matcher(&G_a, &G, 1000);
	M->match();
	
	M->print(stdout);
	
    return 0;
}
