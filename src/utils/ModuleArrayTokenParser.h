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

#ifndef UTILS_MODULEARRAYTOKENPARSER_H_
#define UTILS_MODULEARRAYTOKENPARSER_H_

#include <list>
#include <vector>
#include <string>

#include <omnetpp.h>

class ModuleArrayTokenParser {
public:

    static bool resolveModulePathArrayToken(const char* unexpandedModuleArrayToken, std::list<std::string>& lstExpandedTokens);
    static bool resolveModulePathArrayToken(const char* unexpandedModuleArrayToken, std::vector<std::string>& vExpandedTokens);
    static bool resolveModulePathArrayToken(const char* unexpandedModuleArrayToken, std::string& expandedTokens);

    static bool parseModulePathArrayToken(const char* dirtyHostToken, std::vector<std::string>& strBits,
            std::vector<std::pair<unsigned int, unsigned int>>& numBits, size_t *numExpandedEntries = nullptr, size_t *supSingleEntrySize = nullptr);

private:
    ModuleArrayTokenParser();

};

#endif /* UTILS_MODULEARRAYTOKENPARSER_H_ */
