package com.example.gringgo;

import static android.content.ContentValues.TAG;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.pm.PackageManager;
import android.util.Log;
import android.Manifest;

import androidx.core.app.ActivityCompat;

import java.io.IOException;
import java.util.UUID;

class ConnectThread extends Thread {
    private final BluetoothSocket mmSocket;
    private final BluetoothDevice mmDevice;

    private final Context mmContext; // Variable pour garder le contexte
    private final BluetoothAdapter mmBluetoothAdapter; // Variable pour garder l'adaptateur
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    public ConnectThread(BluetoothDevice device, Context context, BluetoothAdapter bluetoothAdapter) {
        // Use a temporary object that is later assigned to mmSocket
        // because mmSocket is final.
        BluetoothSocket tmp = null;
        mmDevice = device;
        mmContext = context; // Assigner le contexte
        mmBluetoothAdapter = bluetoothAdapter; // Assigner l'adaptateur

        try {
            // Get a BluetoothSocket to connect with the given BluetoothDevice.
            // MY_UUID is the app's UUID string, also used in the server code.
            // Utilisez le contexte que vous avez passé (mmContext)
            if (ActivityCompat.checkSelfPermission(mmContext, Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                // Il est généralement préférable de vérifier les permissions AVANT de lancer le thread.
                // Loguer une erreur ici est une bonne pratique.
                Log.e(TAG, "Permission BLUETOOTH_CONNECT manquante. Impossible de créer le socket.");
                // Lancer une exception ou retourner null n'est pas possible directement ici.
                // On laisse mmSocket à null, ce qui sera géré plus tard.
            } else {
                tmp = device.createRfcommSocketToServiceRecord(MY_UUID);
            }
        } catch (IOException e) {
            Log.e(TAG, "La création du Socket a échoué", e);
        }
        mmSocket = tmp;
    }

    public void run() {
        // Cancel discovery because it otherwise slows down the connection.
        if (ActivityCompat.checkSelfPermission(mmContext, Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            return;
        }
        mmBluetoothAdapter.cancelDiscovery();

        try {
            // Connect to the remote device through the socket. This call blocks
            // until it succeeds or throws an exception.
            if (ActivityCompat.checkSelfPermission(mmContext, android.Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                return;
            }
            mmSocket.connect();
        } catch (IOException connectException) {
            // Unable to connect; close the socket and return.
            try {
                mmSocket.close();
            } catch (IOException closeException) {
                Log.e(TAG, "Could not close the client socket", closeException);
            }
            return;
        }

        // PUT SOME WORK AWAY
    }

    // Closes the client socket and causes the thread to finish.
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) {
            Log.e(TAG, "Could not close the client socket", e);
        }
    }
}