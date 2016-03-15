#!/usr/bin/python
import sys

def compare(prog_output, correct_pair, num_nodes, max_correct_num, output, flag=0):
	f_p = open(prog_output)
	p_lines = f_p.readlines() # G_a, G
	f_p.close()

	f_c = open(correct_pair)
	c_lines = f_c.readlines() # G_a, G
	dic = {}
	for line in c_lines:
		eles = line.split()
		dic[eval(eles[0])] = eval(eles[1])
	f_c.close()

	f = open(output, "w")

	i = 1
	correct = 0
	for p_line in p_lines:
#		print "dealing with No. " + str(i) + "\r",
#                sys.stdout.flush()
		eles = p_line.split()
		if eval(eles[1]) == dic[eval(eles[0])] and eval(eles[1]) <= max_correct_num:
			correct += 1
		if flag == 0:
			f.write("G1: %s GA: %s G2: %d Total: %.2f%%\n" % (eles[1], eles[0], dic[eval(eles[0])], correct * 100.0 / i))
		else:
			f.write("%d %d\n" % (i, correct))
		i = i + 1

	f.close()
	print ("%s finished.\n" % prog_output)

if __name__ == "__main__":
#	compare("result/LOG50sparsify", "data/50%sparsify/pair_a_c.txt", 7500, 5000, "result/50sparsify")
#	compare("bak_res.txt", "data/50%sparsify/pair_a_c.txt", 7500, 5000, "result/50sparsify_old_way")

#	compare("result/LOG50naive_RSnew", "data/50%naive/pair_a_c.txt", 7500, 5000, "result/50naive_RSnew")
#	compare("bak_res.txt", "data/50%naive/pair_a_c.txt", 7500, 5000, "result/50naive_RSnew_old_way")

#	compare("bak_res.txt", "data/100%/pair_a_c.txt", 10000, 10000, "result/100_old_way")
#	compare("result/LOG100", "data/100%/pair_a_c.txt", 10000, 10000, "result/100")
        
        compare("result/algo_pair.log", "data/pair_a_c.txt", 2000, 2000, "result/algo_onetime.log")

        pass
