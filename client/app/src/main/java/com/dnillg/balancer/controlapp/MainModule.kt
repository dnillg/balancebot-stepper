package com.dnillg.balancer.controlapp

import com.dnillg.balancer.controlapp.serial.SerialIO
import com.dnillg.balancer.controlapp.serial.SerialWorkerFactory
import com.dnillg.balancer.controlapp.serial.serialization.ControlSerialUnitSerializer
import com.dnillg.balancer.controlapp.serial.serialization.DiagDataSerializer
import com.dnillg.balancer.controlapp.serial.serialization.SerialUnitSerializer
import com.dnillg.balancer.controlapp.serial.serialization.SetPIDSerialUnitSerializer
import com.dnillg.balancer.controlapp.serial.serialization.TriggerSerialUnitSerializer
import dagger.Module
import dagger.Provides

@Module
class MainModule {

    @Provides
    fun serialIO(): SerialIO {
        val serializers = HashSet<SerialUnitSerializer<*>>();
        serializers.add(DiagDataSerializer());
        serializers.add(SetPIDSerialUnitSerializer());
        serializers.add(ControlSerialUnitSerializer());
        serializers.add(TriggerSerialUnitSerializer());
        return SerialIO(serializers = serializers);
    }

    @Provides
    fun serialUnitThreadFactory(serialIO: SerialIO): SerialWorkerFactory {
        return SerialWorkerFactory(serialIO);
    }

}