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

package inet_ncs.controllers;

import inet_ncs.IController;

simple OPEN_LOOP like IController
{
    parameters:      
        @display("i=block/app");
        
        @signal[controllerOutput];//(type=double);
        @signal[controllerError];//(type=double);
        
        @statistic[controllerOutput](title="controller output"; source=controllerOutput; record=count,vector,stats; interpolationmode=sample-hold);
        @statistic[controllerError](title="controller error"; source=controllerError; record=count,vector,stats; interpolationmode=sample-hold);
        
        @class(OPEN_LOOP);
}
