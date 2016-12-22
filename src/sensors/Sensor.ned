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

package inet_ncs.sensors;

import inet_ncs.IPBaseApp;

//
// A basic sensor application receiving signals of a physical process
// (i.e. the actuating nodes) 
//
simple Sensor extends IPBaseApp
{
    parameters:
        double samplingPeriod @unit("s") = default(2s); // Initial sampling period

		string controllerAddresses = default("");
		string actuatorAddresses = default("");

        int sensorDataLength @unit("B"); // Size of IP payload containing sample and app protocol
        
        int sensorDataProtocol;  // Protocol to send sensor data to a controller
    	int sensorControlProtocol; // Protocol to receive control commands from a controller
        
        @signal[newSensorSample](type=double);
        @statistic[sensorSample](title="sample value"; source=newSensorSample; record=count,vector,stats; interpolationmode=sample-hold);
        @class(Sensor);
    gates:
}

