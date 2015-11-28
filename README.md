# De-anonymization
By bsnsk (And Jialin L) | Started @ Sept, 2015
***

## Description
Social network de-anonymization (provided with an anonymized graph and a crawled graph)

## Current Problem

- 思路
	- 对度数小的点，原匹配算法不使用。利用已匹配的邻居信息判断，并分多次逐渐匹配所有点
- 流程
	1. 原算法匹配，结果中删去涉及到度数 `out_deg + in_deg< thrsd = 6`的点，删去的点用精炼算法
	2. 设置标志 TIME = 0 （用以控制次数）
	3. 按照已匹配邻居通路数，然后按照weight排序然后贪心选取
	4. weight增加的时候增加max(TINY, sim_nodes[u][v])，其中TINY为sim_nodes矩阵中最小的非零值
	5. 将weight从大到小排序，依次处理
		1. 如果该次已经匹配10条边，
		则记录TIME++，并回到第2步，重新计算weight矩阵
		2. 否则，匹配该边并继续
- 效果


## Next Step

- 理论上界计算
- 多种匿名方法实验
