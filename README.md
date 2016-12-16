# inet_ncs
Provides a basic set of utilities to create network control systems in omnet++ (with inet).

# Caution
This project is heavily WIP. So don't rely on a perfect codebase.

# Usage
Install the inet framework in your omnet workspace from:
git clone -b ieee802154_example https://github.com/i-tek/inet.git
(This is necesary since some minor customizations are required)

Then clone this repo, build everything and run the simulation.


# Remarks
The current state of this project features a basic Network Control System which
has been inspired by a solar power tower where heliostats can be 'either turned on
or turned off'.

The current example implementation of the control circuit looks as follows:
 - Sensor: Measures flux density (by receiving omnet-signals from the heliostats).
 - Controller: Assigns a set of actuators to the states on and off (and sends unicast control commands to them).
 - Actuators: Focus sun light on the sensor (by receiving commands and signalling the impact they have on the plant).
