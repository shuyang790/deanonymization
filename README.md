# De-anonymization
By bsnsk (And Jialin L) | Started @ Sept, 2015
***

## Description
Social network deanonymization (provided with an anonymized graph and a crawled graph)

## Current Problem
	Mis-matched vertices with following features:
	- Matched in the 2nd half of the output;
	- With rather small out-degrees (mostly not greater than 3, a large part is 1);
	- Similarity scores with other nodes cannot be discerned.

## Next Step
	Develop algorithm for refinement. Possible solutions:
	- Vertices with small degrees shall be forced to match according to neighbor matching results (similar to KNN);
	- Vertices with small degrees shall match with the help of rough global position informaiton.
