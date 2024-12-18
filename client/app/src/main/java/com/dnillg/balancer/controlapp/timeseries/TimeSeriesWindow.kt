package com.dnillg.balancer.controlapp.timeseries

import java.util.HashMap
import java.util.LinkedList



class TimeSeriesWindow<T : TimeSeriesEntry> (
  private val duration : Double = 5.0,
  private val seriesPoints : MutableMap<String, LinkedList<T>> = HashMap(),
  private val seriesConfigs : MutableMap<String, TimeSeriesConfig> = HashMap(),
  private val entryCreator: (TimeSeriesConfig, Float) -> T
) {

  fun addPoint(name: String, value: Float?) {
    val sensorPoints = seriesPoints.getOrPut(name, this::emptyList)
    val timeSeriesConfig = seriesConfigs.getOrPut(name, { TimeSeriesConfig() })
    val min = duration * timeSeriesConfig.diffMultiplier * timeSeriesConfig.samplesPerSecond;
    if (value != null) {
      sensorPoints.add(entryCreator.invoke(timeSeriesConfig, value))
    }
    while (!sensorPoints.isEmpty() && sensorPoints.first().value < -min) {
      sensorPoints.removeFirst()
    }
    seriesConfigs[name]!!.next();
  }

  fun getPoints(name: String) : List<T> {
    seriesPoints.putIfAbsent(name, emptyList())
    return seriesPoints[name]!!
  }

  fun clear() {
    seriesPoints.clear()
  }

  fun clear(name: String) {
    seriesPoints.remove(name)
    seriesConfigs.remove(name)
  }

  fun init(name: String, samplesPerSecond: Int = 200) {
    this.seriesConfigs[name] = TimeSeriesConfig(samplesPerSecond)
  }

  private fun emptyList() = LinkedList<T>()


}