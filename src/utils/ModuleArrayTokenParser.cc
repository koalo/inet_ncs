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

#include <list>
#include <vector>
#include <string>

#include <omnetpp.h>
#include "inet/networklayer/common/L3Address.h"
#include "inet/networklayer/common/L3AddressResolver.h"

#include "ModuleArrayTokenBuilder.h"

#include <ModuleArrayTokenParser.h>

using namespace inet;

ModuleArrayTokenParser::ModuleArrayTokenParser() {}

/*
 * Appends to a list of module paths from a single module path token with array definitions a la "[3..7]".
 * These expanded tokens then can be resolved by Omnet's L3AddressResolver.
 */
bool ModuleArrayTokenParser::resolveModulePathArrayToken(const char* unexpandedModuleArrayToken, std::list<std::string>& lstExpandedTokens)
{
    EV_STATICCONTEXT;

    std::vector<std::string> strBits;
    std::vector<std::pair<unsigned int, unsigned int>> numBits;
    size_t numExpandedEntries, supEntryStringSize;

    bool neededExpansion = parseModulePathArrayToken(unexpandedModuleArrayToken, strBits, numBits, &numExpandedEntries, &supEntryStringSize);

    // If no array definitions could be found
    if (!neededExpansion) return false;

    std::string curToken;
    ModuleArrayTokenBuilder tb(strBits, numBits, supEntryStringSize);
    while (tb.nextToken(curToken)) {
       lstExpandedTokens.push_back(curToken);
    }

    return true;
}

/*
 * Appends to a vector of module paths from a single module path token with array definitions a la "[3..7]".
 * These expanded tokens then can be resolved by Omnet's L3AddressResolver.
 */
bool ModuleArrayTokenParser::resolveModulePathArrayToken(const char* unexpandedModuleArrayToken, std::vector<std::string>& vExpandedTokens)
{
    EV_STATICCONTEXT;

    std::vector<std::string> strBits;
    std::vector<std::pair<unsigned int, unsigned int>> numBits;
    size_t numExpandedEntries, supEntryStringSize;

    bool neededExpansion = parseModulePathArrayToken(unexpandedModuleArrayToken, strBits, numBits, &numExpandedEntries, &supEntryStringSize);

    // If no array definitions could be found
    if (!neededExpansion) return false;

    std::string curToken;
    ModuleArrayTokenBuilder tb(strBits, numBits, supEntryStringSize);
    while (tb.nextToken(curToken)) {
        vExpandedTokens.push_back(curToken);
    }

    return true;
}

/*
 * Creates a string of concatenated module path definitions from a single module path token with array definitions a la "[3..7]".
 * This expanded token string then can be resolved by Omnet's standard destination parsing routines.
 */
bool ModuleArrayTokenParser::resolveModulePathArrayToken(const char* unexpandedModuleArrayToken, std::string& expandedTokens)
{
    EV_STATICCONTEXT;

    std::vector<std::string> strBits;
    std::vector<std::pair<unsigned int, unsigned int>> numBits;
    size_t numExpandedEntries, supEntryStringSize;

    bool neededExpansion = parseModulePathArrayToken(unexpandedModuleArrayToken, strBits, numBits, &numExpandedEntries, &supEntryStringSize);

    // If no array definitions could be found
    if (!neededExpansion) return false;

    // Reserve string memory (slightly overestimated)
    expandedTokens = "";
    expandedTokens.reserve(expandedTokens.length() + supEntryStringSize * numExpandedEntries);
    EV_INFO << "Reserved a string with estimated length of " << std::to_string(supEntryStringSize * numExpandedEntries) << std::endl;

    std::string curToken;
    ModuleArrayTokenBuilder tb(strBits, numBits, supEntryStringSize);
    while (tb.nextToken(curToken)) {
        expandedTokens.append(curToken);
        expandedTokens.append(" ");
    }

    EV_INFO << "Final string length is " << expandedTokens.length() << std::endl;

    return true;
}


/**
 * Slices a single module path token with array definition(s) into string snippets and array bound definitions
 * to be combined by appending a string snippet and a number from within the bounds in an alternating fashion.
 * Example: `addresses = "hosts[1..2].ipApp[3..4]"`
 * Result: strBits={"hosts[", "].ipApp[", "]"} strBits={(1,2), (3,4)}
 *
 * This method does not test whether the module path is erroneous.
 *
 * Unfortunately, this hack is necessary since omnet does seem not to parse bound definitions :(
 * TODO: Detect upper array bound if left out. Get module array size.
 *
 * @param dirtyHostToken        A single module path token (e.g. "net.host[1..2].ipApp[0]").
 * @param strBits               A reference to a vector to put the tokens string snippets in.
 * @param numBits               A reference to a vector to put the array ranges in.
 * @param numExpandedEntries    A pointer to a variable which will contain the final number of expanded tokens (optional).
 * @param supSingleEntrySize    A pointer to a variable which will contain a supremum for the length of a string concatenating all expanded tokens (optional).
 *
 * @return                      A boolean indicating whether token expansion was necessary.
 *
 */
bool ModuleArrayTokenParser::parseModulePathArrayToken(const char* dirtyHostToken, std::vector<std::string>& strBits,
        std::vector<std::pair<unsigned int, unsigned int>>& numBits, size_t *numExpandedEntries, size_t *supSingleEntrySize)
{
    EV_STATICCONTEXT;

    // First check if expansion is necessary at all
    // --- Excerpt from L3AddressResolver.cc
    int addrType = (L3AddressResolver::ADDR_IPv4 | L3AddressResolver::ADDR_IPv6
            | L3AddressResolver::ADDR_MAC | L3AddressResolver::ADDR_MODULEPATH
            | L3AddressResolver::ADDR_MODULEID);

    // empty address
    L3Address result = L3Address();
    if (!dirtyHostToken || !*dirtyHostToken)
        return false;

    // handle address literal
    if (result.tryParse(dirtyHostToken))
        return false;

    std::string modname, ifname, protocol, destnodename, extension;
    const char *p = dirtyHostToken;
    const char *endp = strchr(p, '\0');
    const char *nextsep = strpbrk(p, "%>(/");
    if (!nextsep)
        nextsep = endp;

    // Extract module name
    modname.assign(p, nextsep - p);
    // --- Excerpt from L3AddressResolver.cc END

    extension.assign(nextsep);


    // Initial check whether it's necessary to process token
    const char *c_modname = modname.c_str();
    const char *sep, *endsep, *after_last_endsep;
    sep = c_modname;

    sep = strpbrk(sep, "[");
    if (!sep)
        return false;

    endsep = strpbrk(sep+1, "]");
    if (!endsep)
        return false;

    const char *dots = strstr(sep+1,"..");
    if (!dots)
        return false;



    // If reached here, at least a single range case (perhaps malformed)

    sep = c_modname;
    after_last_endsep = c_modname; // Attention: do not access after this initialization (segfault possible)
    int nStart, nEnd;

    size_t singleEntrySize = 0; // Used to preallocate final concatenated string
    size_t curEntries = 1;

    strBits.clear();
    numBits.clear();

    // Repeatedly search for range definitions in the dirtyHostToken
    while (true)
    {
        // sep must have been incremented at the end of the loop
        sep = strpbrk(sep, "[");
        // If no array brackets in modulename -> skip
        if (!sep)
            break;

        endsep = strpbrk(sep+1, "]");
        // If no array brackets in modulename -> skip
        if (!endsep)
            break;

        // strstr is unsafe
        const char *dots = strstr(sep+1,"..");

        if (!dots) break;

        // Dots beyond endsep
        if (dots > endsep) {
            sep = endsep+1;
            continue;
        }


        // Extract array bounds
        std::string sTemp;

        if (dots == sep+1) {
            // Content of brackets begins with ..
            nStart = 0;
        } else {
            // There must be a number or error is thrown
            sTemp.assign(sep+1, dots - (sep+1));
            EV_INFO << "THE START NUMBER IS: " + sTemp;
            nStart = std::stoi(sTemp);
        }

        if (dots + 2 == endsep) {
            // Content of brackets ends with ..
            throw cRuntimeError("Auto detection of upper array bound not yet implemented!");
            //nEnd = max module array size
        } else {
            // There must be a number or error is thrown
            sTemp.assign(dots + 2, endsep - (dots+2));
            EV_INFO << "THE END NUMBER IS: " + sTemp;
            nEnd = std::stoi(sTemp);
        }


        // Get module name before and after [a..b]
        std::string sbefore;
        sbefore.assign(after_last_endsep, (sep+1) - after_last_endsep); // sep points to [ but we want [ too -> +1

        strBits.push_back(sbefore);

        if (nEnd < nStart) std::swap<int>(nStart, nEnd);
        numBits.emplace_back(nStart, nEnd);

        if (nEnd - nStart + 1 <= 0) throw cRuntimeError("Invalid array range.");

        // current cycles number of entries * ( before stringlength + Max number length + space)
        singleEntrySize += (sbefore.length() + std::to_string(nEnd).length() + 1);
        curEntries *= (nEnd - nStart + 1);


        after_last_endsep = endsep;
        sep = endsep + 1;

    } // WHILE END


    // If no more append last part and add its size and add extension with size
    std::string sbeyond;
    sbeyond.assign(after_last_endsep);

    sbeyond += extension;
    strBits.push_back(sbeyond);

    singleEntrySize += sbeyond.length();

    if (numExpandedEntries) *numExpandedEntries = curEntries;
    if (supSingleEntrySize) *supSingleEntrySize = singleEntrySize;

    if (numBits.size() != strBits.size() - 1)
        throw cRuntimeError("Expected size of vector differs!");

    return true;
}
