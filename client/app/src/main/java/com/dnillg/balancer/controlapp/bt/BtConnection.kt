package com.dnillg.balancer.controlapp.bt

import android.annotation.SuppressLint
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothSocket
import android.util.Log
import java.io.BufferedReader
import java.io.BufferedWriter
import java.io.InputStreamReader
import java.io.OutputStreamWriter
import java.util.UUID

interface IBtConnection

class BtConnection(
    private val device : BluetoothDevice,
) : IBtConnection {

    private var consecutiveErrorCount: Long = 0;
    private var socket : BluetoothSocket = createSocket(device)
    private var bufferedReader: BufferedReader = BufferedReader(InputStreamReader(socket.inputStream))
    private var bufferedWriter: BufferedWriter = BufferedWriter(OutputStreamWriter(socket.outputStream))

    fun reconnect() {
        this.socket = createSocket(this.device);
        this.bufferedReader = BufferedReader(InputStreamReader(socket.inputStream));
        this.bufferedWriter = BufferedWriter(OutputStreamWriter(socket.outputStream));
    }

    fun isAlive() : Boolean {
        return socket.isConnected && consecutiveErrorCount <= 10;
    }

    fun close() {
        if (socket.isConnected) {
            socket.close();
        }
    }

    fun readLine(): String? {
        try {
            val line = bufferedReader.readLine()
            consecutiveErrorCount = 0;
            return line;
        } catch (e: Exception) {
            Log.e("BtConnection", "Error reading line", e)
            consecutiveErrorCount++;
            return null;
        }
    }

    fun writeLine(line: String) {
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