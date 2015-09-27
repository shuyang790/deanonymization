
num_limit = 500

def main(src_dir, dst_dir, file_name, flag):
	fin = open (src_dir + file_name)
	lines = fin.readlines()
	fin.close()
	
	fou = open (dst_dir + file_name, "w")

	i = 0
	if flag:
		fou.write(lines[0])
		i = 1
	for line in lines[i:]:
		eles = line.split()
		a = eval(eles[0])
		b = eval(eles[1])
		if a <= num_limit and b <= num_limit:
			fou.write("%d %d\n" % (a, b))

	fou.close()

if __name__ == "__main__":
	main("data/50%/", "data/50_small/", "anonymized.txt", 1)
	main("data/50%/", "data/50_small/", "crawled.txt", 1)
	main("data/50%/", "data/50_small/", "pair_a_c.txt", 0)
