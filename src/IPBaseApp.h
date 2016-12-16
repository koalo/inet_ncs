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

#ifndef IPBASEAPP_H_
#define IPBASEAPP_H_


#include <vector>
#include <map>

#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/applications/base/ApplicationBase.h"

#include "inet/common/lifecycle/ILifecycle.h"



class IPBaseApp : public inet::ApplicationBase {

private:
    enum BaseSelfMessageKinds { START = 100, STOP };
    cMessage *startStopMessage = nullptr;

protected:

    // parameters: see the NED files for more info
    int protocol;
    simtime_t startTime;
    simtime_t stopTime;

    // statistics
    int numSent = 0;
    int numReceived = 0;
    static simsignal_t sentPkSignal;
    static simsignal_t rcvdPkSignal;

protected:

    // Sends given IP packet to destination host, default protocol can be set via "protocol" in ned file
    virtual void sendPacket(cPacket *payload, inet::L3Address destAddr, int proto=-1);

    virtual void initialize(int stage) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;
    virtual bool handleNodeStart(inet::IDoneCallback *doneCallback) override;
    virtual void finish() override;

    virtual void registerIPProtocol(int proto);

    bool enabledAppSimtime(simtime_t time);
    virtual void processPacket(cPacket *pkt);
    virtual void processStart() {};
    virtual void processStop() {};

    std::vector<inet::L3Address> destAddresses;

public:
    IPBaseApp();
    virtual ~IPBaseApp();
};

#endif /* IPBASEAPP_H_ */
