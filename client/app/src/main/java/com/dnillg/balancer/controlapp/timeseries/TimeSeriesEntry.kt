package com.dnillg.balancer.controlapp.timeseries

interface TimeSeriesEntry {
    val timeSeriesConfig: TimeSeriesConfig
    val value: Float
}