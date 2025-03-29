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
import java.util.LinkedList
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
  private val receivedBacklog: LinkedList<String> = LinkedList(),
  private val sendDebounceMap: MutableMap<String, Long> = HashMap<String, Long>(),
) : Runnable {

  private val taskScope = CoroutineScope(Dispatchers.Default + Job())

  override fun run() {
    taskScope.launch {
      readRoutine()
    }
    taskScope.launch {
      consumeBacklog()
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
          try {
            val line = serial.send(it);
            Log.i("SerialUnitThread", "Sending: $line")
            btConnection.writeLine(line)
          } catch (e : Exception) {
            Log.e(this.javaClass .simpleName, "Could not write BT", e);
          }
        }
        unitsToSend.clear();
      }
    }
  }

  private suspend fun readRoutine() {
    while (!btConnection.isClosed) {
      val line = btConnection.readLine();
      if (line != null) {
        receivedBacklog.add(line);
      } else {
        delay(1)
      }
    }
  }

  private suspend fun consumeBacklog() {
    while(!btConnection.isClosed) {
      if (receivedBacklog.isEmpty()) {
        delay(1)
      }
      try {
        while (receivedBacklog.isNotEmpty()) {
          if (receivedBacklog.size > 20) {
            Log.w("SerialUnitThread", "Backlog is too big: ${receivedBacklog.size}")
            receivedBacklog.clear()
          }
          val line = receivedBacklog.poll();
          if (line != null) {
            val unit = serial.receive(line);
            //Log.i("SerialUnitThread", "Received: $unit")
            listeners.forEach { listener ->
              if (listener.clazz.isInstance(unit)) {
                listener.action(unit)
              }
            }
          }
        }
      } catch (exception : Exception) {
        Log.e(this.javaClass.simpleName, "Error while reading line", exception)
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

  fun enqueueAndDebounce(unit: SerialUnit, key: String, debounceTime: Long) {
    taskScope.launch {
      if (sendDebounceMap.getOrDefault(key, 0L) + debounceTime > System.currentTimeMillis()) {
        Log.i("SerialWorker", "Debouncing send for key: $key")
        return@launch
      }
      mutex.withLock {
        unitsToSend.add(unit)
        sendDebounceMap[key] = System.currentTimeMillis()
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