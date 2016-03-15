#!/usr/bin/python
from os import system
from compare import compare
from sys import argv
from time import time, localtime, strftime

ISOTIMEFORMAT = '%Y-%m-%d-%X'

def init():
    system("(cd databuilder; mkdir bin; mkdir data)")
    system("(cd databuilder; make bins)")
    system("mkdir result")

def clean():
    system("(cd databuilder; rm -rf data; rm -rf bin)")
    system("(cd result; rm -rf *.log)")

def gen_graph(method, nodes, overlap):
    print ("Generating graphs ...")
    with open("databuilder/Makefile", "r") as f:
        lines = f.readlines()
    with open("databuilder/Makefile", "w") as f:
        f.write("OVERLAP=" + str(overlap) + "\n")
        f.write("NODE=" + str(nodes) + "\n")
        f.write("ENCRYPT=" + str(method) + "\n")
        for line in lines[3:]:
            f.write(line)
    system("(cd databuilder; make graph)")
    system("(cd databuilder; mv data/graph1.txt ../data/crawled.txt)")
    system("(cd databuilder; mv data/graphTarget.txt ../data/anonymized.txt)")
    system("(cd databuilder; mv data/pair.txt ../data/pair_a_c.txt)")

def extract_res(filename, num_nodes):
    with open(filename, "r") as f:
        lines = f.readlines()
    ret = [0 for x in xrange(num_nodes)]
    for _line in lines:
        line = _line.rstrip()
        if line == "":
            continue
        else:
            idx = eval(line.split(" ")[0])
            correct = eval(line.split(" ")[1])
            ret[idx-1] = correct
    return ret

def build_exec():
    system("(mkdir build; cd build; cmake ..; make; rm -rf build)")

def run_prog(name, nodes, overlap):
    print ("\n[Running] `" + name + "` ...")
    start_time = time()
    if name == "base":
        system("time ./main baseline > result/base_pair.log")
    else:
        system("time ./main > result/" + name + "_pair.log")
    end_time = time()
    compare("result/" + name + "_pair.log", \
            "data/pair_a_c.txt", \
            (nodes - overlap) / 2 + overlap, \
            overlap, \
            "result/" + name + "_onetime.log", \
            flag=1)
    system("mv most_simi.log result/" + name + "_mostsimi.log")
    compare("result/" + name + "_mostsimi.log", \
            "data/pair_a_c.txt", \
            (nodes - overlap) / 2 + overlap, \
            overlap, \
            "result/" + name + "_simires.log", \
            flag = 1)
    with open("result/" + name + "_simires.log", "r") as f:
        lines = f.readlines()
    for _line in lines:
        line = _line.rstrip()
        if (line != ""):
            simi_correct = eval(line.split(" ")[1])

    return (extract_res("result/" + name + "_onetime.log", (nodes - overlap) / 2 + overlap), \
            end_time - start_time,\
            simi_correct)

def test(method, nodes, overlap):
    gen_graph(method, nodes, overlap)
    build_exec()
    (res_algo, time_algo, simi_algo) = run_prog("algo", nodes, overlap)
    (res_base, time_base, simi_base) = run_prog("base", nodes, overlap)
    return (res_algo, res_base, time_algo, time_base, simi_algo, simi_base)


def main():
    T = eval(argv[1])
    M = eval(argv[2])
    N = eval(argv[3])
    O = eval(argv[4])

    init()

    num_nodes = (N-O)/2+O
    tot_algo = [0 for i in xrange(num_nodes)]
    tot_base = [0 for i in xrange(num_nodes)]
    time_algo = 0
    time_base = 0
    simi_algo = 0
    simi_base = 0
    for i in range(T):
        print ("\n===== Testing dataset %d =====" % (i,))
        (cur_algo, cur_base, curt_algo, curt_base, curs_algo, curs_base) = test(M, N, O)
        for u in xrange(num_nodes):
            tot_algo[u] += cur_algo[u]
            tot_base[u] += cur_base[u]
        time_algo += curt_algo
        time_base += curt_base
        simi_algo += curs_algo
        simi_base += curs_base
    for u in xrange(num_nodes):
        tot_algo[u] /= T
        tot_base[u] /= T
    time_algo /= T
    time_base /= T
    simi_base /= T
    simi_algo /= T

    dirname = strftime(ISOTIMEFORMAT, localtime())\
            .replace(' ', '-')\
            .replace(':', '-')
    system("mkdir result/%s" % (dirname))
    with open("result/" + dirname + "/algo_ave.res", "w") as f:
        for u in xrange(num_nodes):
            f.write(str(u+1) + " " + str(tot_algo[u]) + "\n")
    with open("result/" + dirname + "/base_ave.res", "w") as f:
        for u in xrange(num_nodes):
            f.write(str(u+1) + " " + str(tot_base[u]) + "\n")
    with open("result/" + dirname + "/time_cost.res", "w") as f:
        f.write("proposed_algorithm %.2f\n" % time_algo)
        f.write("baseline_algorithm %.2f\n" % time_base)
    with open("result/" + dirname + "/simi_pairs.res", "w") as f:
        f.write("proposed_algorithm %.2f\n" % simi_algo)
        f.write("baseline_algorithm %.2f\n" % simi_base)

    clean()

    print ("\n===== Task finished =====")
    print ("Find your results at `./result/" + dirname + "/`")

if __name__ == "__main__":
    if (len(argv) != 5):
        print ("Usage: run.py [T] [M] [N] [O]")
        print ("\tT -- number of data sets")
        print ("\tM -- anonymization method (0-naive, 1-sparsify, 2-switching) ")
        print ("\tN -- number of nodes in generator")
        print ("\tO -- number of overlap nodes")
    else:
        main()


