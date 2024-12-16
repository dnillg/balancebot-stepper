package com.dnillg.balancer.controlapp.bluetooth

import android.annotation.SuppressLint
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.util.Log
import com.dnillg.balancer.controlapp.serial.SerialInterface
import java.io.BufferedReader
import java.io.BufferedWriter
import java.io.InputStreamReader
import java.io.OutputStreamWriter
import java.util.UUID

class BtConnection (
    private val device : BluetoothDevice,
) : SerialInterface {

    private var consecutiveErrorCount: Long = 0;
    private var socket : BluetoothSocket = createSocket(device)
    private var bufferedReader: BufferedReader = BufferedReader(InputStreamReader(socket.inputStream))
    private var bufferedWriter: BufferedWriter = BufferedWriter(OutputStreamWriter(socket.outputStream))

    override fun reconnect() {
        closeResources();
        this.socket = createSocket(this.device);
        this.bufferedReader = BufferedReader(InputStreamReader(socket.inputStream));
        this.bufferedWriter = BufferedWriter(OutputStreamWriter(socket.outputStream));
    }

    private fun closeResources() {
        try {
            if (socket.isConnected) {
                socket.close();
            }
        } catch (e: Exception) {
            Log.e(this::class.simpleName, "Error closing socket", e)
        }
    }

    override fun isAlive() : Boolean {
        return socket.isConnected && consecutiveErrorCount <= 10;
    }

    override fun close() {
        closeResources()
    }

    override fun readLine(): String? {
        try {
            val line = bufferedReader.readLine()
            consecutiveErrorCount = 0;
            return line;
        } catch (e: Exception) {
            Log.e(this::class.simpleName, "Error reading line", e)
            consecutiveErrorCount++;
            return null;
        }
    }

    override fun writeLine(line: String) {
        if (!socket.isConnected) {
            this.reconnect();
        }
        bufferedWriter.write(line);
        bufferedWriter.newLine();
        bufferedWriter.flush();
    }

    companion object {
        private val ESP32_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

        @SuppressLint("MissingPermission")
        private fun createSocket(device: BluetoothDevice): BluetoothSocket {
            val socket = device.createRfcommSocketToServiceRecord(ESP32_UUID)
            socket.connect()
            return socket
        }
    }


}