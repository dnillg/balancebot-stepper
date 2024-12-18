package com.dnillg.balancer.controlapp.serial

interface SerialInterface {
  val isClosed : Boolean

  fun reconnect()
  fun isAlive() : Boolean
  fun close()
  fun readLine(): String?
  fun writeLine(line: String)
}