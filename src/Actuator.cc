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

#include <controldefs.h>
#include <algorithm>
#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/contract/INetworkProtocolControlInfo.h"

#include "DestinationParser.h"

#include "Actuator.h"

using namespace inet;

Define_Module(Actuator);

simsignal_t Actuator::changeActiveSignal = registerSignal("changeActive");


Actuator::Actuator() {}
Actuator::~Actuator() {
    cancelAndDelete(selfMessageActuation);
    cancelAndDelete(selfMessageStatus);
}


void Actuator::initialize(int stage)
{
    // To use start and stop timers
    IPBaseApp::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {

        actuatorStatusReplyLength = par("actuatorStatusReplyLength").longValue();

        actuatorControlProtocol = par("actuatorControlProtocol");
        actuatorStatusProtocol = par("actuatorStatusProtocol");

        selfMessageActuation = new cMessage("selfMessageActuation");
        selfMessageStatus = new cMessage("selfMessageStatus");

        currentAct = ActuationCommand::NONE;
        nextAct = ActuationCommand::NONE;

    } else if (stage == INITSTAGE_TRANSPORT_LAYER) {
        registerIPProtocol(actuatorControlProtocol);
        registerIPProtocol(actuatorStatusProtocol);

    } else if(stage == INITSTAGE_APPLICATION_LAYER) {
        // see handleNodeStart
    }
}

void Actuator::finish()
{
    IPBaseApp::finish();
}

// This function's layout is a bit tricky, it should only be changed if everything is understood
void Actuator::handleMessageWhenUp(cMessage *msg)
{
    if(msg == selfMessageActuation) {

        // TODO: save actuation command in kind instead of extra variable
        switch(msg->getKind()) {

        case selfMessageKinds::CHANGE_ACTIVATION:
            if (nextAct != currentAct) {
                // FIXME: Problems might occur if a fast reverting command is sent :S still same state but signal?
                emit(changeActiveSignal, (nextAct == ActuationCommand::ON));
                EV_INFO << "Finished actuation! New state is: " << ((nextAct == ActuationCommand::ON)? "ON" : "OFF") << std::endl;
                currentAct = nextAct;
            } else {
                EV_INFO << "Actuation state still is: " << ((nextAct == ActuationCommand::ON)? "ON" : "OFF") << std::endl;
            }

            break;
        }

    } else if (msg == selfMessageStatus) {
        cPacket *statusPkt = new cPacket("statusReply", statusMessageType::ACTUATOR_STATUS_REPORT, actuatorStatusReplyLength<<3);
        IPBaseApp::sendPacket(statusPkt, statusReplyReceiver, actuatorStatusProtocol);

    } else if (!msg->isSelfMessage()) {
        processPacket(PK(msg));
    } else {
        IPBaseApp::handleMessageWhenUp(msg);
    }
}


bool Actuator::handleNodeStart(IDoneCallback *doneCallback)
{
    // Resolve module paths to apps into cComponent* pointers
    // to compare received messages' senders to.

    const char *controllerAddresses = par("controllerAddresses");
    std::vector<cComponent *> components;
    DestinationParser::parseDestinations(controllerAddresses, components, getSimulation());

    auto it = components.begin();
    while(it != components.end()) {
        EV_INFO << (*it)->getFullPath() << std::endl;
        ++it;
    }

    return true;
}

// Process external packets
void Actuator::processPacket(cPacket *pkt)
{
    INetworkProtocolControlInfo *ctrl = dynamic_cast<INetworkProtocolControlInfo *>(pkt->getControlInfo());
    L3Address l3Sender = ctrl->getSourceAddress();

    EV << "Owner module path: " << pkt->getOwner()->getFullPath() << std::endl;

    // If an array of Controllers has been specified only those are allowed to send actuation commands
    // Else any command is accepted
    bool valid = true;
    if (validControllers.size() > 0)
        valid = std::find(validControllers.begin(), validControllers.end(), l3Sender) != validControllers.end();

    if (!valid) {
        // This would not count in the statistic -- TODO: is that sensible!? Not really...
        EV_INFO << "Packet not for me...";
        delete pkt;
        return;
    }

    // Process content
    cArray& parms = pkt->getParList();

    int controlMsgIdx = parms.find("controlMsgType");
    int statusMsgIdx = parms.find("statusMsgType");

    if (controlMsgIdx != -1) {

        controlMessageType cmt = (controlMessageType)  ((cMsgPar *)parms.get(controlMsgIdx))->operator unsigned char();

        switch (cmt) {
        case controlMessageType::CHANGE_ACTIVATION:
        {
            EV_INFO << "Received control packet: " << pkt->getName() << std::endl;
            ActuationCommand recvAct = (ActuationCommand)pkt->par("actuationCommand").operator unsigned char();
            handleChangeActivation(l3Sender, recvAct);
            break;
        }

        default:
            EV_INFO << "ERROR: Received unknown control packet";
        }


    } else if (statusMsgIdx != -1) {
        statusMessageType smt = (statusMessageType)  ((cMsgPar *)parms.get(statusMsgIdx))->operator unsigned char();

        switch (smt) {

          case statusMessageType::ACTUATOR_STATUS_REQUEST:
              {
                  if (!selfMessageStatus->isScheduled()) {
                      statusReplyReceiver = l3Sender;
                      scheduleAt(simTime() + par("statusReplyDelay").doubleValue(), selfMessageStatus);
                  }
              }
              break;

          default:
              EV_INFO << "ERROR: Received unknown status packet";
          }


    }

    // Emit received signal and delete msg
    IPBaseApp::processPacket(pkt);
}

void Actuator::handleChangeActivation(L3Address sender, ActuationCommand recvAct)
{
    // Time to schedule actuation end
    simtime_t actuateTime = simTime() + par("actuationDelay").doubleValue();

    // If changing state at the moment.
    if (selfMessageActuation->isScheduled()) {

        // If current actuation equals received
        if (nextAct == recvAct) {
            return;
        }

        if (currentAct == recvAct) {
            // FIXME: If undone, will a signal be emitted?
            // Undo change till now (Only sensible for On/Off Acuators)
            // Go back to state before calculate actuation time reverse

            simtime_t nextActArrival = selfMessageActuation->getArrivalTime();
            // nextActReceived must have been initialized since a selfmessage is running
            actuateTime = simTime() + (nextActArrival - nextActReceived) - (nextActArrival - simTime());
        }

    } else {
        // If not scheduled but received state is equals current
        if (currentAct == recvAct) {
            // Do nothing
            return;
        }
    }


    if (!IPBaseApp::enabledAppSimtime(actuateTime)) {
        EV_WARN << "App is not running/enabled!" << std::endl;
        return;
    }

    cancelEvent(selfMessageActuation); // Won't do anything if not scheduled
    selfMessageActuation->setKind(selfMessageKinds::CHANGE_ACTIVATION);
    nextAct = recvAct;
    nextActReceived = simTime();
    scheduleAt(actuateTime, selfMessageActuation);
}
