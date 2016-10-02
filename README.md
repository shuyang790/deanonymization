# De-anonymization
By Shuyang S (and Jialin L, Yuemei Z) | Started @ Sept, 2015
***

## Contents
- [Project Description](#desc)
- [Skeleton](#sklt)
- [Performance](#pfmc)
- [Experiment Tool](#next)


## Description <span id="desc"></span>
Social network de-anonymization (provided with an anonymized graph and a crawled graph)

## Skeleton <span id="sklt"></span>
The process is divided into two parts: firstly generate a similarity matrix for node pairs acrossing two graphs, 
and then use specific methods to match them into answer pairs.

## Performance <span id="pfmc"></span>
Our algorithm has a satisfying performance with considerable improvement compared with baseline algorithm.

## Experiment Tool <span id="expr"></span>
Use `run.py`.

### Requirement
- File `soc-Livejournal1.txt` placed at the root directory of this repository. 
- `Cmake` installed.

### Usage
Format: `./run.py [T] [M] [N] [O]`, where

- `T` is the number of dataset to be run; 
- `M` is the method of anonymization, where
	- `0` for naive,
	- `1` for sparsify, and
	- `2` for switching;
- `N` is the number of nodes in the subgraph which is used to generate two graphs;
- `O` is the number of nodes in the overlap part.

And therefore the number of nodes in generate graphs should be `(N - O) / 2 + O`.

An example is below:

```
$ ./run.py 1 0 10000 5000
```

Find your result at the folder `result/`.
