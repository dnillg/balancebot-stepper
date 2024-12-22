package com.dnillg.balancer.controlapp.domain.model

enum class ConnectionStatusEnum (
  val displayName: String
) {
  CONNECTED("Connected"),
  NOT_CONNECTED("Not connected"),
  UNHEALTHY("Unhealthy");
}

data class ConnectionStatus (
  val status: ConnectionStatusEnum = ConnectionStatusEnum.NOT_CONNECTED,
  val batteryLevel: Float = 1.0f,
) {
  fun toUnhealthy(): ConnectionStatus {
    return this.copy(status = ConnectionStatusEnum.UNHEALTHY)
  }

  fun toDisconnected(): ConnectionStatus {
    return this.copy(status = ConnectionStatusEnum.NOT_CONNECTED)
  }

  fun toConnected(): ConnectionStatus {
    return this.copy(status = ConnectionStatusEnum.CONNECTED, batteryLevel = 1.0f)
  }

  fun withBatteryLevel(level: Float): ConnectionStatus {
    return this.copy(batteryLevel = level)
  }

}