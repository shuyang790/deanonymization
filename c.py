from compare import *
import os

os.system("make")
os.system("time ./main 50 naive > result/LOG50naive_RS")

# 50% sparsify
# compare("result/LOG50sparsify_RS", "data/50%sparsify/pair_a_c.txt", 7500, 5000, "result/50sparsify_RS")
# compare("bak_res.txt", "data/50%sparsify/pair_a_c.txt", 7500, 5000, "result/50sparsify_RS_old_way")

# 50% naive
compare("result/LOG50naive_RS", "data/50%naive/pair_a_c.txt", 7500, 5000, "result/50naive_RS")
compare("bak_res.txt", "data/50%naive/pair_a_c.txt", 7500, 5000, "result/50naive_RS_old_way")




os.system("time ./main 100 naive > result/LOG100naive_RS")

# 100% naive
compare("result/LOG100naive_RS", "data/100%naive/pair_a_c.txt", 10000, 10000, "result/100naive_RS")
compare("bak_res.txt", "data/100%naive/pair_a_c.txt", 10000, 10000, "result/100naive_RS_old_way")

