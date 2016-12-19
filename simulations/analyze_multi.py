#!/usr/bin/env python
import fileinput
import sys
import glob
import matplotlib.pyplot as plt
from omnet_vector import OmnetVector
import itertools
import numpy as np
import scipy as sp
import scipy.stats

resample = 1

def mean_confidence_interval(data, confidence=0.95):
    a = 1.0*np.array(data)
    n = len(a)
    m, se = np.mean(a), scipy.stats.sem(a)
    h = se * sp.stats.t._ppf((1+confidence)/2., n-1)
    return m, h

data = {}

for f in sys.argv[1:]:
    vec = OmnetVector(fileinput.input(f))
    ref = vec.get_vector("referenceChangeStat","Net802154.host[0].ipApp[0]",resample=resample)
    out = vec.get_vector("sensorSample","Net802154.host[0].ipApp[1]",resample=resample)
    sendInt = float(vec.get_attr("sendInt"))
    if sendInt not in data:
        data[sendInt] = []
    absError = np.abs(np.subtract(out[1],ref[1]))
    data[sendInt].append(np.mean(absError))

x = []
y = []
errs = []

for d in sorted(data.keys()):
    (m,err) = mean_confidence_interval(data[d])
    x.append(d)
    y.append(m)
    errs.append(err)

print x,y,errs

plt.figure(facecolor='white')
plt.bar(x, y, yerr=errs, width=0.1, color='#882222', align="center")
plt.xlabel('Packet Interval of Additional Traffic [s]')
plt.ylabel('Average Absolute Control Deviation')
plt.show()
