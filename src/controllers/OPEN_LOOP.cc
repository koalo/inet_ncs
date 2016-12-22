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

#include <omnetpp.h>

#include "inet/common/INETDefs.h"
#include "inet/common/InitStages.h"
#include <controllers/OPEN_LOOP.h>

using namespace inet;

Define_Module(OPEN_LOOP);


OPEN_LOOP::OPEN_LOOP() {}
OPEN_LOOP::~OPEN_LOOP() {}


void OPEN_LOOP::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    if (stage == INITSTAGE_LOCAL) {

    } else if (stage == INITSTAGE_APPLICATION_LAYER) {
        // Almost same as handleNodeStart
    }
}

double OPEN_LOOP::processSample(double sampleValue, double referenceValue)
{
    // Open Loop: no feedback via sampleValue
    emit(outputSignal, referenceValue);
    return referenceValue;
}
