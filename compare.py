
def compare(prog_output, correct_pair, num_nodes, output):
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
		print ("dealing with No. %d\r" % i)
		eles = p_line.split()
		if eval(eles[1]) == dic[eval(eles[0])]:
			correct += 1
		f.write("G1: %s GA: %s G2: %d Total: %.2f%%\n" % (eles[1], eles[0], dic[eval(eles[0])], correct * 1.0 / i))
		i = i + 1

	f.close()
	print ("%s finished.\n" % prog_output)

if __name__ == "__main__":
	compare("result/LOG50", "data/50%/pair_a_c.txt", 7500, "result/50")
	compare("result/LOG100", "data/100%/pair_a_c.txt", 10000, "result/100")
