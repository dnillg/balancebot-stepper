#ifndef SERIALUNITROUTER_HPP_
#define SERIALUNITROUTER_HPP_

#include <Arduino.h>
#include <map>

#include <SerialUnits/SerialUnits.hpp>
#include <BluetoothSerial.h>

class SerialUnitRouter {
    public:
        SerialUnitRouter(
            BluetoothSerial& bluetoothSerial,
            HardwareSerial& controlSerial
        );
        void route(SerialUnitEndpoint source, SerialUnitAlias alias, String message);
    private:
        BluetoothSerial* bluetoothSerial;
        HardwareSerial* controlSerial;
        void send(SerialUnitEndpoint destination, String message);
};

#endif // SERIALUNITROUTER_HPP_