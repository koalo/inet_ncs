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

#ifndef PRIMITIVELRUACTUATORSTATE_H_
#define PRIMITIVELRUACTUATORSTATE_H_

#include <controldefs.h>
#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/L3Address.h"

// Implements a State that is ordered in a LRU-fashion
class PrimitiveLRUActuatorState {
public:
    PrimitiveLRUActuatorState(inet::L3Address actuatorAddress);
    PrimitiveLRUActuatorState(inet::L3Address actuatorAddress, ActuationCommand lastCmd, simtime_t lastSent);
    virtual ~PrimitiveLRUActuatorState();

    static bool compareStateForActive(const PrimitiveLRUActuatorState& a, const PrimitiveLRUActuatorState& b);
    static bool compareStateForInactive(const PrimitiveLRUActuatorState& a, const PrimitiveLRUActuatorState& b);

    // An activity is pending the Controller should not send further commands to this Actuator to avoid confusion
    simtime_t getLastActuationTime() { return lastActuationTime; };
    virtual void updateSentCommand(simtime_t time, ActuationCommand sentCmd);

    // Last actuation command sent On/Off
    ActuationCommand getLastActuationCommand() {return lastActuationCommand;};
    // Last actuation command sent On/Off
    void setLastActuationCommand(ActuationCommand cmd) {lastActuationCommand = cmd;};

    inet::L3Address getActuatorAddress() {return actuatorAddress;};
    void setActuatorAddress(inet::L3Address addr) {actuatorAddress = addr;};

protected:

    ActuationCommand lastActuationCommand = ActuationCommand::NONE;
    simtime_t lastActuationTime = SIMTIME_ZERO;
    inet::L3Address actuatorAddress;
};

// Comparators

// For active entities
struct PrimitiveLRUActuatorStateActivePtrComparator {
    bool operator()(PrimitiveLRUActuatorState *a, PrimitiveLRUActuatorState *b)
    {
        return PrimitiveLRUActuatorState::compareStateForActive(*a, *b);
    }
};

struct PrimitiveActuatorStateActiveRefComparator {
    bool operator()(PrimitiveLRUActuatorState& a, PrimitiveLRUActuatorState& b)
    {
        return PrimitiveLRUActuatorState::compareStateForActive(a, b);
    }
};



// INACTIVE

struct PrimitiveLRUActuatorStateInactiveRefComparator {
    bool operator()(PrimitiveLRUActuatorState& a, PrimitiveLRUActuatorState& b)
    {
        return PrimitiveLRUActuatorState::compareStateForInactive(a, b);
    }
};

struct PrimitiveLRUActuatorStateInactivePtrComparator {
    bool operator()(PrimitiveLRUActuatorState *a, PrimitiveLRUActuatorState *b)
    {
        return PrimitiveLRUActuatorState::compareStateForInactive(*a, *b);
    }
};

#endif /* PRIMITIVELRUACTUATORSTATE_H_ */
