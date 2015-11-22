# De-anonymization
By bsnsk (And Jialin L) | Started @ Sept, 2015
***

## Description
Social network deanonymization (provided with an anonymized graph and a crawled graph)

## Current Problem

- 思路
	- 对度数小的点，原匹配算法不使用。利用已匹配的邻居信息判断，并分多次逐渐匹配所有点
- 流程
	1. 原算法匹配，结果中删去涉及到度数out_deg < thrsd = 3的点，删去的点用精炼算法
	2. 设置标志 TIME = 0 （用以控制次数）
	3. 按照已匹配邻居通路数，然后按照weight排序然后贪心选取
	4. weight增加的时候增加max(1e-10, sim_nodes[u][v])
	5. 将weight从大到小排序，依次处理
		1. 如果weight值<3e-10 (i.e. 基于当前状况只有最多两条通路）且TIME < 3，则记录TIME++，并回到第2步，重新计算weight矩阵
		2. 否则，匹配该边并继续
- 效果
	- 10000点的图（100% overlap）
		- 有提升，从81.43% 上升到 89.31%
		- 精炼前期匹配准确率很高，出错主要在理论上无法区分的点上
		- 精炼的整个过程中（一直到最后几对匹配）都有较多匹配正确的点
		- 最后有将近400个点只有一条通路，它们的匹配结果相对没有那么好（也可能是因为理论上无法匹配的点主要出现在这里）
	- 7500点的图（50% overlap）
		- 有提升，从50.15%上升到54.47%
		- 相关解释类似

## Next Step

- 理论上界计算
- 多种匿名方法实验
