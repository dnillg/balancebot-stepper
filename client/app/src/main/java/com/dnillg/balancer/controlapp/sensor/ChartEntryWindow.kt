package com.dnillg.balancer.controlapp.sensor

import com.github.mikephil.charting.data.Entry
import java.util.HashMap
import java.util.LinkedList

class TimeSeries (val samplesPerSecond: Int = 200) {
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

class TimeSeriesEntry(val timeSeries: TimeSeries, value: Float): Entry(timeSeries.toFloat(), value) {
  private val start = timeSeries.toLong()

  override fun getX(): Float {
    return (start - timeSeries.toLong()) * timeSeries.diffMultiplier
  }
}

class ChartEntryWindow (
  private val seconds : Double = 5.0,
  private val points : MutableMap<String, LinkedList<TimeSeriesEntry>> = HashMap(),
  private val counters : MutableMap<String, TimeSeries> = HashMap(),
) {

  fun addPoint(sensor: String, value: Float) {
    val sensorPoints = points.getOrPut(sensor, this::emptyList)
    val timeSeries = counters.getOrPut(sensor, { TimeSeries() })
    sensorPoints.add(TimeSeriesEntry(timeSeries, value))
    val min = seconds * timeSeries.diffMultiplier * timeSeries.samplesPerSecond;
    while(!sensorPoints.isEmpty() && sensorPoints.first.x < - min) {
      sensorPoints.removeFirst()
    }
    counters[sensor]!!.next();
  }

  fun getPoints(sensor: String) : List<TimeSeriesEntry> {
    return points.getOrElse(sensor, this::emptyList)
  }

  fun clear() {
    points.clear()
  }

  fun generateSin(name: String, samplesPerSecond : Int, offsetRadians: Double = 0.0, amplitude: Double = 100.0) {
    this.counters[name] = TimeSeries(samplesPerSecond)

    val maxPoints = (seconds * samplesPerSecond).toInt()
    val frequency = 1.0 / seconds
    val period = 1.0 / frequency
    val step = period / samplesPerSecond
    val radiansPerStep = 2 * Math.PI * frequency * step
    for (i in 0 until maxPoints) {
      val radians = offsetRadians + i * radiansPerStep
      val value = (Math.sin(radians) * amplitude).toFloat()
      addPoint(name, value)
    }
  }

  private fun emptyList() = LinkedList<TimeSeriesEntry>()


}