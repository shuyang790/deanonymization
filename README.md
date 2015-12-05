# De-anonymization
By bsnsk (And Jialin L) | Started @ Sept, 2015
***

## Contents
- [Project Description](#desc)
- [Previous Consideration](#prev)
- [Current Status](#curr)
	- [Version 1: DNMC](#dnmc)
	- [Version 2: BTCH](#btch)
- [理解与阐述](#unde)
	- [Baseline Algorithm](#base)
	- [Improvement Algorithm](#impr)
	- [贡献与创新](#cont)
- [Next Step](#next)


## Description <span id="desc"></span>
Social network de-anonymization (provided with an anonymized graph and a crawled graph)

## Previous Consideration <span id="prev"></span>

- 思路
	- 对度数小的点，原匹配算法不使用。利用已匹配的邻居信息判断，并分多次逐渐匹配所有点
- 流程
	1. 原算法匹配，结果中删去涉及到度数 `out_deg + in_deg< thrsd = 6`的点，删去的点用精炼算法
	2. 设置标志 TIME = 0 （用以控制次数）
	3. 对于点对`u, v`，按照已匹配邻居通路数为`sim_nodes[u][v]`的权重，计算`weight[u][v]`值
		- `weight[u][v] = R(u, v) * max(TINY, sim_nodes[u][v])`
		- 其中`TINY`为`sim_nodes`矩阵中最小的非零值
		- 其中`R(u,v)`为下面集合的大小
		$\{<u, u', v', v> | (u, u') \in E[G_a], (v, v') \in E[G], (u', v') \in Matched\}$
	5. 将`weight`从大到小排序，依次贪心处理
		1. 如果该次已经匹配10条边，
			则记录TIME++，并回到第2步，重新计算weight矩阵
		2. 否则，如果该对两个点都未匹配，则匹配该对
- 效果（略）

## Current Status: Use 'refinement' to match most vertices <span id="curr"></span>

借用前面想要refine的算法，做主要的迭代（而不仅仅是refine）。

目前分为DNMC和BTCH两个版本。(通过`match.hpp`中的宏定义来控制)

### DNMC: 每次匹配之后动态维护weights矩阵 <span id="dnmc"></span>
- 想法
	- 利用原来的迭代算法，计算出seeds（一定比例）。
		(由该算法的特点可以知道seeds的度数一定较大且正确性较高。)
	- 从seeds出发迭代匹配，每次匹配weights值最大的一对，匹配以后动态更新weights
		- 更新的时候，使用的公式是`weights[u][v] += sim_nodes[u][v] * DECAY(iterno)`
		- 其中，函数`DECAY`是一个不上升函数，自变量是迭代轮数`iterno`
	- 剩余未匹配的少量点用Baseline算法匹配。
- 效果
	- 1 graph with 7500 vertices graph (50% overlap, precision upper bound 66.7%)
		- Baseline: 45.83%
		- `DECAY(i)=1`
			- `perc_thrsd=0.07`, `DECAY(i)=1`: 52.45%. 提升6.62个百分点
			- `perc_thrsd=0.05`, `DECAY(i)=1`: 51.56%. 提升5.73个百分点
			- `perc_thrsd=0.10`, `DECAY(i)=1`: 51.52%. 提升5.69个百分点
		- `DECAY(i) = (1 - 1e-4) ^ i`
			- `perc_thrsd=0.07`, `DECAY(i) = (1 - 1e-4) ^ i`: 52.19%, 提升6.36个百分点
			- `perc_thrsd=0.05`, `DECAY(i) = (1 - 1e-4) ^ i`: 52.41%, 提升6.58个百分点


### BTCH: 每考虑若干对匹配之后重建weights矩阵 <span id="btch"></span>

- 思路
	- 用baseline算法得到占比重`PERC_THRSD`的匹配对
	- 用refine算法，一次`NUM_PER_ITER`对逐步匹配剩余点对
	- 余下的点对用baseline算法匹配
- 效果
	- Graph: 7500 vertices, 50%overlap (upper bound precision: 66.7%)
		- Baseline: 45.83%
		- `PERC_THRSD=0.05`, `NUM_PER_ITER=10`: 55.03%
			- 提升了9.20个百分点

### 实验结果

| Graph No. | Vertices Quantity | Overlap Rate | Upper Bound Precision | Program Version | Parameters | Precision | Improvement | Remarks |
|-----------|-------------------|--------------|-----------------------|-----------------|------------|-----------|-------------|--------|
| 1         | 7500              | 50%          | 66.7%                 | Baseline        |  /         |   45.83%  |    /   | / |
| 1         | 7500              | 50%          | 66.7%                 | DNMC            |PERC_THRSD = 0.07, DECAY(i)=1     |   52.45%  |  6.62 | Correct matches go first in the result       |
| 1         | 7500              | 50%          | 66.7%                 | BTCH            |PERC_THRSD = 0.05, NUM_PER_ITER = 10 |   55.03%  |   9.20 |      |
| 2         | 7500              | 50%          | 66.7%                 | Baseline        |  /         |   56.84%  |    /   | / |
| 2         | 7500              | 50%          | 66.7%                 | DNMC            |PERC_THRSD = 0.10, DECAY(i)=(1-1e-4)^i     |   58.73%  |  1.89 | Correct matches go first in the result       |
| 2         | 7500              | 50%          | 66.7%                 | BTCH            |PERC_THRSD = 0.05, NUM_PER_ITER = 10 |   59.75%  |   2.91 |      |
| 3         |10000              | 100%          | 100%                 | Baseline        |  /         |   90.80%  |    /   | / |
| 3         |10000              | 100%          | 100%                 | DNMC            |PERC_THRSD = 0.8, DECAY(i)=(1-3e-5)^i     |   90.85%  |  0.05 | Correct matches go first in the result       |
| 3         |10000              | 100%          | 100%                 | BTCH            |PERC_THRSD = 0.05, NUM_PER_ITER = 5 |   90.70%  |   -0.10 |      |

## 相关理解与阐述 <span id="unde"></span>
###  Baseline 算法 <span id="base"></span>
该算法在计算的时候未将点对的相似权值归一化，事实上，度数大的点对权值大。
事实上，度数大的点对，匹配的置信度相对也高。

因此，Baseline算法实际上是将**邻居相似度**和**度数置信度**统一在一个实数中进行迭代。

### 改进算法 <span id="impr"></span>
将Baseline得到的相似度矩阵`sim_nodes`作为基础，用Baseline匹配的前5％的点作为已匹配的种子，重新构造`weights`矩阵。
`weights`矩阵利用了匹配的**中间结果**，利用已经匹配的邻居数量作为权值，求匹配对的加权`sum(sim_nodes)`。

其中，两个版本的区别在于
- DNMC: 动态维护`weights`，用堆维护，每次匹配以后都更新（单线程运行）
- BTCH: 每次处理`NUM_PER_ITER`对候选匹配对之后，重建`weights`矩阵（重建过程可并行）

### 贡献与创新 <span id="cont"></span>
综合利用了**多种信息**：
- 邻居相似度；
- 度数大小作为置信度；
- 匹配中间结果的影响。

计算上，
- 利用Baseline算法得到种子，不需要数据给定种子；
- 单线程版本（`DNMC`）由于操作数量不多而并不慢；
- 批处理版本（`BTCH`）可以并行构建weights矩阵，速度更快，在粒度合适的时候效果甚至可能更好。

## Next Step <span id="next"></span>

- 多种匿名方法实验
- 准备整理？
