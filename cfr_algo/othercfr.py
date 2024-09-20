import random
import sys
import numpy as np
import math

def get_action(odds):
    x = random.uniform(0,1)
    tmp = 0
    
    for a in range(0,len(odds)):
        tmp += odds[a]

        if tmp >= x:
            return a

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

# This Table describes the winng reward for each action
solution = [0.4, 0.4, 0.2]
winning = [ [0, -1, 2],
            [1, 0, -2],
            [-1, 1, 0] ]

trys = 1
time = 100000

data = np.zeros( (time, 3) )

for z in range(trys):
    rp1 = [ random.uniform(0, 1),  random.uniform(0, 1),  random.uniform(0, 1) ]
    rn1 = [ random.uniform(0, 1), random.uniform(0, 1), random.uniform(0, 1) ]

    rp2 = [ 1/3,  1/3, 1/3 ]
    rn2 = [ 1/3,  1/3, 1/3 ]
    
    for ti in range(time):
        s1 = percentage(rp1, rn1)
        s2 = percentage(rp2, rn2)
        a1 = get_action( s1 )
        a2 = get_action( s2 )

        if ti % 2 == 0:
            for x in range(3):
                t =  winning[x][a1] * s1[a1]

                if( t > 0 ):
                    rp1[x] += t
                else:
                    rn1[x] -= t
            
            a = percentage(rp1,rn1)
            for y in range(3):
                data[ti][y] = data[ti][y] + abs( a[y] - solution[y] )


    d1 = percentage(rp1,rn1)
    d2 = percentage(rp2,rn2)
    print(d1)
    print(len(d1))

err = 0
out = []
'''
for x in data:
    err = 0
    for i,y in enumerate(x):
        err += y / (trys * 3 )
    out.append( err )
for i,x in enumerate(out):
    print(i," ",x," ", (1/math.sqrt(i+1)) )
    '''
