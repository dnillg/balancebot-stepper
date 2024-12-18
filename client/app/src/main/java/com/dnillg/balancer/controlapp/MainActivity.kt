package com.dnillg.balancer.controlapp

import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.content.pm.PackageManager
import android.content.res.Configuration
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.systemBarsPadding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowBack
import androidx.compose.material.icons.filled.ArrowForward
import androidx.compose.material.icons.filled.Call
import androidx.compose.material.icons.filled.Close
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.dnillg.balancer.controlapp.bluetooth.BtConnection
import com.dnillg.balancer.controlapp.chart.TimeSeriesChartEntry
import com.dnillg.balancer.controlapp.timeseries.TimeSeriesWindow
import com.dnillg.balancer.controlapp.chart.SinusGenerator
import com.dnillg.balancer.controlapp.serial.SerialWorker
import com.dnillg.balancer.controlapp.serial.SerialWorkerFactory
import com.dnillg.balancer.controlapp.serial.model.DiagDataSerialUnit
import com.dnillg.balancer.controlapp.ui.theme.ControlAppTheme
import com.github.mikephil.charting.charts.LineChart
import com.github.mikephil.charting.components.Legend
import com.github.mikephil.charting.components.XAxis
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.Job
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import java.util.concurrent.atomic.AtomicInteger
import javax.inject.Inject
import androidx.compose.material.icons.filled.Add
import androidx.compose.material.icons.filled.Check
import androidx.compose.material.icons.filled.Email
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import com.dnillg.balancer.controlapp.components.Joystick

data class MainActivityAsyncJobs(
  var chartRenderer: Job? = null,
  var sinGenerator: Job? = null,
)

class MainActivity @Inject constructor() : ComponentActivity() {

  @Inject
  lateinit var serialWorkerFactory: SerialWorkerFactory
  private val asyncJobs: MainActivityAsyncJobs = MainActivityAsyncJobs()
  private var serialWorker: SerialWorker? = null
  private var btConnection: BtConnection? = null
  private val chartDataBacklog: ConcurrentSerialUnitBacklog<DiagDataSerialUnit> =
    ConcurrentSerialUnitBacklog()
  private val timeSeriesWindow = TimeSeriesWindow(
    duration = 5.0,
    entryCreator = { timeSeries, value -> TimeSeriesChartEntry(timeSeries, value) }
  )
  private val sinusGenerators: MutableMap<String, SinusGenerator> = HashMap()

  //TODO: Remove
  private val counter: AtomicInteger = AtomicInteger(0)

  init {
    sinusGenerators["1"] =
      SinusGenerator(samplesPerSecond = 100, amplitude = 100.0, frequency = 2.0)
    sinusGenerators["2"] = SinusGenerator(samplesPerSecond = 100, amplitude = 80.0, frequency = 1.0)
    timeSeriesWindow.init("roll", 100)
    timeSeriesWindow.init("target-roll", 100)
    timeSeriesWindow.init("speed", 100)
    timeSeriesWindow.init("target-speed", 100)
    timeSeriesWindow.init("motor-left", 100)
    timeSeriesWindow.init("motor-right", 100)
    sinusGenerators["1"]!!.generate(100 * 5, { timeSeriesWindow.addPoint("roll", it) });
    sinusGenerators["2"]!!.generate(100 * 5, { timeSeriesWindow.addPoint("target-roll", it) });
  }

  private var lineChart: LineChart? = null;

  override fun onCreate(savedInstanceState: Bundle?) {
    super.onCreate(savedInstanceState)
    DaggerAppComponent.create().inject(this)
    enableEdgeToEdge()
    setContent {
      ControlAppTheme {
        Box(
          modifier = Modifier
            .fillMaxSize()
            .background(Color.Black)
            .systemBarsPadding() // Prevent content from overlapping system UI
        ) {
          DefaultLayout()
        }
      }
    }
  }

  @Composable
  fun DefaultLayout() {
    OrientationBasedLayout()
  }


  @Composable
  @Preview(showBackground = true)
  fun OrientationBasedLayout() {
    val configuration = LocalConfiguration.current
    when (configuration.orientation) {
      Configuration.ORIENTATION_LANDSCAPE -> {
        LandscapeLayout()
      }
    }
  }

  @Composable
  fun LandscapeLayout() {
    Row(
      modifier = Modifier
        .fillMaxSize()
        .background(Color.Black)
        .padding(0.dp),
      horizontalArrangement = Arrangement.spacedBy(0.dp),
      verticalAlignment = Alignment.CenterVertically
    ) {
      Box(
        modifier = Modifier
          .weight(1f)
          .fillMaxHeight(1.0f)
          .background(Color.Red)
      ) {
        StyledLineChart()
      }
      Column(
        modifier = Modifier
          .width(240.dp)
          .fillMaxHeight()
          .background(Color.Black),
        verticalArrangement = Arrangement.spacedBy(8.dp),
        horizontalAlignment = Alignment.CenterHorizontally
      ) {
        Row(
          modifier = Modifier.fillMaxWidth(),
          verticalAlignment = Alignment.CenterVertically,
          horizontalArrangement = Arrangement.spacedBy(8.dp) // This works now
        ) {
          SimpleButton({ /*TODO*/ }, Icons.Default.Call)
          SimpleButton({ /*TODO*/ }, Icons.Default.Close)
          SimpleButton({ /*TODO*/ }, Icons.Default.ArrowBack)
          SimpleButton({ /*TODO*/ }, Icons.Default.ArrowForward)
        }
        Row(
          modifier = Modifier.fillMaxWidth(),
          verticalAlignment = Alignment.CenterVertically,
          horizontalArrangement = Arrangement.spacedBy(8.dp) // This works now
        ) {
          SimpleButton(
            onClick = {
              CoroutineScope(Dispatchers.Main).launch {
                asyncJobs.chartRenderer?.cancel()
                asyncJobs.chartRenderer = null;
                asyncJobs.sinGenerator?.cancel()
                asyncJobs.sinGenerator = null;
              }
            }, imageVector = Icons.Default.Email)
          SimpleButton(onClick = {
            asyncJobs.chartRenderer = CoroutineScope(Dispatchers.Main).launch {
              while (true) {
                val items = chartDataBacklog.getAndClear()
                for (i in items) {
                  timeSeriesWindow.addPoint("roll", i.roll)
                  timeSeriesWindow.addPoint("target-roll", i.targetRoll)
                }
                lineChart!!.invalidate()
                delay(33);
              }
            }
            asyncJobs.sinGenerator = CoroutineScope(Dispatchers.Default).launch {
              while (true) {
                val v = sinusGenerators["1"]!!.next()
                chartDataBacklog.add(
                  DiagDataSerialUnit(
                    v,
                    v * 0.9f,
                    v * 0.7f,
                    v * 0.6f,
                    v * 0.9f,
                    6f
                  )
                )
                delay(5);
              }
            }
          }, Icons.Default.Add)
          SimpleButton({
            CoroutineScope(Dispatchers.Main).launch {
              connectBluetooth()
              createAndStartSerialWorker()
            }
          }, Icons.Default.Check)
        }
        Box(modifier = Modifier.fillMaxHeight()) {
          Joystick(modifier = Modifier.align(Alignment.Center), onMove = { x, y ->
            Log.i(this::class.simpleName, "Joystick: $x, $y")
          })
        }
      }
    }
  }

  @Composable
  private fun SimpleButton(
    onClick: () -> Unit,
    imageVector: ImageVector
  ) {
    IconButton(
      onClick = onClick,
      modifier = Modifier
        .background(Color.LightGray, shape = CircleShape) // Background for the entire button
    ) {
      Icon(
        imageVector = imageVector,
        contentDescription = "Button",
        modifier = Modifier.size(24.dp) // Icon size should be smaller than button
      )
    }
  }

  private fun createAndStartSerialWorker() {
    if (btConnection == null) {
      return;
    }
    serialWorker?.stop();
    serialWorker = serialWorkerFactory.create(btConnection!!)
    serialWorker!!.subscribe(DiagDataSerialUnit::class.java) {
      Log.i(this::class.simpleName, "Received: $it")
    }
    serialWorker!!.run();
  }

  @Composable
  private fun StyledLineChart() {
    val context = LocalContext.current
    this.lineChart = remember {
      LineChart(context).apply {
        setBackgroundColor(Color.Black.toArgb()) // Background color
        axisLeft.apply {
          axisLineColor = Color.White.toArgb()
          textColor = Color.White.toArgb()
        }
        axisRight.isEnabled = false
        xAxis.apply {
          position = XAxis.XAxisPosition.BOTTOM // Bottom X-axis
          textColor = Color.White.toArgb()
        }
        legend.apply {
          form = Legend.LegendForm.LINE
          textColor = Color.White.toArgb()
        }
      }
    }

    val lineData = LineData().apply {
      addDataSet(
        LineDataSet(timeSeriesWindow.getPoints("roll"), "R")
          .activityDefaults().withColor(Color.Red)
      )
      addDataSet(
        LineDataSet(timeSeriesWindow.getPoints("target-roll"), "TR")
          .activityDefaults().withColor(Color.Blue)
      )
      addDataSet(
        LineDataSet(timeSeriesWindow.getPoints("speed"), "S")
          .activityDefaults().withColor(Color.Blue)
      )
      addDataSet(
        LineDataSet(timeSeriesWindow.getPoints("speed"), "TS")
          .activityDefaults().withColor(Color.Blue)
      )
      addDataSet(
        LineDataSet(timeSeriesWindow.getPoints("motor-left"), "ML")
          .activityDefaults().withColor(Color.Blue)
      )
      addDataSet(
        LineDataSet(timeSeriesWindow.getPoints("motor-right"), "MR")
          .activityDefaults().withColor(Color.Blue)
      )
    }
    lineChart!!.data = lineData
    AndroidView(
      factory = { lineChart!! },
      modifier = Modifier.fillMaxSize()
    )
  }

  private fun connectBluetooth() {
    closeBluetoothConnection()

    if (!hasBluetoothPermissions()) {
      Log.i(this::class.simpleName, "Request permission");
      requestBluetoothPermissions();
    }
    if (!hasBluetoothPermissions()) {
      return;
    }
    doConnectBluetooth()
    Log.i(this::class.simpleName, "Connected.")
  }

  @SuppressLint("MissingPermission")
  private fun doConnectBluetooth() {
    val bluetoothManager = this.getSystemService(BLUETOOTH_SERVICE) as BluetoothManager
    val bluetoothAdapter: BluetoothAdapter = bluetoothManager.adapter
    val pairedDevices: Set<BluetoothDevice>? = bluetoothAdapter.bondedDevices
    pairedDevices?.forEach { device ->
      if (device.name.contains("balance")) {
        btConnection = BtConnection(device)
      }
    }
  }

  private fun closeBluetoothConnection() {
    serialWorker!!.stop();
    if (btConnection != null) {
      try {
        btConnection!!.close();
      } catch (e: Throwable) {
        Log.e(this::class.simpleName, "Could not close BT connection", e)
      }
      btConnection = null;
    }
  }

  private fun hasBluetoothPermissions(): Boolean {
    return ContextCompat.checkSelfPermission(
      this,
      Manifest.permission.BLUETOOTH_CONNECT
    ) == PackageManager.PERMISSION_GRANTED
  }

  private fun requestBluetoothPermissions() {
    ActivityCompat.requestPermissions(
      this,
      arrayOf(
        Manifest.permission.BLUETOOTH_CONNECT
      ),
      BLUETOOTH_PERMISSION_REQUEST_CODE
    )
  }

  companion object {
    private const val BLUETOOTH_PERMISSION_REQUEST_CODE = 1001;
  }

}

private fun LineDataSet.activityDefaults(): LineDataSet {
  lineWidth = 1f
  setDrawValues(false)
  setDrawCircles(false)
  return this
}

private fun LineDataSet.withColor(color: Color): LineDataSet {
  this.color = color.toArgb()
  return this
}
