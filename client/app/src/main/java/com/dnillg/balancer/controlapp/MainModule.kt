package com.dnillg.balancer.controlapp

import com.dnillg.balancer.controlapp.serial.SerialIO
import com.dnillg.balancer.controlapp.serial.SerialWorkerFactory
import com.dnillg.balancer.controlapp.serial.serialization.ControlSerialUnitSerializer
import com.dnillg.balancer.controlapp.serial.serialization.DG1Serializer
import com.dnillg.balancer.controlapp.serial.serialization.DiagDataSerializer
import com.dnillg.balancer.controlapp.serial.serialization.GetPIDResponseSerialUnitSerializer
import com.dnillg.balancer.controlapp.serial.serialization.GetPIDSerialUnitSerializer
import com.dnillg.balancer.controlapp.serial.serialization.MotorToggleSerialUnitSerializer
import com.dnillg.balancer.controlapp.serial.serialization.SerialUnitSerializer
import com.dnillg.balancer.controlapp.serial.serialization.SetPIDSerialUnitSerializer
import com.dnillg.balancer.controlapp.serial.serialization.TriggerSerialUnitSerializer
import dagger.Module
import dagger.Provides

@Module
class MainModule {

    @Provides
    fun serialIO(): SerialIO {
        val serializers = HashSet<SerialUnitSerializer<*>>()
        serializers.add(DiagDataSerializer())
        serializers.add(DG1Serializer())
        serializers.add(SetPIDSerialUnitSerializer())
        serializers.add(GetPIDSerialUnitSerializer())
        serializers.add(GetPIDResponseSerialUnitSerializer())
        serializers.add(ControlSerialUnitSerializer())
        serializers.add(TriggerSerialUnitSerializer())
        serializers.add(MotorToggleSerialUnitSerializer())
        return SerialIO(serializers = serializers)
    }

    @Provides
    fun serialUnitThreadFactory(serialIO: SerialIO): SerialWorkerFactory {
        return SerialWorkerFactory(serialIO)
    }

}