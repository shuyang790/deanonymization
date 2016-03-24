#!/usr/bin/python 

from time import sleep
from os import system
from compare import compare
import run

def run_topk(dirname):
    system("cp data/" + dirname + "/anonymized.txt data/")
    system("cp data/" + dirname + "/crawled.txt data/")
    system("cp data/" + dirname + "/pair.txt data/")
    system("./main > result/" + dirname + "_pair.log")
    sleep(1)
    compare("result/" + dirname + "_pair.log",\
            "data/pair.txt",\
            7500,\
            5000,\
            "result/" + dirname + ".log", \
            flag = 1)
    system("mv most_simi.log result/" + dirname + "_mostsimi.log")
    compare("result/" + dirname + "_mostsimi.log",\
            "data/pair.txt",\
            7500,\
            5000,\
            "result/" + dirname + "_simres.log",\
            flag=1)
    sleep(1)
    print "%s result: " % (dirname)
    system("tail -1 result/" + dirname + ".log")
    print "%s simi: " % (dirname)
    system("tail -1 result/" + dirname + "_simres.log")


def main():
    system("rm -rf build");
    system("(mkdir build; cd build; cmake ..; make)")
    run_topk("sparsify_50%")
    run_topk("switch_50%")

if __name__ == "__main__":
    main()
