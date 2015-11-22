
f = open("data/50%/pair_a_c.txt", "r")
lines = f.readlines()
f.close()

cnt = 0
for line in lines:
    if (eval(line.split(" ")[0])<=5000 and eval(line.split(" ")[1])<=5000):
        cnt += 1

print cnt
