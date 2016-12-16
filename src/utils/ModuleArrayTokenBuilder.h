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

#ifndef UTILS_MODULEARRAYTOKENBUILDER_H_
#define UTILS_MODULEARRAYTOKENBUILDER_H_

#include <stddef.h>
#include <string>
#include <vector>

/*
 * Allows on the fly generation of module tokens. Requires parsed array of tokens.
 */
class ModuleArrayTokenBuilder {
public:
    ModuleArrayTokenBuilder(const std::vector<std::string>& strBitsRef, const std::vector<std::pair<unsigned int, unsigned int>>& numBitsRef, size_t sup = 0);

    bool nextToken(std::string& strToken);

private:
    bool isEnd = false;

    const std::vector<std::string>& strBits;
    const std::vector<std::pair<unsigned int, unsigned int>>& numBits;

    std::vector<unsigned int> curVal;

    unsigned int curEntries = 0;
    unsigned int maxEntries = 0;

    size_t singleEntrySizeSupremum = 0;

private:
    void incrementModuleIndex();

};

#endif /* UTILS_MODULEARRAYTOKENBUILDER_H_ */
