package com.dnillg.balancer.controlapp

import android.util.Log
import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock

class ConcurrentSerialUnitBacklog <T : SerialUnit> (
  private val items: MutableList<T> = ArrayList(),
  private val mutex: Mutex = Mutex(),
) {

  suspend fun add(item: T) {
    mutex.withLock {
      while (items.size > 500) {
        items.removeAt(0)
        Log.i("ConcurrentSerialUnitBacklog", "Buffer is full, removing oldest item")
      }
      items.add(item)
    }
  }

  suspend fun getAndClear(): List<T> {
    mutex.withLock {
      val itemsCopy = ArrayList(items)
      items.clear()
      return itemsCopy
    }
  }

}