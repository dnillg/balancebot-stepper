package com.dnillg.balancer.controlapp

import HornSpeakerWithHandleSvgrepoCom
import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.content.Context
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
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowBack
import androidx.compose.material.icons.filled.ArrowForward
import androidx.compose.material.icons.filled.Build
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
import javax.inject.Inject
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.MoreVert
import androidx.compose.material3.Text
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.window.Dialog
import com.dnillg.balancer.controlapp.chart.TimeSeriesType
import com.dnillg.balancer.controlapp.chart.chartConfigurations
import com.dnillg.balancer.controlapp.components.Joystick
import com.dnillg.balancer.controlapp.serial.model.ControlSerialUnit

data class MainActivityAsyncJobs(
  var chartRenderer: Job? = null,
  var sinGenerator: Job? = null,
)

class MainActivity @Inject constructor() : ComponentActivity() {

  @Inject
  lateinit var serialWorkerFactory: SerialWorkerFactory

  private val asyncJobs: MainActivityAsyncJobs = MainActivityAsyncJobs()
  private val chartDataBacklog: ConcurrentSerialUnitBacklog<DiagDataSerialUnit> =
    ConcurrentSerialUnitBacklog()
  private val timeSeriesWindow = TimeSeriesWindow(
    duration = 5.0,
    entryCreator = { timeSeries, value -> TimeSeriesChartEntry(timeSeries, value) }
  )
  private val sinusGenerators: MutableMap<String, SinusGenerator> = HashMap()

  private lateinit var lineChart: LineChart;
  private var serialWorker: SerialWorker? = null
  private var btConnection: BtConnection? = null
  private var chartConfigIndex = 0;

  init {
    sinusGenerators["1"] =
      SinusGenerator(samplesPerSecond = 100, amplitude = 100.0, frequency = 2.0)
    sinusGenerators["2"] = SinusGenerator(samplesPerSecond = 100, amplitude = 1.0, frequency = 3.0)
    TimeSeriesType.entries.forEach {
      timeSeriesWindow.init(it.alias, 100)
    }
  }

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
            .systemBarsPadding()
        ) {
          StandardLayout()
        }
      }
    }
  }

  @Composable
  @Preview(showBackground = true)
  fun StandardLayout() {
    val configuration = LocalConfiguration.current
    when (configuration.orientation) {
      Configuration.ORIENTATION_LANDSCAPE -> {
        LandscapeLayout()
      }
    }
  }

  @Composable
  fun LandscapeLayout() {
    var pageIndex = remember { mutableStateOf(0) }
    var showDialog = remember { mutableStateOf(false) }

    if (showDialog.value) {
      FullScreenDialogExample({ showDialog.value = false })
    }

    Row(
      modifier = Modifier
        .fillMaxSize()
        .background(Color.Black),
      horizontalArrangement = Arrangement.spacedBy(0.dp),
      verticalAlignment = Alignment.CenterVertically
    ) {
      Box(
        modifier = Modifier
          .weight(1f)
          .fillMaxHeight(1.0f)
      ) {
        when (pageIndex.value) {
          0 -> StyledLineChart()
          1 -> PidPage()
        }
      }
      Column(
        modifier = Modifier
          .width(240.dp)
          .fillMaxHeight(),
        verticalArrangement = Arrangement.spacedBy(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally
      ) {
        SidebarContent({showDialog.value = true}, pageIndex)
      }
    }
  }

  @Composable
  private fun PidPage() {
    Column(
      modifier = Modifier.fillMaxSize(),
      verticalArrangement = Arrangement.spacedBy(16.dp),
      horizontalAlignment = Alignment.CenterHorizontally
    ) {
      Text("PID Page")
    }
  }

  @Composable
  private fun SidebarContent(
    openTriggerBoard: () -> Unit,
    page : MutableState<Int>
  ) {
    Row(
      modifier = Modifier.fillMaxWidth(),
      verticalAlignment = Alignment.CenterVertically,
      horizontalArrangement = Arrangement.spacedBy(8.dp)
    ) {
      SimpleButton({ openTriggerBoard.invoke() }, Icons.Default.Call)
      SimpleButton({
        stopSinGeneratorRoutine()
        stopChartRendererRoutine()
      }, Icons.Default.Close)
      SimpleButton({ stepChartConfig(-1) }, Icons.Default.ArrowBack)
      SimpleButton({ stepChartConfig(1)}, Icons.Default.ArrowForward)
    }
    Row(
      modifier = Modifier.fillMaxWidth(),
      verticalAlignment = Alignment.CenterVertically,
      horizontalArrangement = Arrangement.spacedBy(8.dp)
    ) {
      SimpleButton(
        onClick = {
          CoroutineScope(Dispatchers.Main).launch {
            startChartRenderer()
            startSinGenerator()
          }
        }, imageVector = HornSpeakerWithHandleSvgrepoCom
      )
      SimpleButton(onClick = {
        page.value = 1;
      }, Icons.Filled.Build)
      SimpleButton({
        page.value = 0;
      }, Icons.Filled.Home)
      SimpleButton({
        CoroutineScope(Dispatchers.Main).launch {
          openTriggerBoard()
        }
      }, Icons.Default.MoreVert)
    }
    Box(modifier = Modifier.fillMaxHeight()) {
      Joystick(modifier = Modifier.align(Alignment.Center), onMove = { x, y ->
        serialWorker?.enqueueAndRemoveOthers(ControlSerialUnit(x, y))
        Log.i(this::class.simpleName, "Joystick: $x, $y")
      })
    }
  }

  private fun startSinGenerator() {
    asyncJobs.sinGenerator = CoroutineScope(Dispatchers.Default).launch {
      while (true) {
        val v = sinusGenerators["2"]!!.next()
        chartDataBacklog.add(
          DiagDataSerialUnit(
            v * 150f,
            v * 180f,
            v * 8000f,
            v * 9000f,
            v * 4000f,
            v * 6000f
          )
        )
        delay(5);
      }
    }
  }

  private fun startChartRenderer() {
    asyncJobs.chartRenderer = CoroutineScope(Dispatchers.Main).launch {
      while (true) {
        val items = chartDataBacklog.getAndClear()
        for (i in items) {
          timeSeriesWindow.addPoint(TimeSeriesType.ROLL.alias, i.roll)
          timeSeriesWindow.addPoint(TimeSeriesType.TARGET_ROLL.alias, i.targetRoll)
          timeSeriesWindow.addPoint(TimeSeriesType.SPEED.alias, i.speed)
          timeSeriesWindow.addPoint(TimeSeriesType.TARGET_SPEED.alias, i.targetSpeed)
          timeSeriesWindow.addPoint(TimeSeriesType.MOTOR_LEFT_SPEED.alias, i.motorLeft)
          timeSeriesWindow.addPoint(TimeSeriesType.MOTOR_RIGHT_SPEED.alias, i.motorRight)
          timeSeriesWindow.addPoint(TimeSeriesType.MOTOR_SCALED_ROLL_ERROR.alias, (i.targetRoll - i.roll) * 100 )
        }
        lineChart.invalidate()
        delay(33);
      }
    }
  }

  @Composable
  fun FullScreenDialogExample(onDismiss: () -> Unit) {
    Dialog(onDismissRequest = onDismiss) {
      Box(
        modifier = Modifier
          .background(Color.White, shape = RoundedCornerShape(16.dp))
          .padding(16.dp)
      ) {
        Column(
          verticalArrangement = Arrangement.spacedBy(16.dp),
          horizontalAlignment = Alignment.CenterHorizontally
        ) {
          Row(
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.spacedBy(16.dp)
          ) {
            SimpleButton({ onDismiss() }, Icons.Default.Close)
            SimpleButton({ onDismiss() }, Icons.Default.Close)
            SimpleButton({ onDismiss() }, Icons.Default.Close)
            SimpleButton({ onDismiss() }, Icons.Default.Close)
          }

        }
      }
    }
  }

  private fun stopSinGeneratorRoutine() {
    asyncJobs.sinGenerator?.cancel()
    asyncJobs.sinGenerator = null;
  }

  private fun stopChartRendererRoutine() {
    asyncJobs.chartRenderer?.cancel()
    asyncJobs.chartRenderer = null;
  }

  private fun stepChartConfig(offset: Int = 1) {
    chartConfigIndex = (chartConfigurations.size + chartConfigIndex + offset) % chartConfigurations.size
    val config = chartConfigurations[chartConfigIndex]
    lineChart.axisLeft.axisMinimum = config.minimumValue;
    lineChart.axisLeft.axisMaximum = config.maximumValue;
    val lineData = LineData().apply {
      config.activeSeries.forEach {
        addDataSet(
          LineDataSet(timeSeriesWindow.getPoints(it.alias), it.label)
            .activityDefaults().withColor(it.color)
        )
      }
    }
    lineChart.data = lineData
    lineChart.invalidate()
  }

  @Composable
  private fun SimpleButton(
    onClick: () -> Unit,
    imageVector: ImageVector
  ) {
    IconButton(
      onClick = onClick,
      modifier = Modifier
        .background(Color.LightGray, shape = CircleShape)
        .size(42.dp)
    ) {
      Icon(
        imageVector = imageVector,
        contentDescription = "Button",
        modifier = Modifier.size(32.dp)
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
    val context = LocalContext.current;
    this.lineChart = remember { diagChart(context) };
    stepChartConfig(0)
    AndroidView(
      factory = { lineChart },
      modifier = Modifier.fillMaxSize()
    )
  }

  //@Composable
  private fun diagChart(context: Context): LineChart {
    return LineChart(context).apply {
      setBackgroundColor(Color.Black.toArgb())
      axisLeft.apply {
        axisLineColor = Color.White.toArgb()
        textColor = Color.White.toArgb()
      }
      axisRight.isEnabled = false
      xAxis.apply {
        position = XAxis.XAxisPosition.BOTTOM
        textColor = Color.White.toArgb()
      }
      legend.apply {
        form = Legend.LegendForm.LINE
        textColor = Color.White.toArgb()
      }

      xAxis.axisMinimum = -5000f;
      xAxis.axisMaximum = 0f;
    }
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
