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

#include <PrimitiveLRUActuatorState.h>

PrimitiveLRUActuatorState::~PrimitiveLRUActuatorState() {}

PrimitiveLRUActuatorState::PrimitiveLRUActuatorState(inet::L3Address actuatorAddress) {this->actuatorAddress = actuatorAddress;}
PrimitiveLRUActuatorState::PrimitiveLRUActuatorState(inet::L3Address actuatorAddress, ActuationCommand lastCmd, simtime_t lastSent)
{
    this->actuatorAddress       = actuatorAddress;
    this->lastActuationCommand  = lastCmd;
    this->lastActuationTime     = lastSent;
}


bool PrimitiveLRUActuatorState::compareStateForActive(const PrimitiveLRUActuatorState& a, const PrimitiveLRUActuatorState& b)
{
    if (a.lastActuationCommand != b.lastActuationCommand) {
        if (a.lastActuationCommand == ActuationCommand::ON) return true;
        if (b.lastActuationCommand == ActuationCommand::ON) return false;
    }

    if (a.lastActuationTime < b.lastActuationTime) return true;
    if (a.lastActuationTime > b.lastActuationTime) return false;

    return false;
}

bool PrimitiveLRUActuatorState::compareStateForInactive(const PrimitiveLRUActuatorState& a, const PrimitiveLRUActuatorState& b)
{
    if (a.lastActuationCommand != b.lastActuationCommand) {
        if (a.lastActuationCommand == ActuationCommand::OFF) return true;
        if (b.lastActuationCommand == ActuationCommand::OFF) return false;
    }

    if (a.lastActuationTime < b.lastActuationTime) return true;
    if (a.lastActuationTime > b.lastActuationTime) return false;

    return false;
}


void PrimitiveLRUActuatorState::updateSentCommand(simtime_t time, ActuationCommand sentCmd)
{
    lastActuationCommand = sentCmd;
    lastActuationTime = time;
}
