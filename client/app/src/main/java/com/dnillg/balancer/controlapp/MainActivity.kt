package com.dnillg.balancer.controlapp

import HornSpeakerWithHandleSvgrepoCom
import android.Manifest
import android.annotation.SuppressLint
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.content.Context
import android.content.pm.PackageManager
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.RowScope
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.systemBarsPadding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowBack
import androidx.compose.material.icons.filled.ArrowForward
import androidx.compose.material.icons.filled.Build
import androidx.compose.material.icons.filled.Call
import androidx.compose.material.icons.filled.Close
import androidx.compose.material.icons.filled.Face
import androidx.compose.material.icons.filled.Favorite
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.MoreVert
import androidx.compose.material.icons.filled.Notifications
import androidx.compose.material.icons.filled.PlayArrow
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.compose.ui.window.Dialog
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.dnillg.balancer.controlapp.bluetooth.BtConnection
import com.dnillg.balancer.controlapp.components.ConnectionBar
import com.dnillg.balancer.controlapp.components.FloatingPointBox
import com.dnillg.balancer.controlapp.components.Joystick
import com.dnillg.balancer.controlapp.components.LargeButton
import com.dnillg.balancer.controlapp.components.SimpleButton
import com.dnillg.balancer.controlapp.domain.chart.SinusGenerator
import com.dnillg.balancer.controlapp.domain.chart.TimeSeriesChartEntry
import com.dnillg.balancer.controlapp.domain.chart.TimeSeriesType
import com.dnillg.balancer.controlapp.domain.chart.chartConfigurations
import com.dnillg.balancer.controlapp.domain.model.ConnectionStatus
import com.dnillg.balancer.controlapp.domain.model.PIDType
import com.dnillg.balancer.controlapp.domain.model.PIDValues
import com.dnillg.balancer.controlapp.serial.SerialWorker
import com.dnillg.balancer.controlapp.serial.SerialWorkerFactory
import com.dnillg.balancer.controlapp.serial.model.ControlSerialUnit
import com.dnillg.balancer.controlapp.serial.model.DiagDataSerialUnit
import com.dnillg.balancer.controlapp.serial.model.GetPIDResponseSerialUnit
import com.dnillg.balancer.controlapp.serial.model.GetPIDSerialUnit
import com.dnillg.balancer.controlapp.serial.model.MotorToggleSerialUnit
import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import com.dnillg.balancer.controlapp.serial.model.SetPIDSerialUnit
import com.dnillg.balancer.controlapp.serial.model.TriggerSerialUnit
import com.dnillg.balancer.controlapp.serial.model.TriggerType
import com.dnillg.balancer.controlapp.timeseries.TimeSeriesWindow
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
  private lateinit var connectionStatus: MutableState<ConnectionStatus>;
  private lateinit var pidValues: MutableState<PIDValues>;
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

  private fun sendUnit(unit: SerialUnit) {
    serialWorker?.enqueue(unit);
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
          LandscapeLayout()
        }
      }
    }
  }

  @Composable
  fun LandscapeLayout() {
    val pageIndex = remember { mutableIntStateOf(0) }

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
        when (pageIndex.intValue) {
          0 -> StyledLineChart()
          1 -> PidPage()
        }
      }
      Column(
        modifier = Modifier
          .width(200.dp)
          .padding(8.dp)
          .fillMaxHeight(),
        verticalArrangement = Arrangement.spacedBy(8.dp),
        horizontalAlignment = Alignment.CenterHorizontally
      ) {
        SidebarContent(pageIndex)
      }
    }
  }

  @Composable
  fun PidAdjustButton(
    text: String,
    textColor: Color,
    onClick: () -> Unit,
    enabled: Boolean = true
  ) {
    val bgColor = if (enabled) Color.LightGray else Color.Gray
    val effectiveOnClick : () -> Unit = if (enabled) onClick else {->Log.d(this::class.simpleName, "Disabled")}

    Button(
      onClick = effectiveOnClick,
      colors = ButtonDefaults.buttonColors(containerColor = bgColor),
      shape = RoundedCornerShape(8.dp),
      contentPadding = PaddingValues(0.dp),
      modifier = Modifier
        .height(36.dp)
        .width(36.dp)
    ) {
      Text(
        text = text,
        fontSize = 24.sp,
        color = textColor,
        fontWeight = FontWeight.Bold
      )
    }
  }

  @Composable
  private fun PidPage() {
    Column(
      modifier = Modifier.fillMaxSize().padding(8.dp),
      verticalArrangement = Arrangement.spacedBy(16.dp),
      horizontalAlignment = Alignment.CenterHorizontally
    ) {
      pidValues = remember { mutableStateOf(PIDValues()) }
      val setPidAction = {
        serialWorker?.enqueue(SetPIDSerialUnit(
          pidValues.value.pidType!!,
          pidValues.value.p,
          pidValues.value.i,
          pidValues.value.d
        )) }
      Row(
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.spacedBy(16.dp)
      ) {
        val rollButtonColor = if (pidValues.value.pidType == PIDType.ROLL) Color.Red else Color.DarkGray
        LargeButton("Roll", rollButtonColor, Color.White, {
          pidValues.value = PIDValues(PIDType.ROLL)
          serialWorker?.enqueue(GetPIDSerialUnit(PIDType.ROLL))
        })
        val speedButtonColor =
          if (pidValues.value.pidType == PIDType.SPEED) Color.Red else Color.DarkGray
        LargeButton("Speed", speedButtonColor, Color.White, {
          pidValues.value = PIDValues(PIDType.SPEED)
          serialWorker?.enqueue(GetPIDSerialUnit(PIDType.SPEED))
        })
      }

      PidValueRow(pidValues, {pv -> pv.p}, {inc ->
        pidValues.value = pidValues.value.incP(inc)
        setPidAction.invoke()
      })
      PidValueRow(pidValues, {pv -> pv.i}, {inc ->
        pidValues.value = pidValues.value.incI(inc)
        setPidAction.invoke()
      })
      PidValueRow(pidValues, {pv -> pv.d}, {inc ->
        pidValues.value = pidValues.value.incD(inc)
        setPidAction.invoke()
      })
    }
  }

  @Composable
  private fun PidValueRow(pidValues: MutableState<PIDValues>, valueExt: (PIDValues)->Float, incFunc: (Float)->Unit) {
    Row(
      verticalAlignment = Alignment.CenterVertically,
      horizontalArrangement = Arrangement.spacedBy(8.dp)
    ) {
      val enabled = pidValues.value.initialized();
      PidAdjustButton("--", Color.Black, {
        incFunc.invoke(-0.1f)
      }, enabled = enabled)
      PidAdjustButton("-", Color.Black, {
        incFunc.invoke(-0.01f)
      }, enabled = enabled)
      FloatingPointBox(number = valueExt.invoke(pidValues.value))
      PidAdjustButton("+", Color.Black, {
        incFunc.invoke(0.01f)
      }, enabled = enabled)
      PidAdjustButton("++", Color.Black, {
        incFunc.invoke(0.1f)
      }, enabled = enabled)
    }
  }

  @Composable
  private fun SidebarContent(
    page : MutableState<Int>
  ) {
    val showDialog = remember { mutableStateOf(false) }
    connectionStatus = remember { mutableStateOf(ConnectionStatus()) }

    if (showDialog.value) {
      FullScreenDialogExample({ showDialog.value = false })
    }

    ConnectionBar(connectionStatus)

    SidebarRow {
      SimpleButton({
        connectBluetooth()
        createAndStartSerialWorker()
        connectionStatus.value = ConnectionStatus().toConnected()
        startChartRenderer()
      }, Icons.Default.Call)
      SimpleButton({
        stopChartRendererRoutine()
        serialWorker?.stop()
        btConnection?.close()
      }, Icons.Default.Close)
      SimpleButton({ stepChartConfig(-1) }, Icons.Default.ArrowBack)
      SimpleButton({ stepChartConfig(1)}, Icons.Default.ArrowForward)
    }

    SidebarRow {
      SimpleButton(
        onClick = {
          serialWorker?.enqueue(MotorToggleSerialUnit())
        }, imageVector = Icons.Filled.Lock
      )
      SimpleButton(onClick = {
        page.value = 1;
      }, Icons.Filled.Build)
      SimpleButton({
        page.value = 0;
      }, Icons.Filled.Home)
      SimpleButton({
        showDialog.value = true
      }, Icons.Default.MoreVert)
    }

    //TODO: Remove
    SidebarRow {
      SimpleButton({
        startChartRenderer()
        startSinGenerator()
        connectionStatus.value = ConnectionStatus().toConnected()
      }, Icons.Default.PlayArrow)
    }


    Box(modifier = Modifier.fillMaxHeight()) {
      Joystick(modifier = Modifier.align(Alignment.Center), onMove = { x, y ->
        serialWorker?.enqueueAndRemoveOthers(ControlSerialUnit(x, y))
        Log.i(this::class.simpleName, "Joystick: $x, $y")
      })
    }
  }

  @Composable
  private fun SidebarRow(content: @Composable RowScope.() -> Unit) {
    Row(
      modifier = Modifier.fillMaxWidth().padding(0.dp),
      verticalAlignment = Alignment.CenterVertically,
      horizontalArrangement = Arrangement.SpaceAround,
      content = content
    )
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
            SimpleButton({ onDismiss() }, Icons.Default.Notifications)
            SimpleButton({ onDismiss() }, Icons.Default.Face)
            SimpleButton({ onDismiss() }, Icons.Default.Face)
            SimpleButton({ onDismiss(); sendUnit(TriggerSerialUnit(TriggerType.FEELING_LOVE)) }, Icons.Default.Favorite)
            SimpleButton({ onDismiss() }, HornSpeakerWithHandleSvgrepoCom)
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

  private fun createAndStartSerialWorker() {
    if (btConnection == null) {
      return;
    }
    serialWorker?.stop()
    serialWorker = serialWorkerFactory.create(btConnection!!)
    serialWorker!!.subscribe(DiagDataSerialUnit::class.java) {
      CoroutineScope(Dispatchers.Default).launch {
        chartDataBacklog.add(it as DiagDataSerialUnit)
        Log.i(this::class.simpleName, "Added to backlog")
      }
    }
    serialWorker!!.subscribe(GetPIDResponseSerialUnit::class.java) {
      it as GetPIDResponseSerialUnit
      pidValues.value = PIDValues(it.type, it.p, it.i, it.d)
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
    serialWorker?.stop();
    try {
      btConnection?.close();
    } catch (e: Throwable) {
      Log.e(this::class.simpleName, "Could not close BT connection", e)
    }
    btConnection = null;
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
