f = open("bak_res.txt", "r")
lines = f.readlines()
f.close()
cnt1 = 0
for line in lines:
    a = eval(line.split(" ")[0])
    b = eval(line.split(" ")[1])
    if a==b:
        cnt1 += 1

f = open("T", "r")
lines = f.readlines()
f.close()
cnt2 = 0
for line in lines:
    a = eval(line.split(" ")[0])
    b = eval(line.split(" ")[1])
    if a==b:
        cnt2 += 1

print ("correct pairs: %d (old way: %d)" % (cnt2, cnt1))

