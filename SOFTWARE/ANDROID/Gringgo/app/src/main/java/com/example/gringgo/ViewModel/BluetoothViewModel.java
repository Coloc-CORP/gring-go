package com.example.gringgo.ViewModel;

import android.bluetooth.BluetoothDevice;

import androidx.lifecycle.LiveData;
import androidx.lifecycle.MutableLiveData;
import androidx.lifecycle.ViewModel;

import java.util.ArrayList;
import java.util.List;

public class BluetoothViewModel extends ViewModel {
    private final MutableLiveData<List<BluetoothDevice>> devices = new MutableLiveData<>(new ArrayList<>());
    private final MutableLiveData<BluetoothDevice> selectedDevice = new MutableLiveData<>();

    public LiveData<List<BluetoothDevice>> getDevices() {
        return devices;
    }

    public LiveData<BluetoothDevice> getSelectedDevice() {
        return selectedDevice;
    }

    public void addDevice(BluetoothDevice device) {
        List<BluetoothDevice> current = devices.getValue();
        if (current != null && !current.contains(device)) {
            current.add(device);
            devices.setValue(current);
        }
    }

    public void selectDevice(BluetoothDevice device) {
        selectedDevice.setValue(device);
    }
}