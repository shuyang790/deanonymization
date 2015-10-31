
#ifndef data_analysis_hpp
#define data_analysis_hpp

#include "match.hpp"
#include <vector>
#include <algorithm>
#include <map>

using namespace std;

class analyst {
private:
	void calc_degree(class graph &G, int * deg);

public:
	/* Analysis data and output features */
	void analysis(class matcher &M);

};

#endif
