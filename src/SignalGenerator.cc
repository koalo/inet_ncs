//
// Copyright (C) 2016 Florian Kauer <florian.kauer@koalo.de>
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

#include "SignalGenerator.h"
#include "inet/common/INETDefs.h"

using namespace inet;

SignalGenerator::SignalGenerator() {}
SignalGenerator::~SignalGenerator() {
}

void SignalGenerator::initialize(int stage, cModule* module) {
    if (stage == INITSTAGE_LOCAL) {
        std::string type = module->par("signalType").str();
        if(type == "\"sine\"") {
            signalType = SINE;
        }
        else if(type == "\"step\"") {
            signalType = STEP;
        }
        else {
            ASSERT(false);
        }

        startTime = module->par("startTime");
        signalMax = module->par("signalMax");
        signalFrequency = module->par("signalFrequency");
    }
}

double SignalGenerator::getCurrentValue() {
    auto time = simTime();
    if(time < startTime) {
        return 0;
    }
    else {
        double x = (time-startTime).dbl();
        switch(signalType) {
        case SINE:
            return (-cos(x*signalFrequency*2*M_PI)+1)*signalMax/2;
        case STEP:
            return signalMax;
        default:
            return 0;
        }
    }
}
