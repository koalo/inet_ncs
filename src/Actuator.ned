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

package inet_ncs;

import inet_ncs.IPBaseApp;

//
// Actuator is a simple On/Off-Device. 
//
simple Actuator extends IPBaseApp
{
    parameters:
		bool activeAtStart = default(false);
		
		string controllerAddresses = default("");  // Module paths of ipApp controllers to filter incoming messages.
												   //   If none passed all messages will be accepted.
        int actuatorStatusReplyLength @unit("B");  // Size of IP payload containing status reply and app protocol
		
		int actuatorControlProtocol;
        int actuatorStatusProtocol;
        
        volatile double actuationDelay @unit("s") = default(1s); // Time the actuation change takes (turning motor)
        volatile double statusReplyDelay @unit("s") = default(0s); // Processing time of status reply
		
        @signal[changeActive](type=bool);
        @statistic[changeState](title="actuator state"; source=changeActive; record=count,vector,stats; interpolationmode=sample-hold);
        @class(Actuator);
    gates:
}

