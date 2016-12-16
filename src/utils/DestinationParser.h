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

#ifndef UTILS_DESTINATIONPARSER_H_
#define UTILS_DESTINATIONPARSER_H_

#include <omnetpp.h>
#include "inet/common/INETDefs.h"
#include "inet/networklayer/common/L3Address.h"

class DestinationParser {
public:

    virtual ~DestinationParser();

    static void parseDestinations(const char* destAddrs, std::vector<inet::L3Address>& vAddresses);
    static void parseDestinations(const char* componentModules, std::vector<cComponent *>& vComps,
            cSimulation *simulation, bool resolveToParent=false);

private:
    DestinationParser();
};

#endif /* UTILS_DESTINATIONPARSER_H_ */
