package com.dnillg.balancer.controlapp

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
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.RowScope
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.systemBarsPadding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.ArrowBack
import androidx.compose.material.icons.filled.ArrowDropDown
import androidx.compose.material.icons.filled.ArrowForward
import androidx.compose.material.icons.filled.Build
import androidx.compose.material.icons.filled.Call
import androidx.compose.material.icons.filled.Close
import androidx.compose.material.icons.filled.Home
import androidx.compose.material.icons.filled.Lock
import androidx.compose.material.icons.filled.MoreVert
import androidx.compose.material.icons.filled.Refresh
import androidx.compose.material.icons.filled.Settings
import androidx.compose.material3.DropdownMenu
import androidx.compose.material3.DropdownMenuItem
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
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.dnillg.balancer.controlapp.bluetooth.BtConnection
import com.dnillg.balancer.controlapp.components.AdjustEnumValueRow
import com.dnillg.balancer.controlapp.components.AdjustIntegerValueRow
import com.dnillg.balancer.controlapp.components.ConnectionBar
import com.dnillg.balancer.controlapp.components.Joystick
import com.dnillg.balancer.controlapp.components.LargeButton
import com.dnillg.balancer.controlapp.components.SimpleButton
import com.dnillg.balancer.controlapp.components.SimpleImageButton
import com.dnillg.balancer.controlapp.components.TriggerSelectorDialog
import com.dnillg.balancer.controlapp.components.AdjustNumericValueRow
import com.dnillg.balancer.controlapp.domain.chart.TimeSeriesChartEntry
import com.dnillg.balancer.controlapp.domain.chart.TimeSeriesType
import com.dnillg.balancer.controlapp.domain.chart.chartConfigurations
import com.dnillg.balancer.controlapp.domain.model.ConnectionStatus
import com.dnillg.balancer.controlapp.domain.model.PIDType
import com.dnillg.balancer.controlapp.domain.model.PIDValues
import com.dnillg.balancer.controlapp.domain.model.Setting
import com.dnillg.balancer.controlapp.domain.model.SettingDoubleState
import com.dnillg.balancer.controlapp.domain.model.SettingEnumValue
import com.dnillg.balancer.controlapp.domain.model.SettingIntegerState
import com.dnillg.balancer.controlapp.domain.model.SettingState
import com.dnillg.balancer.controlapp.serial.SerialWorker
import com.dnillg.balancer.controlapp.serial.SerialWorkerFactory
import com.dnillg.balancer.controlapp.serial.model.ControlSerialUnit
import com.dnillg.balancer.controlapp.serial.model.DG1SerialUnit
import com.dnillg.balancer.controlapp.serial.model.DiagDataSerialUnit
import com.dnillg.balancer.controlapp.serial.model.GetConfigResponseSerialUnit
import com.dnillg.balancer.controlapp.serial.model.GetConfigSerialUnit
import com.dnillg.balancer.controlapp.serial.model.GetPIDResponseSerialUnit
import com.dnillg.balancer.controlapp.serial.model.GetPIDSerialUnit
import com.dnillg.balancer.controlapp.serial.model.MotorToggleSerialUnit
import com.dnillg.balancer.controlapp.serial.model.SerialUnit
import com.dnillg.balancer.controlapp.serial.model.SetConfigSerialUnit
import com.dnillg.balancer.controlapp.serial.model.SetPIDSerialUnit
import com.dnillg.balancer.controlapp.serial.model.TriggerSerialUnit
import com.dnillg.balancer.controlapp.serial.model.TriggerType
import com.dnillg.balancer.controlapp.serial.serialization.GetConfigSerialUnitSerializer
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
)

enum class ActivityPage {
  DIAGNOSTICS,
  PID_ADJUST,
  PARAM_ADJUST,
}

class MainActivity @Inject constructor() : ComponentActivity() {

  @Inject
  lateinit var serialWorkerFactory: SerialWorkerFactory

  private val asyncJobs: MainActivityAsyncJobs = MainActivityAsyncJobs()
  private val chartDataBacklog: ConcurrentSerialUnitBacklog<DiagDataSerialUnit> = ConcurrentSerialUnitBacklog()
  private val timeSeriesWindow = TimeSeriesWindow(
    duration = 5.0,
    entryCreator = { timeSeriesConfig, value -> TimeSeriesChartEntry(timeSeriesConfig, value) },
  )
  private lateinit var lineChart: LineChart;

  // Mutable Activity State
  private lateinit var connectionStatus: MutableState<ConnectionStatus>
  private lateinit var pidValues: MutableState<PIDValues>
  private lateinit var settingValueState : MutableState<SettingState<*>>
  private var motorsEnabled: MutableState<Boolean> = mutableStateOf(false)
  private var chartConfigIndex = 0;
  // Bluetooth
  private var serialWorker: SerialWorker? = null
  private var btConnection: BtConnection? = null

  init {
    TimeSeriesType.entries.forEach {
      timeSeriesWindow.init(it.alias, TIMESERIES_SAMPLES_PER_SECOND)
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
          LandscapeLayout()
        }
      }
    }
  }

  @Composable
  fun LandscapeLayout() {
    val page = remember { mutableStateOf(ActivityPage.DIAGNOSTICS) }

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
        when (page.value) {
          ActivityPage.DIAGNOSTICS -> StyledLineChart()
          ActivityPage.PID_ADJUST -> PidPage()
          ActivityPage.PARAM_ADJUST -> ParamsPage()
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
        SidebarContent(page)
      }
    }
  }

  @Composable
  private fun PidPage() {
    Column(
      modifier = Modifier
        .fillMaxSize()
        .padding(8.dp),
      verticalArrangement = Arrangement.spacedBy(16.dp),
      horizontalAlignment = Alignment.CenterHorizontally
    ) {
      pidValues = remember { mutableStateOf(PIDValues()) }
      val setPidAction = {
        serialWorker?.enqueue(
          SetPIDSerialUnit(
            pidValues.value.pidType!!,
            pidValues.value.p,
            pidValues.value.i,
            pidValues.value.d
          )
        )
      }
      Row(
        verticalAlignment = Alignment.CenterVertically,
        horizontalArrangement = Arrangement.spacedBy(16.dp)
      ) {
        val rollButtonColor =
          if (pidValues.value.pidType == PIDType.ROLL) Color.Red else Color.DarkGray
        LargeButton("Roll", rollButtonColor, Color.White) {
          pidValues.value = PIDValues(PIDType.ROLL)
          serialWorker?.enqueue(GetPIDSerialUnit(PIDType.ROLL))
        }
        val speedButtonColor =
          if (pidValues.value.pidType == PIDType.SPEED) Color.Red else Color.DarkGray
        LargeButton("Speed", speedButtonColor, Color.White) {
          pidValues.value = PIDValues(PIDType.SPEED)
          serialWorker?.enqueue(GetPIDSerialUnit(PIDType.SPEED))
        }
      }

      val enabled = pidValues.value.initialized()
      AdjustNumericValueRow(enabled, pidValues.value.p.toDouble(), { inc ->
        pidValues.value = pidValues.value.incP(inc)
        setPidAction.invoke()
      })
      AdjustNumericValueRow(enabled, pidValues.value.i.toDouble(), { inc ->
        pidValues.value = pidValues.value.incI(inc)
        setPidAction.invoke()
      })
      AdjustNumericValueRow(enabled, pidValues.value.d.toDouble(), { inc ->
        pidValues.value = pidValues.value.incD(inc)
        setPidAction.invoke()
      })
    }
  }

  @Composable
  fun Dropdown(options: List<String>, itemPosition: Int, onSelect: (Int) -> Unit) {

    val isDropDownExpanded = remember {
      mutableStateOf(false)
    }
    val openDropdown = {
      isDropDownExpanded.value = true
    }

    Column(
      modifier = Modifier.fillMaxSize(),
      horizontalAlignment = Alignment.CenterHorizontally,
      verticalArrangement = Arrangement.Top
    ) {


      Row(
        horizontalArrangement = Arrangement.Center,
        verticalAlignment = Alignment.CenterVertically,
        modifier = Modifier.clickable(onClick = openDropdown)
      ) {
        Box(
          contentAlignment = Alignment.CenterStart,
          modifier = Modifier
            .height(42.dp)
            .weight(1f)
            .background(Color.DarkGray, shape = RoundedCornerShape(4.dp)),
        ) {
          Text(
            text = options[itemPosition],
            color = Color.White,
            textAlign = TextAlign.Left,
            modifier = Modifier.padding(start = 8.dp)
          )
        }
        Box(Modifier.offset(x = -21.dp)) {
          SimpleButton(imageVector = Icons.Filled.ArrowDropDown, onClick = openDropdown)
        }
      }
      DropdownMenu(
        expanded = isDropDownExpanded.value,
        onDismissRequest = {
          isDropDownExpanded.value = false
        }) {
        options.forEachIndexed { index, name ->
          DropdownMenuItem(
            text = {
              Text(text = name)
            },
            onClick = {
              isDropDownExpanded.value = false
              onSelect(index)
              settingValueState.value = Setting.settings[index].createState()
            })
        }
      }
    }


  }

  @Composable
  private fun ParamsPage() {
    val itemPosition = remember {
      mutableIntStateOf(0)
    }
    settingValueState = remember { mutableStateOf(SettingIntegerState(Setting.settings[0])) }
    val options = Setting.settings.map { it.name }
    val selectSetting: (Int) -> Unit = { itemPosition.intValue = it }
    val sv = settingValueState.value
    Column(Modifier.fillMaxSize(), horizontalAlignment = Alignment.CenterHorizontally, verticalArrangement = Arrangement.spacedBy(12.dp)) {
      Box(Modifier.height(32.dp))
      Row(
        modifier = Modifier
          .height(42.dp)
          .fillMaxWidth()
      ) {
        Dropdown(options, itemPosition.intValue, selectSetting)
      }
      Row (modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.Center) {
        if (sv is SettingDoubleState) {
          AdjustNumericValueRow(enabled = sv.initialized, sv.value?:0.0, {settingValueState.value = sv.withMultipliedValue(1 + it)})
        } else if (sv is SettingIntegerState) {
          AdjustIntegerValueRow(enabled = sv.initialized, sv.value?:0, {settingValueState.value = sv.withAddedValue(it)}, inc1 = 1, inc2 = 10)
        } else if (sv is SettingEnumValue) {
          Text("...", color = Color.White)
          if (sv.value != null) {
            AdjustEnumValueRow(enabled = sv.initialized, sv.enumValue!!) { settingValueState.value = sv.withValue(it.name) }
          }
        }
      }
      Row (modifier = Modifier.fillMaxWidth(), horizontalArrangement = Arrangement.Center) {
        LargeButton(text = "Read", Color.White, Color.Black) { sendUnit(GetConfigSerialUnit(sv.setting.id)) }
        Spacer(modifier = Modifier.width(12.dp))
        LargeButton("Write", Color.Red, Color.Black, sv.initialized) { sendUnit(SetConfigSerialUnit(sv.setting.id, sv.value.toString())) }
      }
    }
  }

  @Composable
  private fun SidebarContent(
    page: MutableState<ActivityPage>
  ) {
    val showDialog = remember { mutableStateOf(false) }
    connectionStatus = remember { mutableStateOf(ConnectionStatus()) }

    if (showDialog.value) {
      TriggerSelectorDialog({ showDialog.value = false }, { sendUnit(it) })
    }

    ConnectionBar(connectionStatus)

    SidebarButtonRow {
      SimpleButton({
        connectBluetooth()
        if (btConnection != null) {
          createAndStartSerialWorker()
          val config = chartConfigurations[chartConfigIndex]
          serialWorker?.enqueue(TriggerSerialUnit(TriggerType.DIAGMODE, config.alias))
          connectionStatus.value = ConnectionStatus().toConnected()
          startChartRenderer()
        }
      }, Icons.Default.Call)
      SimpleButton({
        stopChartRendererRoutine()
        closeBluetoothConnection()
        connectionStatus.value = ConnectionStatus().toDisconnected()
      }, Icons.Default.Close)
      SimpleButton({ stepChartConfig(-1) }, Icons.Default.ArrowBack)
      SimpleButton({ stepChartConfig(1) }, Icons.Default.ArrowForward)
    }

    SidebarButtonRow {
      SimpleButton(onClick = {
        page.value = ActivityPage.PARAM_ADJUST;
      }, imageVector = Icons.Filled.Settings)
      SimpleButton(onClick = {
        page.value = ActivityPage.PID_ADJUST;
      }, Icons.Filled.Build)
      SimpleButton({
        page.value = ActivityPage.DIAGNOSTICS;
      }, Icons.Filled.Home)
      SimpleButton({
        showDialog.value = true
      }, Icons.Default.MoreVert)
    }

    SidebarButtonRow {
      //sinGeneratorButton()
      SimpleImageButton(
        { sendUnit(TriggerSerialUnit(TriggerType.ROBOT_FACE_STANDARD)) },
        painterResource(R.drawable.robot)
      )
      SimpleButton({
        motorsEnabled.value = !motorsEnabled.value
        sendUnit(MotorToggleSerialUnit(motorsEnabled.value))
      }, if (motorsEnabled.value) Icons.Filled.Lock else Icons.Filled.Refresh)
    }

    Box(modifier = Modifier.fillMaxHeight()) {
      Joystick(modifier = Modifier.align(Alignment.Center), onMove = { x, y ->
        if (x == 0f && y == 0f) {
          // Sending multiple times to ensure the zero value is sent without a transmission error
          for (i in 0..6) {
            sendUnit(ControlSerialUnit(0f, 0f))
          }
        } else {
          serialWorker?.enqueueAndDebounce(ControlSerialUnit(x, y), "ControlSerialUnit", 50)
        }
      })
    }
  }

  @Composable
  private fun SidebarButtonRow(content: @Composable RowScope.() -> Unit) {
    Row(
      modifier = Modifier
        .fillMaxWidth()
        .padding(0.dp),
      verticalAlignment = Alignment.CenterVertically,
      horizontalArrangement = Arrangement.SpaceAround,
      content = content
    )
  }

  private fun startChartRenderer() {
    asyncJobs.chartRenderer = CoroutineScope(Dispatchers.Main).launch {
      while (true) {
        consumeBacklogAndFillChart()
        lineChart.invalidate()
        delay(50);
      }
    }
  }

  private suspend fun consumeBacklogAndFillChart() {
    val items = chartDataBacklog.getAndClear()
    for (i in items) {
      timeSeriesWindow.addPoint(
        TimeSeriesType.ROLL.alias,
        Math.toDegrees(i.roll.toDouble()).toFloat() + 90f
      )
      timeSeriesWindow.addPoint(
        TimeSeriesType.TARGET_ROLL.alias,
        Math.toDegrees(i.targetRoll.toDouble()).toFloat() + 90f
      )
      timeSeriesWindow.addPoint(TimeSeriesType.SPEED.alias, i.speed)
      timeSeriesWindow.addPoint(TimeSeriesType.TARGET_SPEED.alias, i.targetSpeed)
      timeSeriesWindow.addPoint(TimeSeriesType.MOTOR_LEFT_SPEED.alias, i.motorLeft)
      timeSeriesWindow.addPoint(TimeSeriesType.MOTOR_RIGHT_SPEED.alias, i.motorRight)
    }
  }

  private fun stopChartRendererRoutine() {
    asyncJobs.chartRenderer?.cancel()
    asyncJobs.chartRenderer = null;
  }

  private fun stepChartConfig(offset: Int = 1) {
    chartConfigIndex =
      (chartConfigurations.size + chartConfigIndex + offset) % chartConfigurations.size
    val config = chartConfigurations[chartConfigIndex]
    serialWorker?.enqueue(TriggerSerialUnit(TriggerType.DIAGMODE, config.alias))

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
      }
    }
    serialWorker!!.subscribe(DG1SerialUnit::class.java) {
      CoroutineScope(Dispatchers.Default).launch {
        val dg1 = it as DG1SerialUnit
        chartDataBacklog.add(
          DiagDataSerialUnit(
            dg1.seqNo,
            dg1.roll,
            dg1.targetRoll,
            0.0f,
            0.0f,
            0.0f,
            0.0f
          )
        )
      }
    }
    serialWorker!!.subscribe(GetPIDResponseSerialUnit::class.java) {
      it as GetPIDResponseSerialUnit
      pidValues.value = PIDValues(it.type, it.p, it.i, it.d)
    }
    serialWorker!!.subscribe(GetConfigResponseSerialUnit::class.java) {
      it as GetConfigResponseSerialUnit
      val currentState = settingValueState.value
      if (currentState.setting.id.equals(it.name)) {
        if (currentState is SettingIntegerState) {
          settingValueState.value = currentState.withValue(it.value.toInt())
        } else if (currentState is SettingDoubleState) {
          settingValueState.value = currentState.withValue(it.value.toDouble())
        } else if (currentState is SettingEnumValue) {
          settingValueState.value = currentState.withValue(it.value)
        }
      }

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

      xAxis.axisMinimum = -1000f * CHART_TIME_SECONDS;
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
        val onLoss = {
          connectionStatus.value = ConnectionStatus().toUnhealthy()
        }
        val onRecovery = {
          connectionStatus.value = ConnectionStatus().toConnected()
        }
        btConnection = BtConnection(device, onLoss, onRecovery)
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
    serialWorker = null;
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

  private fun sendUnit(unit: SerialUnit) {
    serialWorker?.enqueue(unit);
  }

  companion object {
    private const val BLUETOOTH_PERMISSION_REQUEST_CODE = 1001;
    private const val TIMESERIES_SAMPLES_PER_SECOND = 40
    private const val CHART_TIME_SECONDS = 5
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
