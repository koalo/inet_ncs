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

simple PID like IController
{
    parameters:
        double proportionalGain = default(1.0); // Network Control System Proportional Gain
        double integralGain 	= default(0.0); // Network Control System Integral Gain
        double derivativeGain 	= default(0.0); // Network Control System Derivative Gain
        double integratorMax    = default(-1); // Network Control System Anti-WindUp (negative to disable)
        
        @signal[controllerOutput];//(type=double);
        @signal[controllerError];//(type=double);
        @signal[controllerP];//(type=double);
        @signal[controllerI];//(type=double);
        @signal[controllerD];//(type=double);
        
        @statistic[controllerOutput](title="controller output"; source=controllerOutput; record=count,vector,stats; interpolationmode=sample-hold);
        @statistic[controllerError](title="controller error"; source=controllerError; record=count,vector,stats; interpolationmode=sample-hold);
        @statistic[controllerP](title="proportional output"; source=controllerP; record=count,vector,stats; interpolationmode=sample-hold);
        @statistic[controllerI](title="integral output"; source=controllerI; record=count,vector,stats; interpolationmode=sample-hold);
        @statistic[controllerD](title="derivative output"; source=controllerD; record=count,vector,stats; interpolationmode=sample-hold);
        
        @display("i=block/app");
        @class(PID);
}
