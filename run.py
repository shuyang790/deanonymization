#!/usr/bin/python
from os import system
from compare import compare
from sys import argv
from time import time, localtime, strftime, sleep

ISOTIMEFORMAT = '%Y-%m-%d-%X'

versions = [
        "topk", "algo", "base"
    ]
anonymization_name = [
        "naive", "sparsify", "switching"
    ]

def init():
    system("(cd databuilder; mkdir bin; mkdir data)")
    system("(cd databuilder; make bins)")
    system("mkdir result")
    system("mkdir data")

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
    sleep(1)
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

def run_prog(name, nodes, overlap):
    system("rm -rf build")
    if name == "base":
        system("(mkdir build; cd build; cmake .. -DBASELINE=ON; make)")
    elif name == "algo":
        system("(mkdir build; cd build; cmake .. -DTOPK=OFF; make)")
    elif name == "topk":
        system("(mkdir build; cd build; cmake ..; make)")

    print ("\n[Running] `" + name + "` ...")
    start_time = time()
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

    ret = {
            "tot": extract_res("result/" + name + "_onetime.log", (nodes - overlap) / 2 + overlap),
            "time": end_time - start_time,
            "simi": simi_correct
            }
    return ret

def test(method, nodes, overlap):
    gen_graph(method, nodes, overlap)
    ret = {}
    for v in versions:
        ret[v] = run_prog(v, nodes, overlap) 
    return ret


def main():
    T = eval(argv[1])
    M = eval(argv[2])
    N = eval(argv[3])
    O = eval(argv[4])

    init()

    num_nodes = (N-O)/2+O
    results = {}
    for v in versions:
        result = {
                "tot": [0 for i in xrange(num_nodes)],
                "time": 0,
                "simi": 0
                }
        results[v] = result
    for i in range(T):
        print ("\n===== Testing dataset %d =====" % (i,))
        cur_results = test(M, N, O)
        for v in versions:
            for u in xrange(num_nodes):
                results[v]["tot"][u] += cur_results[v]["tot"][u]
            results[v]["time"] += cur_results[v]["time"]
            results[v]["simi"] += cur_results[v]["simi"]
       
    for v in versions:
        for u in xrange(num_nodes):
            results[v]["tot"][u] /= T
        results[v]["time"] /= T
        results[v]["simi"] /= T

    dirname = strftime(ISOTIMEFORMAT, localtime())\
            .replace(' ', '-')\
            .replace(':', '-')
    system("mkdir result/%s" % (dirname))
    system('echo "Testing @ %s" > README' % dirname)
    system('echo "%d Cases, %s anonymization, %d nodes in generator, %d overlap nodes"'\
            % (T, anonymization_name[M], N, O))
    for v in versions:
        with open("result/" + dirname + "/" + v + "_ave.res", "w") as f:
            for u in xrange(num_nodes):
                f.write(str(u+1) + " " + str(results[v]["tot"][u]) + "\n")
    with open("result/" + dirname + "/time_cost.res", "w") as f:
        for v in versions:
            f.write("version: %s\ttime: %.2f\n" % (v, results[v]["time"]))
    with open("result/" + dirname + "/simi_pairs.res", "w") as f:
        for v in versions:
            f.write("version: %s\tsimi: %.2f\n" % (v, results[v]["simi"]))

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


