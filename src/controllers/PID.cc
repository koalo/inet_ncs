//
// Copyright (C) 2016 Jonas K. <i-tek@web.de>
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

#include <omnetpp.h>

#include "inet/common/INETDefs.h"
#include "inet/common/InitStages.h"
#include <controllers/PID.h>

using namespace inet;

Define_Module(PID);


simsignal_t PID::controllerPSignal = registerSignal("controllerP");
simsignal_t PID::controllerISignal = registerSignal("controllerI");
simsignal_t PID::controllerDSignal = registerSignal("controllerD");


PID::PID() {}
PID::~PID() {}


void PID::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {

        Kp = par("proportionalGain");
        Ki = par("integralGain");
        Kd = par("derivativeGain");
        integratorMagMax = par("integratorMax");

        lastSamplingTime = SIMTIME_ZERO; // ToDo: set last time to first sampling time

    } else if (stage == INITSTAGE_APPLICATION_LAYER) {
        // Almost same as handleNodeStart
    }
}

double PID::processSample(double sampleValue, double referenceValue)
{
    // Last sampling interval (could result in problems when sampling is paused)
    double lastInterval = (simTime() - lastSamplingTime).dbl();

    // Control Action
    double curDelta = referenceValue - sampleValue;
    emit(errorSignal, curDelta);

    // Anti-Windup
    if (integratorMagMax >= 0) {
        if (integrator + curDelta > integratorMagMax) {
            integrator = integratorMagMax;
        } else if (integrator + curDelta < -integratorMagMax) {
            integrator = -integratorMagMax;
        } else {
            integrator += curDelta;
        }
    }

    // PID-Controller
    // TODO:
    double p = Kp * curDelta;
    double i = Ki * lastInterval * integrator;
    double d = Kd * (curDelta - lastDelta)/lastInterval;
    double u = p + i + d;
    emit(controllerPSignal, p);
    emit(controllerISignal, i);
    emit(controllerDSignal, d);
    emit(outputSignal, u);

    // Derivative
    lastDelta = curDelta;
    lastSample = sampleValue;
    lastSamplingTime = simTime();

    return u;
}
