//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004,2011 Andras Varga
// Copyright (C) 2016 Jonas K. <i-tek@web.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <list>
#include <vector>
#include <string>

#include <omnetpp.h>
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"

#include "ModuleArrayTokenParser.h"

#include <DestinationParser.h>

using namespace inet;

DestinationParser::DestinationParser() {}
DestinationParser::~DestinationParser() {}

// Repeatedly expands modulenames with array definition and generates a vector of network layer addresses
void DestinationParser::parseDestinations(const char* destAddresses, std::vector<L3Address>& vAddresses)
{
    EV_STATICCONTEXT;
    vAddresses.clear();
    cStringTokenizer tokenizer(destAddresses);
    std::list<std::string> lstTokens;

    // Insert into working list
    while (tokenizer.hasMoreTokens()) {
        lstTokens.push_back(std::string(tokenizer.nextToken()));
    }

    // Resolve and insert addresses
    auto token = lstTokens.begin();
    while (token != lstTokens.end()) {

        const char *ctok = token->c_str();

         // If the module path contains array definitions -> expand else return false and resolve L3Address
         if (!ModuleArrayTokenParser::resolveModulePathArrayToken(ctok, lstTokens)) {

             L3Address result;
             L3AddressResolver().tryResolve(ctok, result);
             if (result.isUnspecified())
                 EV_WARN << "cannot resolve destination address: " << *token << std::endl;
             else
                 vAddresses.push_back(result);
         }

         token++;
         lstTokens.pop_front(); // Remove handled list entry

     } // end while
}

// Repeatedly expands modulenames with array definition
// Little helper to maintain a list of pointer for comparison instead of textual comparison at simulation time
void DestinationParser::parseDestinations(const char* componentModules, std::vector<cComponent *>& vComps,
        cSimulation *simulation, bool resolveToParent)
{
    EV_STATICCONTEXT;
    vComps.clear();
    cStringTokenizer tokenizer(componentModules);
    std::list<std::string> lstTokens;

    // Insert into working list
    while (tokenizer.hasMoreTokens()) {
        lstTokens.push_back(std::string(tokenizer.nextToken()));
    }

    // Resolve and insert addresses
    auto token = lstTokens.begin();
    while (token != lstTokens.end()) {

        const char *ctok = token->c_str();

         // If the module path contains array definitions -> expand else return false and resolve L3Address
         if (!ModuleArrayTokenParser::resolveModulePathArrayToken(ctok, lstTokens)) {

             cModule *comp;
             if (resolveToParent)
                 comp = simulation->getSystemModule()->getModuleByPath(ctok)->getParentModule();
             else
                 comp = simulation->getSystemModule()->getModuleByPath(ctok);

             if (comp != nullptr) {
                 EV_INFO << "Resolved module path for \"" << ctok << "\" is: " << comp->getFullPath() << std::endl;
                 vComps.push_back(comp); // cast
             }
         }

         token++;
         lstTokens.pop_front(); // Remove handled list entry

    } // end while
}
