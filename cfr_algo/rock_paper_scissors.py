import random
import sys
import numpy as np
import math

def percentage(p,n):
    s_sum = 0
    strat = [0, 0, 0]

    f = lambda p, n: n / p

    for x in range(3):
        a = f(p[x], n[x])

        if a > 0:
            s_sum += a  # Normalize p/(p+n) is better than p/n || create more stable results in this card but should be tested in the future

    for x in range(3):
        strat[x] = f(p[x], n[x]) / s_sum
    
    return strat

def lr(x, i):
    x = x * i**1.1  # is better than (i^2, sqrt(i), i * sqrt(i))
    return x

# This Table describes the winng reward for each action
solution = [0.4, 0.4, 0.2]
winning = [ [0, -1, 2],
            [1, 0, -2],
            [-1, 1, 0] ]

trys = 1
time = 1000

data = np.zeros( (time, 3) )

for z in range(trys):
    rp1 = [ random.uniform(0, 1),  random.uniform(0, 1),  random.uniform(0, 1) ]
    rn1 = [ random.uniform(0, 1), random.uniform(0, 1), random.uniform(0, 1) ]

    rp2 = [ random.uniform(0, 1),  random.uniform(0, 1),  random.uniform(0, 1) ]
    rn2 = [ random.uniform(0, 1), random.uniform(0, 1), random.uniform(0, 1) ]
    
    for ti in range(time):
        s1 = percentage(rp1, rn1)
        s2 = percentage(rp2, rn2)

        if ti % 2 == 0:
            for x in range(3):
                t = 0
                for y in range(3):
                    t += ( winning[x][y] * s1[y] * s2[x])

                t = lr(t,ti)

                if( t > 0 ):
                    rp1[x] += t
                else:
                    rn1[x] -= t
            
            a = percentage(rp1,rn1)
            for y in range(3):
                data[ti][y] = data[ti][y] + abs( a[y] - solution[y] )
        else:
            for x in range(3):
                t = 0
                for y in range(3):
                    t += ( winning[x][y] * s2[x] * s1[y])
                
                t = lr(t,ti)

                if( t > 0 ):
                    rp2[x] += t
                else:
                    rn2[x] -= t
            
            a = percentage(rp2,rn2)
            for y in range(3):
                data[ti][y] = data[ti][y] + abs( a[y] - solution[y] ) 
        

    d1 = percentage(rp1,rn1)
    d2 = percentage(rp2,rn2)


err = 0
out = []

for x in data:
    err = 0
    for i,y in enumerate(x):
        err += y / (trys * 3 )
    out.append( err )
for i,x in enumerate(out):
    print(i," ",x," ", (1/math.sqrt(i+1)) )
