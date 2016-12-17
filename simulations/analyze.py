#!/usr/bin/env python
import fileinput
import matplotlib.pyplot as plt
from omnet_vector import OmnetVector

vec = OmnetVector(fileinput.input())
ref = vec.get_vector("referenceChangeStat","Net802154.host[0].ipApp[0]")
out = vec.get_vector("sensorSample","Net802154.host[0].ipApp[1]")

plt.step(ref[0],ref[1],"r.",where="post")
plt.step(out[0],out[1],"b.",where="post")
plt.show()
