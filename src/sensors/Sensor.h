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


#ifndef SENSOR_H_
#define SENSOR_H_

#include <omnetpp.h>
#include "inet/common/INETDefs.h"
#include "IPBaseApp.h"

class Sensor : public IPBaseApp, public cListener {

private:
    double curPlantValue;

    static simsignal_t newSensorSampleSignal;
    static simsignal_t changeActiveSignal;

    cMessage *selfMessage = nullptr;

    simtime_t samplingPeriod;

    // IP Payload Lengths
    long sensorDataLength;

    int sensorDataProtocol;
    int sensorControlProtocol;


    std::vector<cComponent *> inputNodes; // Signals from these cComponents are processed
    std::vector<inet::L3Address> controlNodes; // Layer3 Addresses of sample receiving controllers

protected:
    enum SelfMsgKinds { SAMPLE = 1, RESUME, PAUSE };

    // Override IPBaseApp methods
    virtual void initialize(int stage) override;
    virtual void finish() override;
    virtual void processPacket(cPacket *pkt) override;
    virtual bool handleNodeStart(inet::IDoneCallback *doneCallback) override;
    virtual void handleMessageWhenUp(cMessage *msg) override;

    // Override cListener methods
    virtual void unsupportedType(simsignal_t signalID, const char *dataType) override;
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, bool b, cObject *details) override;

    void sampleAndSend();
    void scheduleNextSample();
    void subscribeToActiveChangeSignals();

    virtual void processStart() override;
    virtual void processStop() override;

public:
    double getSample();

    Sensor();
    virtual ~Sensor();
};

#endif /* SENSOR_H_ */
