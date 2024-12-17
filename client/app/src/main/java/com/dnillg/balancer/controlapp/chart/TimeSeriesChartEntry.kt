package com.dnillg.balancer.controlapp.chart

import com.dnillg.balancer.controlapp.timeseries.TimeSeriesConfig
import com.dnillg.balancer.controlapp.timeseries.TimeSeriesEntry
import com.github.mikephil.charting.data.Entry

class TimeSeriesChartEntry(
  override val timeSeriesConfig: TimeSeriesConfig,
  override val value: Float
): Entry(timeSeriesConfig.toFloat(), value), TimeSeriesEntry {
  private val start = timeSeriesConfig.toLong()

  override fun getX(): Float {
    return (start - timeSeriesConfig.toLong()) * timeSeriesConfig.diffMultiplier
  }

}