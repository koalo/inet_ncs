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

#include <stddef.h>
#include <string>
#include <vector>

#include <omnetpp.h>

#include "ModuleArrayTokenBuilder.h"

using namespace omnetpp;


ModuleArrayTokenBuilder::ModuleArrayTokenBuilder(const std::vector<std::string>& strBitsRef, const std::vector<std::pair<unsigned int, unsigned int>>& numBitsRef, size_t sup) : strBits(strBitsRef), numBits(numBitsRef)
{
    if (strBits.size() > 0 && strBits.size()-1 != numBits.size()) throw cRuntimeError("ERROR: Vector sizes do not match the requirements!");

    curVal.reserve(numBits.size());
    maxEntries=1;
    curEntries=0;

    // Calculate sizes (string, number of entries) from supplied vectors
    singleEntrySizeSupremum = 0;
    for(unsigned int i=0;i<numBits.size();i++)
    {
        maxEntries *= numBits.at(i).second - numBits.at(i).first + 1;
        curVal.push_back(numBits.at(i).first);

        singleEntrySizeSupremum += strBits.at(i).length();
    }
    singleEntrySizeSupremum += strBits.at(strBits.size()-1).length();
}

// The resolved string is created on the fly
bool ModuleArrayTokenBuilder::nextToken(std::string& strToken) {

    if (isEnd) return false;

    strToken.clear();
    strToken.reserve(singleEntrySizeSupremum);

    // Append parts of token
    for(unsigned int i=0; i<numBits.size(); i++)
    {
        strToken.append(strBits.at(i));
        strToken.append(std::to_string(curVal.at(i)));
    }
    strToken.append(strBits.at(strBits.size() - 1));

    incrementModuleIndex();

    return true;
};

void ModuleArrayTokenBuilder::incrementModuleIndex() {

    if (isEnd) return;

    // Increment the value
    for(int c=numBits.size()-1;c>=0;c--) {
        if (curVal.at(c) + 1 > numBits.at(c).second) { //>=
            curVal.at(c) = numBits.at(c).first; // reset value
        } else {
            curVal.at(c)++;
            break;
        }
    }
    curEntries++;

    if (curEntries == maxEntries)
        isEnd = true;
}
