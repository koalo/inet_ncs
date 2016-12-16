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

#include <algorithm>
#include <controldefs.h>
#include <omnetpp.h>
#include <PrimitiveLRUActuatorState.h>

#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/common/ModuleAccess.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include "DestinationParser.h"

#include "src/Controller.h"


using namespace inet;

Define_Module(Controller);

simsignal_t Controller::referenceChangeSignal = registerSignal("referenceChange");
simsignal_t Controller::continuousControllerOutputSignal = registerSignal("continuousControllerOutput");
simsignal_t Controller::mappedControllerOutputSignal = registerSignal("mappedControllerOutput");
simsignal_t Controller::numProbablyActuatedSignal = registerSignal("numProbablyActuated");

// PID
simsignal_t Controller::continuousControllerPSignal = registerSignal("continuousControllerP");
simsignal_t Controller::continuousControllerISignal = registerSignal("continuousControllerI");
simsignal_t Controller::continuousControllerDSignal = registerSignal("continuousControllerD");


Controller::Controller() {}
Controller::~Controller() {
    cancelAndDelete(selfMessage);
}

void Controller::finish()
{
    IPBaseApp::finish();
}

void Controller::initialize(int stage)
{
    IPBaseApp::initialize(stage); // start stop timer and parameters

    if (stage == INITSTAGE_LOCAL) {

        sensorControlLength = par("sensorControlLength").longValue();
        actuatorControlLength = par("actuatorControlLength").longValue();
        actuatorStatusRequestLength = par("actuatorStatusRequestLength").longValue();

        sensorDataProtocol = par("sensorDataProtocol");
        sensorControlProtocol = par("sensorControlProtocol");
        actuatorControlProtocol = par("actuatorControlProtocol");
        actuatorStatusProtocol = par("actuatorStatusProtocol");

        Kp = par("proportionalGain");
        Ki = par("integralGain");
        Kd = par("derivativeGain");

        selfMessage = new cMessage("selfMessage");

    } else if (stage == INITSTAGE_TRANSPORT_LAYER) {

        EV << "Register protocols: " << sensorDataProtocol << ", " << actuatorStatusProtocol << std::endl;
        registerIPProtocol(sensorDataProtocol);
        registerIPProtocol(actuatorStatusProtocol);

    } else if (stage == INITSTAGE_APPLICATION_LAYER) {
        // Almost same as handleNodeStart
    }
}

bool Controller::handleNodeStart(IDoneCallback *doneCallback)
{
    EV_INFO << "Handle node start" << std::endl;
    std::vector<L3Address> vActAddrs;
    const char *actuatorAddresses = par("actuatorAddresses");
    DestinationParser::parseDestinations(actuatorAddresses, vActAddrs);

    const char *sensorAddresses = par("sensorAddresses");
    DestinationParser::parseDestinations(sensorAddresses, vSensorAddresses);


    // Create Actuator Status Database
    auto aaddr = vActAddrs.begin();
    while ( aaddr != vActAddrs.end() ) {

        EV << "Address <" << aaddr->str() << "> contains an actuating node?" << std::endl;
        PrimitiveLRUActuatorState actState(*aaddr, ActuationCommand::OFF, SIMTIME_ZERO);
        actuatorStates.push_back(actState);
        aaddr++;
    }

    // Set initial reference value to zero at SIMTIME_ZERO
    changeReferenceValue(0.0);

    // Schedule start and stop timers
    IPBaseApp::handleNodeStart(doneCallback);

    return true;
}

// This function's layout is a bit tricky, it should only be changed if everything is understood
void Controller::handleMessageWhenUp(cMessage *msg)
{
    // Handle own selfMessage
    if (msg == selfMessage) {
        // No self messages yet

    } else if (!msg->isSelfMessage()) {
        // Handle external packet first by this class and then by superclass (for removal and stats)
        processPacket(PK(msg));

    } else {
        // msg has to be a self message but not this classes one... perhaps of the superclass
        IPBaseApp::handleMessageWhenUp(msg);
    }
}



void Controller::processStart()
{
    // Handle start timer event
    changeReferenceValue(par("referenceValue").doubleValue());
}


void Controller::processStop()
{
    // Handle stop timer event
}


void Controller::processPacket(cPacket *pkt)
{
    cArray& parms = pkt->getParList();

    // Verification that from correct sensor? -> performance drop?

    int sensorMsgIdx = parms.find("sensorMsgType");
    int controlMsgIdx = parms.find("controlMsgType");

    if (sensorMsgIdx != -1) {
        sensorMessageType stype = (sensorMessageType)((cMsgPar *)parms.get(sensorMsgIdx))->operator unsigned char();

        switch(stype) {
        case sensorMessageType::NEW_SAMPLE:

            double newFlux = pkt->par("sampleValue").doubleValue();
            processSample(newFlux);
            break;
        }

    } else if (controlMsgIdx != -1) {
        controlMessageType ctype = (controlMessageType)((cMsgPar *)parms.get(controlMsgIdx))->operator unsigned char();
    }

    // Signal packetRcvd and delete message
    IPBaseApp::processPacket(pkt);
}


/**
 * This is just a quick and dirty controller with horrible characteristics.
*/
void Controller::processSample(double sample)
{
    static double integrator = 0;
    const double integrator_mag_max = 10;
    currentFlux = sample;

    // Control Action
    double curDelta = referenceValue - currentFlux;

    // Anti-Windup
    if (integrator + curDelta > integrator_mag_max) {
        integrator = integrator_mag_max;
    } else if (integrator + curDelta < -integrator_mag_max) {
        integrator = -integrator_mag_max;
    } else {
        integrator += curDelta;
    }

    // PID-Controller
    double p = Kp * curDelta;
    double i = Ki * integrator;
    double d = Kd * (curDelta - lastFluxDelta);
    double u = p + i + d;
    emit(continuousControllerPSignal, p);
    emit(continuousControllerISignal, i);
    emit(continuousControllerDSignal, d);
    emit(continuousControllerOutputSignal, u);


    // Map controller output to actuator on/off number

    int mapped = static_cast<int>(u);
    int total = actuatorStates.size();

    if (abs(mapped) > total) {
        mapped = (mapped < 0)? -total : total;
    }

    emit(mappedControllerOutputSignal, mapped);

    changeState(mapped);

    // Derivative
    lastFluxDelta = curDelta;
    lastFlux = currentFlux;
}

/**
 * Prints a given list of states in its current order.
 */
void Controller::print_states(std::vector<PrimitiveLRUActuatorState *> destStates)
{
    std::cout << "Print Actuator States:" << std::endl;

    int i=1;
    auto it = destStates.begin();
    while (it != destStates.end()) {

        std::string cmd;
        ActuationCommand a = (*it)->getLastActuationCommand();
        switch(a) {
        case ActuationCommand::NONE:
            cmd = "None";
            break;
        case ActuationCommand::OFF:
            cmd = "Off";
            break;
        case ActuationCommand::ON:
            cmd = "On";
            break;
        default:
            cmd = "UNKNOWN";
        }

        std::cout << std::to_string(i) << ". Command ----------" << std::endl;
        std::cout << " Last command: " << cmd << std::endl;
        std::cout << " Last actuation time: " << (*it)->getLastActuationTime().str() << std::endl;

        it++;i++;
    }
    std::cout << std::endl;
}

/**
 * Unicast to a certain number of actuators.
 */
void Controller::changeState(int changeNum)
{
    unsigned int num = std::abs(changeNum);

    std::vector<PrimitiveLRUActuatorState *> destStates;

    // ugly workaround
    auto it = actuatorStates.begin();
    while (it != actuatorStates.end()) {
        destStates.push_back(&(*it));
        ++it;
    }

    /*
     * Here the actuators are sorted by there currently assumed state
     * and secondly by the last point in time they sent a command.
     * FIXME: If an actuator didn't change state although it is assumed to
     *        and there aren't enough other actuators in the other state left to change
     *        state in order to satisfy the controller (only if the error is large enough)
     *        the 'did not' change actuator will receive a repeated command
     *          -> Prefer random choice of actuators (or pure LRU and not such a mixed version)
     */
    ActuationCommand cmd;
    if (changeNum > 0) {
        // Horrible replacement but stable, other is WIP
        std::stable_sort(destStates.begin(), destStates.end(), PrimitiveLRUActuatorStateInactivePtrComparator());
        std::cout << std::endl << "Turn on some of these:" << std::endl;
        cmd = ActuationCommand::ON;
    } else if (changeNum < 0) {
        // Horrible replacement but stable, other is WIP
        std::stable_sort(destStates.begin(), destStates.end(), PrimitiveLRUActuatorStateActivePtrComparator());
        std::cout << std::endl << "Turn off some of these:" << std::endl;
        cmd = ActuationCommand::OFF;
    } else {
        return;
    }

    //DEBUG: print_states(destStates);

    // workaround: reduce to required size (shouldn't be needed since capped in processSample)
    if (destStates.size() > num)
        destStates.resize(num);

    if (destStates.size() != num)
        EV_ERROR << "Cannot change states of that many actuators!" << std::endl
            << "  Only " << destStates.size() << " Actuators available..." << std::endl;

    // Prepare packet
    cPacket *pkt = new cPacket();
    pkt->addPar("controlMsgType") = controlMessageType::CHANGE_ACTIVATION;
    pkt->addPar("actuationCommand") = cmd;
    pkt->addPar("newState") = (changeNum > 0);
    pkt->setByteLength(actuatorControlLength);

    // FIXME: Potential problem: due to sampling interval a lot of sequential unicast packets might queue up
    // Generate an identical unicast packet for every selected actuator
    auto itm = destStates.begin();
    while (itm != destStates.end())
    {
        cPacket *sPkt = pkt->dup();
        sendPacket(sPkt, (*itm)->getActuatorAddress(), actuatorControlProtocol);
        (*itm)->updateSentCommand(simTime(), cmd);
        itm++;
    }

    if (changeNum >= 0) {
        emit(numProbablyActuatedSignal, destStates.size());
    } else {
        emit(numProbablyActuatedSignal, -static_cast<long>(destStates.size()) );
    }

    delete pkt;
}



/**
 * Updates the control circuit's reference value.
 */
void Controller::changeReferenceValue(double newReference)
{
    referenceValue = newReference;
    emit(referenceChangeSignal, newReference);
}




