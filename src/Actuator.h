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

#ifndef ACTUATOR_H_
#define ACTUATOR_H_

#include <controldefs.h>
#include <vector>

#include "inet/common/INETDefs.h"
#include "inet/common/lifecycle/ILifecycle.h"
#include "inet/networklayer/common/L3Address.h"

#include "IPBaseApp.h"


class Actuator : public IPBaseApp {

private:
    bool isActive;

    // All equally named signals are globally mapped to one number
    static simsignal_t changeActiveSignal;

protected:

    enum selfMessageKinds {CHANGE_ACTIVATION};

    cMessage *selfMessageActuation;
    cMessage *selfMessageStatus;

    // IP Payload Length
    long actuatorStatusReplyLength;

    int actuatorControlProtocol;
    int actuatorStatusProtocol;

    std::vector<inet::L3Address> validControllers;

    simtime_t nextActReceived;

    ActuationCommand nextAct;
    ActuationCommand currentAct;

    inet::L3Address statusReplyReceiver;

protected:

    virtual void initialize(int stage) override;
    virtual void finish() override;
    virtual void processPacket(cPacket *pkt) override;
    virtual bool handleNodeStart(inet::IDoneCallback *doneCallback) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;

    void handleChangeActivation(inet::L3Address sender, ActuationCommand recvAct);

public:
    Actuator();
    virtual ~Actuator();
};

#endif /* ACTUATOR_H_ */
