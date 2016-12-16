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


#include "inet/common/INETDefs.h"
#include "inet/common/ProtocolGroup.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/IProtocolRegistrationListener.h"

#include "inet/networklayer/contract/IL3AddressType.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/contract/INetworkProtocolControlInfo.h"
#include "inet/applications/base/ApplicationBase.h"

#include "DestinationParser.h"
#include "ModuleArrayTokenBuilder.h"

#include "IPBaseApp.h"
#include <list>

using namespace inet;


Define_Module(IPBaseApp)

simsignal_t IPBaseApp::sentPkSignal = registerSignal("sentPk");
simsignal_t IPBaseApp::rcvdPkSignal = registerSignal("rcvdPk");



IPBaseApp::IPBaseApp() {}
IPBaseApp::~IPBaseApp() {
    cancelAndDelete(startStopMessage);
}

void IPBaseApp::initialize(int stage)
{
    ApplicationBase::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {

        numSent = 0;
        numReceived = 0;
        WATCH(numSent);
        WATCH(numReceived);

        protocol = par("protocol");
        startTime = par("startTime").doubleValue();
        stopTime = par("stopTime").doubleValue();

        if (stopTime >= SIMTIME_ZERO && stopTime < startTime)
            throw cRuntimeError("Invalid startTime/stopTime parameters");

        startStopMessage = new cMessage("startStopMessage");

    } else if (stage == INITSTAGE_TRANSPORT_LAYER) {
        // Only register reception of protocol if given as ned parameter
        if (protocol != -1)
            registerIPProtocol(protocol);
    }

}

/**
 * Returns whether passed point in time lies within the application's
 * run time.
 */
bool IPBaseApp::enabledAppSimtime(simtime_t time)
{
    return (startTime <= time) && ( (stopTime < SIMTIME_ZERO) || (time < stopTime) );
}

/**
 * Saves stats and cleans up.
 */
void IPBaseApp::finish()
{
    recordScalar("packets sent", numSent);
    recordScalar("packets received", numReceived);
    ApplicationBase::finish();
}

/**
 * Registers the protocol belonging to the passed IANA protocol number
 * at the MessageDispatcher connected to the gate "ipOut". This is
 * necessary for receiving IP packets with the corresponding number.
 */
void IPBaseApp::registerIPProtocol(int proto)
{
    registerProtocol(*ProtocolGroup::ipprotocol.getProtocol(proto), gate("ipOut"));
}


/**
 * Sends a packet to the given destination node via IP.
 * @param   pkt         Pointer to the payload of the packet (size and custom parameters should be set in advance).
 * @param   destAddr    Network protocol address of the packet's destination.
 * @param   proto       Protocol number of the used transport protocol.
 *
 * FIXME: Rather duplicate passed cPacket on heap to avoid stack use after free.
 */
void IPBaseApp::sendPacket(cPacket *pkt, L3Address destAddr, int proto)
{
    // Name is chosen automatically
    char msgName[34];
    sprintf(msgName, "ipAppData-%d", numSent);
    pkt->setName(msgName);

    IL3AddressType *addressType = destAddr.getAddressType();
    INetworkProtocolControlInfo *controlInfo = addressType->createNetworkProtocolControlInfo();
    //controlInfo->setSourceAddress();
    controlInfo->setDestinationAddress(destAddr);
    if (proto == -1)
        controlInfo->setTransportProtocol(protocol);
    else
        controlInfo->setTransportProtocol(proto);
    pkt->setControlInfo(check_and_cast<cObject *>(controlInfo));

    EV_INFO << "Sending packet: " << pkt->getFullName() << std::endl;
    //printPacket(pkt);
    emit(sentPkSignal, pkt);
    send(pkt, "ipOut");
    numSent++;
}


bool IPBaseApp::handleNodeStart(IDoneCallback *doneCallback)
{
    DestinationParser::parseDestinations(par("destAddresses"), destAddresses);

    simtime_t start = std::max(startTime, simTime());
    if ((stopTime < SIMTIME_ZERO) || (start < stopTime) || (start == stopTime && startTime == stopTime)) {
        startStopMessage->setKind(START);
        scheduleAt(start, startStopMessage);
    }
    return true;
}


// Other classes rely on this exact layout
void IPBaseApp::handleMessageWhenUp(cMessage *msg)
{
    // Handle own self messages
    if (msg == startStopMessage) {

        switch(msg->getKind())
        {
        case BaseSelfMessageKinds::START:
            // Call start function
            processStart();

            // Schedule stop if set
            if (startTime < stopTime && simTime() <= stopTime)
            {
                startStopMessage->setKind(BaseSelfMessageKinds::STOP);
                scheduleAt(stopTime, startStopMessage);
            }
            break;

        case BaseSelfMessageKinds::STOP:
            // Call start function
            processStop();
            break;
        }
    }
    else if (!msg->isSelfMessage()) {
        processPacket(PK(msg));
    }
}


void IPBaseApp::processPacket(cPacket *msg)
{
    emit(rcvdPkSignal, msg);
    EV_INFO << "Received packet with info: " << msg->detailedInfo() << endl;
    delete msg;
    numReceived++;
}

