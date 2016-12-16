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

#ifndef CONTROLDEFS_H_
#define CONTROLDEFS_H_

// Controller -> On/Off-Actuator
typedef enum : unsigned char {
    CHANGE_ACTIVATION
} controlMessageType;

// Sensor -> Controller
typedef enum : unsigned char {
    NEW_SAMPLE
} sensorMessageType;

// Controller -> Sensor
typedef enum : unsigned char {
    PAUSE,
    RESUME,
    CHANGE_SAMPLING_PERIOD
} sensorControlMessageType;

typedef enum : unsigned char {
    ACTUATOR_STATUS_REPORT,
    ACTUATOR_STATUS_REQUEST,

    SENSOR_STATUS_REPORT,
    SENSOR_STATUS_REQUEST
} statusMessageType;


// Types of states the actuator is assumed to be in
typedef enum : unsigned char {
    UNKNOWN = 0,
    ACTIVE,
    INACTIVE
} EstimatedActuatorState;

//
typedef enum : unsigned char {
    NONE = 0,
    ON,
    OFF
} ActuationCommand;

#endif /* CONTROLDEFS_H_ */
