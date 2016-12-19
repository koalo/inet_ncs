#
# Python module to parse OMNeT++ vector files
#
# Currently only suitable for small vector files since
# everything is loaded into RAM
#
# Authors: Florian Kauer <florian.kauer@tuhh.de>
#
# Copyright (c) 2015, Institute of Telematics, Hamburg University of Technology
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the Institute nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

import re
import scipy.interpolate
import numpy as np

vectors = []

class OmnetVector:
    def __init__(self,file_input):
        self.vectors = {}
        self.dataTime = {}
        self.dataValues = {}
        self.maxtime = 0
        self.attrs = {}

        for line in file_input:
            m = re.search("([0-9]+)\t([0-9]+)\t([0-9.e\-+]+)\t([0-9.e\-+na]+)",line)
            #m = re.search("([0-9]+)",line)
            if m:
                vector = int(m.group(1))
                if not vector in self.dataTime:
                    self.dataTime[vector] = []
                    self.dataValues[vector] = []
                time = float(m.group(3))
                self.dataTime[vector].append(time)
                self.maxtime = max(self.maxtime,time)
                self.dataValues[vector].append(float(m.group(4)))
            else:
                # vector 7  Net802154.host[0].ipApp[0]  referenceChangeStat:vector  ETV
                m = re.search("vector *([0-9]*) *([^ ]*) *(.*):vector",line)
                if m:
                    number = int(m.group(1))
                    module = m.group(2)
                    name = m.group(3)
                    if not name in self.vectors:
                        self.vectors[name] = {}
                    self.vectors[name][module] = number
                else:
                    m = re.search("attr ([^ ]*) ([^ ]*)\n",line)
                    if m:
                        self.attrs[m.group(1)] = m.group(2)
                        

    def get_vector(self,name,module,resample=None):
        num = self.vectors[name][module]
        (time,values) = (self.dataTime[num],self.dataValues[num])
        if resample != None:
            newpoints = np.arange(0,self.maxtime,resample)
            lastvalue = values[-1]
            return (newpoints, scipy.interpolate.interp1d(time,values,'zero',assume_sorted=True,
                                                          bounds_error=False,fill_value=(0,lastvalue)).__call__(newpoints))
        else:
            return (time,values)

    def get_attr(self,name):
        return self.attrs[name]

