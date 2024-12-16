import android.content.Context
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.width
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.viewinterop.AndroidView
import com.github.mikephil.charting.charts.LineChart
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import com.github.mikephil.charting.components.XAxis

@Composable
fun LineChartView(dataPoints: List<Float>, context: Context) {
    // Create the chart using AndroidView
    AndroidView(
        factory = { ctx ->
            LineChart(ctx).apply {
                setupLineChart(this, dataPoints)
            }
        },
        modifier = Modifier.fillMaxSize(),
        update = { chart ->
            // Update chart data dynamically
            val lineData = createLineData(dataPoints)
            chart.data = lineData
            chart.notifyDataSetChanged()
            chart.invalidate() // Refresh the chart
        }
    )
}

// Helper function to set up LineChart
private fun setupLineChart(chart: LineChart, dataPoints: List<Float>) {
    chart.description.isEnabled = false
    chart.setDrawGridBackground(false)
    chart.setTouchEnabled(true)
    chart.isDragEnabled = true
    chart.setScaleEnabled(true)
    chart.setPinchZoom(true)
    chart.xAxis.position = XAxis.XAxisPosition.BOTTOM
    chart.axisRight.isEnabled = false
    chart.data = createLineData(dataPoints)
}

// Helper function to create LineData
private fun createLineData(dataPoints: List<Float>): LineData {
    val entries = dataPoints.mapIndexed { index, value ->
        Entry(index.toFloat(), value)
    }

    val dataSet = LineDataSet(entries, "Line Chart Example").apply {
        color = android.graphics.Color.BLUE
        lineWidth = 2f
        setDrawCircles(true)
        setDrawCircleHole(false)
        setDrawValues(false)
    }

    return LineData(dataSet)
}
