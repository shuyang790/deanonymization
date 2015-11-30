#!/usr/bin/python
import os
from compare import compare

# overlap rate
overlap = 50

# anonymization flag
aflag = "sparsify"

# number
number = 0

flags = aflag + (str(number) if number > 0 else "")
run_cmd = "./main " + str(overlap) + " " + flags
LOG_filename = "result/LOG" + str(overlap) + flags
res_filename = "result/" + str(overlap) + flags
std_filename = "data/" + str(overlap) + "%" + flags + "/pair_a_c.txt"

a_filename = "data/" + str(overlap) + "%" + flags + "/anonymized.txt"
c_filename = "data/" + str(overlap) + "%" + flags + "/crawled.txt"

f = open(a_filename, "r")
num1 = eval(f.readline().split(" ")[0])
f.close()
f = open(c_filename, "r")
num2 = eval(f.readline().split(" ")[0])
f.close()

num = num1 if num1 < num2 else num2

os.system("make")
os.system("time " + run_cmd + " > " + LOG_filename)
compare(LOG_filename, std_filename, num, overlap * 100, res_filename)
compare("bak_res.txt", std_filename, num, overlap * 100, res_filename + "_old_way")
