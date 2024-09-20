import random
import sys
import numpy as np
import math

def get_Strategy(p):
    s_sum = 0
    strat = [0, 0, 0]

    for x in range(3):
        if p[x] > 0:
            s_sum += p[x]
        else:
            p[x] = 0

    if not s_sum > 0:
        for x in range(3):
            strat[x] = 1/3
    else:
        for x in range(3):
            if p[x] > 0:
                strat[x] = p[x] / s_sum
            else:
                strat[x] = 0
    return strat

def get_action(odds):
    x = random.uniform(0,1)
    tmp = 0
    
    for a in range(0,len(odds)):
        tmp += odds[a]

        if tmp >= x:
            return a

# This Table describes the winng reward for each action
solution = [0.4, 0.4, 0.2]
winning = [ [0, -1, 2],
            [1, 0, -2],
            [-1, 1, 0] ]

time = 10000

strat_sum_1 = [ 0, 0, 0 ]
strat_sum_2 = [ 0, 0, 0 ]
regret_sum1 = [ 0, 0, 0 ]

data = np.zeros( (time, 3) )

def train():
    # calc strategy
    s1 = get_Strategy(regret_sum1)

    # add strat to stratsum
    for x in range(3):
        strat_sum_1[x] += s1[x]

    # generating decisions
    d1 = get_action(s1);
    d2 = get_action(solution);
    
    # calc error
    for x in range(3):
        # other error - own error
        a = winning[x][d2] - winning[d1][d2]
        regret_sum1[x] += a


data1 = []
data2 = []
data3 = []

print(solution)
for ti in range(time):
    train()
    st = get_Strategy(strat_sum_1)

    data[0] = st[0]
    data[1] = st[1]
    data[2] = st[2]

print(regret_sum1)
from matplotlib import pyplot as plt 
plt.plot(data[0]) 
plt.plot(data[1])
plt.plot(data[2])
plt.show()

'''
err = 0
out = []

for x in data:
    err = 0
    for i,y in enumerate(x):
        err += y / (trys * 3 )
    out.append( err )

for i,x in enumerate(out):
    print(i," ",x," ", (1/math.sqrt(i+1)) )

'''
