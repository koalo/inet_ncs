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

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include <PrimitiveLRUActuatorState.h>
#include <list>
#include <vector>
#include <map>

#include "inet/common/INETDefs.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "IController.h"
#include "IPBaseApp.h"
#include "SignalGenerator.h"


class Controller : public IPBaseApp {
public:
    Controller();
    virtual ~Controller();

    static simsignal_t referenceChangeSignal;
    static simsignal_t mappedControllerOutputSignal;
    static simsignal_t numProbablyActuatedSignal;

    static void print_states(std::vector<PrimitiveLRUActuatorState *> destStates);

protected:

    enum selfMessageKinds {};

    // IP Payload Lengths
    long sensorControlLength;
    long actuatorControlLength;
    long actuatorStatusRequestLength;

    // Network Layer Protocol Numbers
    int sensorDataProtocol;
    int sensorControlProtocol;
    int actuatorControlProtocol;
    int actuatorStatusProtocol;

    cMessage *selfMessage = nullptr;

    // Control Variables
    double referenceValue = 0;



    std::vector<inet::L3Address> vSensorAddresses;

    SignalGenerator signalGenerator;
    IController *controller = nullptr;

protected:

    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual bool handleNodeStart(inet::IDoneCallback *doneCallback) override;
    virtual void processPacket(cPacket *pkt) override;
    virtual void processStart() override;
    virtual void processStop() override;


    int mapControlValue(double ctrlOut);
    void changeState(int changeNum);

    void changeReferenceValue(double reference);
    virtual void finish() override;

    std::list<PrimitiveLRUActuatorState> actuatorStates;
};

#endif /* CONTROLLER_H_ */
