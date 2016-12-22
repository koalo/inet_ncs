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

#ifndef CONTROLLERS_PID_H_
#define CONTROLLERS_PID_H_

#include <omnetpp.h>
#include "IController.h"

class PID : public IController, public omnetpp::cSimpleModule {

protected:

    static simsignal_t controllerPSignal;
    static simsignal_t controllerISignal;
    static simsignal_t controllerDSignal;

    simtime_t lastSamplingTime = 0;


    double lastSample = 0;
    double lastDelta = 0;

    double Kp;
    double Ki;
    double Kd;
    double integrator = 0;
    double integratorMagMax;

public:
    PID();
    virtual ~PID();

    void initialize(int stage) override;
    double processSample(double sampleValue, double referenceValue);
};

#endif /* CONTROLLERS_PID_H_ */
