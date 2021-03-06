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

package inet_ncs;

import inet_ncs.IPBaseApp;

//
// Controller is a simple controller with sample values input and On/Off-Command output.
//
module Controller extends IPBaseApp
{
    parameters:
        string actuatorAddresses = default("");
        string sensorAddresses = default("");
       
        // Packet lengths
        
        int sensorControlLength @unit("B");   // Size of IP payload containing command and app protocol
        int actuatorControlLength @unit("B"); // Size of IP payload containing command and app protocol
        int actuatorStatusRequestLength @unit("B");  // Size of IP payload containing status request and app protocol
        
        // values for ~IPv4ControlInfo / ~IPv6ControlInfo protocol fields
        int sensorDataProtocol;      // Protocol number used for sensor sample transmission // INCOMING
        int sensorControlProtocol;   // Protocol number used for changing sampling periods // OUTGOING
        int actuatorControlProtocol; // Protocol number used for commands to the actuators // OUTGOING
        int actuatorStatusProtocol;  // IN/OUT

        // Signal Generator
        string signalType = default("sine");
        double signalMax;
        double signalFrequency @unit("Hz") = default(0.01 Hz);
		
        
        @signal[referenceChange];//(type=double);
        @signal[mappedControllerOutput];//(type=int);
        @signal[numProbablyActuated];//(type=int);
        
        @statistic[referenceChangeStat](title="reference value"; source=referenceChange; record=count,vector,stats; interpolationmode=sample-hold);
        @statistic[continuousControllerOutputStat](title="continuous controller output"; source=continuousControllerOutput; record=count,vector,stats; interpolationmode=sample-hold);
        @statistic[mappedControllerOutputStat](title="mapped controller output"; source=mappedControllerOutput; record=count,vector,stats; interpolationmode=sample-hold);
        @statistic[numProbablyActuatedStat](title="actually controlled actuators"; source=numProbablyActuated; record=count,vector,stats; interpolationmode=sample-hold);
        
        @class(Controller); // results in crash
        
    submodules:
        controller : <> like IController {
            parameters:
                @display("p=300,300,row,100");
        }
        
}

