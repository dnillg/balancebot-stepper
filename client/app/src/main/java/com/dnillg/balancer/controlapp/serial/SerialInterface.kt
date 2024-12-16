package com.dnillg.balancer.controlapp.serial

interface SerialInterface {
  fun reconnect()
  fun isAlive() : Boolean
  fun close()
  fun readLine(): String?
  fun writeLine(line: String)
}