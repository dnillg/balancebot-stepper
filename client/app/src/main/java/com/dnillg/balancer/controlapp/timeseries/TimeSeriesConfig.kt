package com.dnillg.balancer.controlapp.timeseries

class TimeSeriesConfig (val samplesPerSecond: Int = 40) {
  private var counter = 0L
  val diffMultiplier: Float = 1000.0f / samplesPerSecond

  fun next() : Long {
    return counter++
  }

  fun toFloat() : Float {
    return counter.toFloat()
  }

  fun toLong() : Long {
    return counter
  }

}