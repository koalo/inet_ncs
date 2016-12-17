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

#include "SignalGenerator.h"
#include "inet/common/INETDefs.h"

using namespace inet;

SignalGenerator::SignalGenerator() {}
SignalGenerator::~SignalGenerator() {
}

void SignalGenerator::initialize(int stage) {
    if (stage == INITSTAGE_LOCAL) {

    }
}

double SignalGenerator::getCurrentValue() {
    return -cos(simTime().dbl()/15)*20+20;
}
