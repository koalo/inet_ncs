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


#include <controldefs.h>
#include <Sensor.h>
#include <string>
#include <algorithm>
#include "DestinationParser.h"



using namespace inet;


Define_Module(Sensor);

simsignal_t Sensor::newSensorSampleSignal = registerSignal("newSensorSample");
simsignal_t Sensor::changeActiveSignal = registerSignal("changeActive");

Sensor::Sensor() {}
Sensor::~Sensor() {
    cancelAndDelete(selfMessage);
}

double Sensor::getSample()
{
    return curPlantValue;
}

void Sensor::initialize(int stage)
{
    IPBaseApp::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {

        samplingPeriod = par("samplingPeriod").doubleValue();
        sensorDataLength = par("sensorDataLength").longValue();

        sensorDataProtocol = par("sensorDataProtocol");
        sensorControlProtocol = par("sensorControlProtocol");

        selfMessage = new cMessage("selfMessage");

    } else if(stage == INITSTAGE_TRANSPORT_LAYER) {

        // Register transport protocols to receive
        registerIPProtocol(sensorControlProtocol);

    } else if(stage == INITSTAGE_APPLICATION_LAYER) {
        // see handleNodeStart
    }
}

void Sensor::finish()
{
    EV << "Save stats!";
}

bool Sensor::handleNodeStart(IDoneCallback *doneCallback)
{
    // Parse destination addresses for samples
    const char *controllerAddresses = par("controllerAddresses");
    DestinationParser::parseDestinations(controllerAddresses, controlNodes);

    // Subscribe to the actuators' signals which represent a change in the physical plant
    if (!controlNodes.empty()) {
        EV_WARN << "No controller destination address found! This control loop won't work!";
    }

    // Subscribe to the actuators' signals which represent a change in the physical plant
    const char *actuatorAddresses = par("actuatorAddresses");
    DestinationParser::parseDestinations(actuatorAddresses, inputNodes, this->getSimulation());

    if (!inputNodes.empty()) {
        EV_WARN << "No actuator filtering!" << std::endl;
    }

    subscribeToActiveChangeSignals();

    // Schedule app start timer (for processStart and processStop methods)
    return IPBaseApp::handleNodeStart(doneCallback);
}

// This function's layout is a bit tricky, it should only be changed if everything is understood
void Sensor::handleMessageWhenUp(cMessage *msg)
{
    if(selfMessage == msg) {

        switch(selfMessage->getKind()) {
        case SAMPLE:
            sampleAndSend();
            scheduleNextSample();
            break;

        case RESUME:
            // Set active for signal handling
            // Start sampling
            selfMessage->setKind(SAMPLE);
            scheduleNextSample();
            break;

        case PAUSE:
            // Do nothing at all
            break;
        }

    } else if (!msg->isSelfMessage()) {
        processPacket(PK(msg));
    } else {
        IPBaseApp::handleMessageWhenUp(msg);
    }
}


void Sensor::processStart()
{
    selfMessage->setKind(SelfMsgKinds::SAMPLE);
    scheduleNextSample();
}

void Sensor::processStop()
{

}


void Sensor::sampleAndSend()
{
    emit(newSensorSampleSignal, curPlantValue);

    auto iter = controlNodes.begin();

    while (iter != controlNodes.end()) {

        cPacket *payload = new cPacket();
        payload->setByteLength(sensorDataLength);

        payload->addPar("sensorMsgType") = sensorMessageType::NEW_SAMPLE;
        payload->addPar("sampleValue") = curPlantValue;

        sendPacket(payload, *iter, sensorDataProtocol);
        iter++;
    }
}

void Sensor::scheduleNextSample()
{
    simtime_t nextTime = simTime() + samplingPeriod;
    if (stopTime < SIMTIME_ZERO || nextTime < stopTime)
    {
        //selfMessage->setKind(SelfMsgKinds::SAMPLE); // Make sure it's set as SAMPLE
        scheduleAt(nextTime, selfMessage); // still kind == sample
    }
}

// Process external packets
void Sensor::processPacket(cPacket *pkt)
{
    short kind;

    switch(pkt->getKind()) {

    case sensorControlMessageType::PAUSE:
        kind = selfMessage->getKind();

        if (kind == SelfMsgKinds::SAMPLE) {
            // remove message from schedule, if scheduled
            cancelEvent(selfMessage);
            selfMessage->setKind(SelfMsgKinds::PAUSE);
            scheduleAt(simTime(), selfMessage);
        }
        break;

    case sensorControlMessageType::RESUME:
        kind = selfMessage->getKind();

        // If sensor is in pause (or is going to be paused -- should not occur)
        if (kind == SelfMsgKinds::PAUSE) {
            cancelEvent(selfMessage);
            selfMessage->setKind(SelfMsgKinds::SAMPLE);
            scheduleNextSample();
        }
        break;

    case sensorControlMessageType::CHANGE_SAMPLING_PERIOD:
        // New sampling period will be active after the next pending sample
        samplingPeriod = SimTime(pkt->par("newSamplingPeriod").doubleValue()); // simtime_t as param
        break;

    }

    // Increment stats and delete message
    IPBaseApp::processPacket(pkt);
}

/*
 * Signals are static and all signals with the same name are combined into
 * one big callback pool. The callback function may then check the source
 * component to distinguish callers.
 */
void Sensor::subscribeToActiveChangeSignals()
{
    // TODO: Could also just subscribe to the resolved actuators' modules -> performance might increase,
    //       since checking for valid actuator becomes unnecessary
    if (0) {
        getSimulation()->getSystemModule()->subscribe(changeActiveSignal, this); // subscribe systemwide

    } else {

        auto it = inputNodes.begin();
        while(it != inputNodes.end()) {
            (*it)->subscribe(changeActiveSignal, this);
            it++;
        }
    }
}

void Sensor::unsupportedType(simsignal_t signalID, const char *dataType)
{
    EV_ERROR << "Error: Cannot handle datatype. Not supported.";
    throw cRuntimeError("Error: Cannot handle datatype. Not supported.");
}

void Sensor::receiveSignal(cComponent *source, simsignal_t signalID, bool b, cObject *details)
{
    EV_INFO << "Sensor received a signal from \"" << source->getFullName() << "\" with parent \"" << source->getParentModule()->getFullName() << "\" and path " << source->getParentModule()->getFullPath() << std::endl;

    if (0) {
        // Check if sender valid. ALTERNATIVE: register listener only at those modules
        bool isFromValidNode = true;
        if (!inputNodes.empty())
            isFromValidNode = std::find(inputNodes.begin(), inputNodes.end(), source) != inputNodes.end();

        if (!isFromValidNode) {
            EV_WARN << "This Signal from <" << source->getFullPath() << "> will be ignored" << std::endl;
        }
    }

    // To avoid redundancy make sure this signal is only emitted by actuators if they really have changed their state
    // Else we would need to keep track of every actuator state
    if (b)
        curPlantValue = curPlantValue + 1;
    else
        curPlantValue = curPlantValue - 1;

    EV_INFO << "The controlled value is now at: " << std::to_string(curPlantValue) << std::endl;
}
