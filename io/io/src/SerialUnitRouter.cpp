#include "SerialUnitRouter.hpp"

std::map<SerialUnitAlias, SerialUnitEndpoint> routingFromBluetooth = {
    {SerialUnitAlias::CTRL, SerialUnitEndpoint::CONTROL},
    {SerialUnitAlias::GETPID, SerialUnitEndpoint::CONTROL},
    {SerialUnitAlias::SETPID, SerialUnitEndpoint::CONTROL},
    {SerialUnitAlias::MOTTOG, SerialUnitEndpoint::CONTROL},
    {SerialUnitAlias::TRIG, SerialUnitEndpoint::CONTROL}
};

std::map<SerialUnitAlias, SerialUnitEndpoint> routingFromControl = {
    {SerialUnitAlias::DIAG, SerialUnitEndpoint::BLUETOOTH},
    {SerialUnitAlias::GETPIDRSP, SerialUnitEndpoint::BLUETOOTH}
};

SerialUnitRouter::SerialUnitRouter(
    BluetoothSerial& bluetoothSerial,
    HardwareSerial& controlSerial
) {
    this->bluetoothSerial = &bluetoothSerial;
    this->controlSerial = &controlSerial;
}

void SerialUnitRouter::route(SerialUnitEndpoint source, SerialUnitAlias alias, String message) {
    if (source == SerialUnitEndpoint::BLUETOOTH) {
        auto destination = routingFromBluetooth.find(alias);
        if (destination != routingFromBluetooth.end()) {
            send(destination->second, message);
        }
    }
    if (source == SerialUnitEndpoint::CONTROL) {
        auto destination = routingFromControl.find(alias);
        if (destination != routingFromControl.end()) {
            send(destination->second, message);
        }
    }
}

void SerialUnitRouter::send(SerialUnitEndpoint destination, String message) {
    if (destination == SerialUnitEndpoint::CONTROL) {
        controlSerial->print(message);
    } else if (destination == SerialUnitEndpoint::BLUETOOTH) {
        bluetoothSerial->print(message);
    }
}