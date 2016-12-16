//
// Copyright (C) 2016 Jonas K!
// Copyright (C) 2004 Andras Varga
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

package inet_ncs;

import inet.applications.contract.IIPApp;

//
// This template application will connect to the transport layer
// message dispatcher and register a given protocol.
// It is a stripped down version of IPvXTrafGen to provide a more
// generic starting point for developing direct IP applications.
//
simple IPBaseApp like IIPApp
{
    parameters:
        int protocol = default(-1); // value for ~IPv4ControlInfo / ~IPv6ControlInfo protocol field // Not used if -1
        double startTime @unit("s") = default(1s); // time of sending the first packet
        double stopTime @unit("s") = default(-1s);  // time of finishing sending, negative values mean forever
        string destAddresses = default(""); // list of destination addresses, separated by spaces // no need to use this in derived classes
        
        @signal[sentPk](type=cPacket);
        @signal[rcvdPk](type=cPacket);
        @statistic[rcvdPk](title="packets received"; source=rcvdPk; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
        @statistic[sentPk](title="packets sent"; source=sentPk; record=count,"sum(packetBytes)","vector(packetBytes)"; interpolationmode=none);
		//@statistic[endToEndDelay](title="end-to-end delay"; source="messageAge(rcvdPk)"; unit=s; record=histogram,vector; interpolationmode=none);
		@class(IPBaseApp);
    gates:
        input ipIn @labels(IPv4ControlInfo/up,IPv6ControlInfo/up);
        output ipOut @labels(IPv4ControlInfo/down,IPv6ControlInfo/down);
            
}