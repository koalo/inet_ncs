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

#ifndef CONTROLLERS_OPEN_LOOP_H_
#define CONTROLLERS_OPEN_LOOP_H_

#include <omnetpp.h>
#include "IController.h"

class OPEN_LOOP : public IController, public omnetpp::cSimpleModule {

public:
    OPEN_LOOP();
    virtual ~OPEN_LOOP();

    void initialize(int stage) override;
    double processSample(double sampleValue, double referenceValue);
};

#endif /* CONTROLLERS_OPEN_LOOP_H_ */
