package com.dnillg.balancer.controlapp.serial

import android.util.Log
import com.dnillg.balancer.controlapp.bluetooth.BtConnection
import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.cancel
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
import java.util.ArrayList
import java.util.UUID

data class SerialWorkerListener(
  val uuid: UUID,
  val clazz: Class<*>,
  val action: (SerialUnit) -> Unit,
)

class SerialWorker(

  private val btConnection: SerialInterface,
  private val serial: SerialIO,
  private val listeners: MutableList<SerialWorkerListener> = ArrayList(),
  private val unitsToSend: MutableList<SerialUnit> = ArrayList(),
  private val mutex: Mutex = Mutex(),
) : Runnable {

  private val taskScope = CoroutineScope(Dispatchers.Default + Job())

  override fun run() {
    taskScope.launch {
      readRoutine()
    }
    taskScope.launch {
      writeRoutine()
    }
    taskScope.launch {
      sentinelRoutine()
    }
  }

  private suspend fun sentinelRoutine() {
    while (!btConnection.isClosed) {
      if (!btConnection.isAlive()) {
        try {
          btConnection.reconnect()
        } catch (e: Exception) {
          Log.e("SerialUnitThread", "Error reconnecting", e)
        }
      }
      delay(1000)
    }
  }

  private suspend fun writeRoutine() {
    while (!btConnection.isClosed) {
      mutex.withLock {
        unitsToSend.forEach {
          btConnection.writeLine(serial.send(it))
        }
        unitsToSend.clear();
      }
    }
  }

  private suspend fun readRoutine() {
    while (!btConnection.isClosed) {
      val line = btConnection.readLine();
      if (line != null) {
        val unit = serial.receive(line);
        listeners.forEach {
          if (it.clazz.isInstance(unit)) {
            it.action.invoke(unit);
          }
        }
      } else {
        delay(100)
      }
    }
  }

  fun <T : SerialUnit> subscribe(clazz: Class<T>, listener: (SerialUnit) -> Unit): UUID {
    val uuid = UUID.randomUUID()
    listeners.add(SerialWorkerListener(uuid, clazz, listener));
    return uuid;
  }

  fun unsubscribe(uuid: UUID) {
    listeners.removeIf { it.uuid == uuid }
  }

  fun stop() {
    taskScope.cancel();
  }

  fun enqueue(unit: SerialUnit) {
    taskScope.launch {
      mutex.withLock {
        unitsToSend.add(unit);
      }
    }
  }

  fun enqueueAndRemoveOthers(unit: SerialUnit) {
    taskScope.launch {
      mutex.withLock {
        unitsToSend.removeIf { it.javaClass == unit.javaClass }
        unitsToSend.add(unit);
      }
    }
  }
}

class SerialWorkerFactory(
  private val serial: SerialIO,
) {
  fun create(btConnection: BtConnection): SerialWorker {
    return SerialWorker(btConnection, serial);
  }
}