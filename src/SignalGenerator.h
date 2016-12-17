//
// Copyright (C) Jonas K., 2016 <i-tek@web.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef SIGNAL_GENERATOR_H_
#define SIGNAL_GENERATOR_H_

#include "IPBaseApp.h"

class SignalGenerator {
public:
    SignalGenerator();
    virtual ~SignalGenerator();
    void initialize(int stage, cModule* module);

    double getCurrentValue();

private:
    double startTime;
    double signalMax;
    double signalFrequency;

    enum {
        SINE = 0,
        STEP = 1
    } signalType;
};

#endif /* SIGNAL_GENERATOR_H_ */
