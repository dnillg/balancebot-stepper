package com.dnillg.balancer.controlapp

import android.util.Log
import com.dnillg.balancer.controlapp.bt.BtConnection
import com.dnillg.balancer.controlapp.serial.SerialIO
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

data class ListenerRegistration(
  val uuid: UUID,
  val clazz: Class<*>,
  val listener: (SerialUnit) -> Unit,
)

class SerialUnitThread(
  private val btConnection: BtConnection,
  private val serial: SerialIO,
  private val listeners: MutableList<ListenerRegistration> = ArrayList(),
  private val unitsToSend: MutableList<SerialUnit> = ArrayList(),
  private val mutex: Mutex = Mutex(),
) : Runnable {
  private var shouldContinue = true;

  private val taskScope = CoroutineScope(Dispatchers.Default + Job())

  override fun run() {
    taskScope.launch {
      while (shouldContinue) {
        val line = btConnection.readLine();
        if (line != null) {
          val unit = serial.receive(line);
          listeners.forEach {
            if (it.clazz.isInstance(unit)) {
              it.listener.invoke(unit);
            }
          }
        } else {
          delay(100)
        }
      }
    }

    taskScope.launch {
      while (shouldContinue) {
        mutex.withLock {
          unitsToSend.forEach {
            btConnection.writeLine(serial.send(it))
          }
          unitsToSend.clear();
        }
      }
    }

    taskScope.launch {
      while (shouldContinue) {
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
  }

  fun <T : SerialUnit> subscribe(clazz: Class<T>, listener: (SerialUnit) -> Unit): UUID {
    val uuid = UUID.randomUUID()
    listeners.add(ListenerRegistration(uuid, clazz, listener));
    return uuid;
  }

  fun unsubscribe(uuid: UUID) {
    listeners.removeIf { it.uuid == uuid }
  }

  fun stop() {
    shouldContinue = false;
    taskScope.cancel();
  }

  fun send(unit: SerialUnit) {
    taskScope.launch {
      mutex.withLock {
        unitsToSend.add(unit);
      }
    }
  }
}

class SerialUnitThreadFactory(
  private val serial: SerialIO,
) {
  fun create(btConnection: BtConnection): SerialUnitThread {
    return SerialUnitThread(btConnection, serial);
  }
}