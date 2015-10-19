#########################################################################
# File Name: run.sh
# Author: 
# Created Time: 二  7/28 08:45:27 2015
#########################################################################
#!/bin/bash
./word2vec -train ../passage_without_users.txt -output ../e_vectors_c_100_3_0_e-5_4_0_15_nousers.bin -cbow 1 -size 100 -window 3 -negative 0 -sample 1e-5 -threads 4 -binary 0 -iter 15 
#./word2vec -train ../passage_large.txt -output ../e_vectors_c_100_3_0_e-5_4_0_15.bin -cbow 1 -size 100 -window 3 -negative 0 -sample 1e-5 -threads 3 -binary 0 -iter 15 
#./word2vec -train ../passage.txt -output ../vectors_c_100_3_0_e-5_4_0_100.bin -cbow 1 -size 100 -window 3 -negative 0 -sample 1e-5 -threads 4 -binary 0 -iter 100
#./word2vec -train ../passage.txt -output ../vectors_s_50_3_0_e-5_4_0_15.bin -cbow 0 -size 50 -window 3 -negative 0 -sample 1e-5 -threads 4 -binary 0 -iter 15 
