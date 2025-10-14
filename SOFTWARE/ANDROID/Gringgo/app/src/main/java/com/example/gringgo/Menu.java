package com.example.gringgo;


import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Bundle;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.Manifest;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Toast;


import java.util.ArrayList;
import java.util.List;
import java.util.Map;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Menu#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Menu extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    private static final int REQUEST_ENABLE_BT = 1; // You can use any unique integer here

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    private static final String TAG = "MenuFragment";



    // Permission members
    private static final String[] BLUETOOTH_PERMISSIONS = new String[]{
            Manifest.permission.BLUETOOTH_SCAN,
            Manifest.permission.BLUETOOTH_CONNECT,
            Manifest.permission.BLUETOOTH_ADVERTISE,
            Manifest.permission.ACCESS_COARSE_LOCATION,
            Manifest.permission.ACCESS_FINE_LOCATION
    };

    // Anonym class of the launcher of the permission
    private final ActivityResultLauncher<String[]> requestPermissionLauncher =
            registerForActivityResult(new ActivityResultContracts.RequestMultiplePermissions(),
                    result -> {
                        // Ici on récupère les résultats permission par permission
                        for (Map.Entry<String, Boolean> entry : result.entrySet()) {
                            String permission = entry.getKey();
                            Boolean isGranted = entry.getValue();
                            if (isGranted) {
                                Log.d("Permissions", permission + " accordée ");
                            } else {
                                Log.w("Permissions", permission + " refusée ");
                            }
                        }
                    });




    // Bluetooth members
    private ArrayAdapter<String> devicesAdapter;
    BluetoothAdapter bluetoothAdapter;
    private final List<BluetoothDevice> devicesList = new ArrayList<>();

    // Déclaration + instanciation + implémentation du receiver (classe anonyme)
    // Execution du onReceive seulement en ayant inscrit le receiver avec registerReceiver() + découverte avec startDiscovery()
    // Same as :
    //    public BluetoothReceiver(Context context,
    //                             ArrayAdapter<String> devicesAdapter,
    //                             List<BluetoothDevice> devicesList) {
    //        this.context = context;
    //        this.devicesAdapter = devicesAdapter;
    //        this.devicesList = devicesList;
    //    }
    //
    //    @Override
    //    public void onReceive(Context context, Intent intent) {
    //        String action = intent.getAction();
    //        if (BluetoothDevice.ACTION_FOUND.equals(action)) {
    //            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
    //
    //            if (ActivityCompat.checkSelfPermission(this.context,
    //                    Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
    //                return;
    //            }
    //
    //            if (device != null && device.getName() != null) {
    //                devicesList.add(device);
    //                devicesAdapter.add(device.getName() + "\n" + device.getAddress());
    //            }
    //        }
    //    }
    private final BroadcastReceiver receiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                if (ActivityCompat.checkSelfPermission(requireContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                    // TODO: Consider calling
                    //    ActivityCompat#requestPermissions
                    // here to request the missing permissions, and then overriding
                    //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                    //                                          int[] grantResults)
                    // to handle the case where the user grants the permission. See the documentation
                    // for ActivityCompat#requestPermissions for more details.
                    return;
                }
                if (device != null && device.getName() != null) {
                    devicesList.add(device);
                    devicesAdapter.add(device.getName() + "\n" + device.getAddress());
                }
            }
        }
    };

    // Handle the request to enable Bluetooth
    ActivityResultLauncher<Intent> requestEnableBluetoothLauncher = registerForActivityResult(
            new ActivityResultContracts.StartActivityForResult(),
            result -> {
                if (result.getResultCode() == Activity.RESULT_OK) {
                    // Bluetooth has been enabled by the user.
                    Log.d(TAG, "Bluetooth enabled by user.");
                    // You can now proceed with Bluetooth-related operations.
                    // For example, call a method to start scanning, connecting, etc.
                    // setupBluetoothOperations();
                } else {
                    // Bluetooth was not enabled by the user (or an error occurred).
                    Log.w(TAG, "Bluetooth not enabled by user or request denied.");
                    // Handle this case appropriately (e.g., show a message to the user,
                    // disable Bluetooth-dependent features).
                }
            });

    // Bluetooth device selected
    private BluetoothDevice selectedDevice;






    //Constructors

    public Menu() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment Menu.
     */
    // TODO: Rename and change types and number of parameters
    public static Menu newInstance(String param1, String param2) {
        Menu fragment = new Menu();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }


    // Sensitive methods

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }
    }

    // Creation of the filter + subscription to the receiver
    @Override
    public void onStart() {
        super.onStart();
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        requireContext().registerReceiver(receiver, filter);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_menu, container, false);
    }

    //Execute every return on the app
    @Override
    public void onResume() {
        super.onResume();

        // Check and request permissions
        checkAndRequestPermissions();

        //Verify bluetooth connect bluetooth scan and blue

        // Stop execution si les permissions ne sont pas accordées
        if ((ContextCompat.checkSelfPermission(requireContext(), Manifest.permission.BLUETOOTH_CONNECT)
                != PackageManager.PERMISSION_GRANTED) ||
                (ContextCompat.checkSelfPermission(requireContext(), Manifest.permission.BLUETOOTH_SCAN)
                        != PackageManager.PERMISSION_GRANTED)
                ) {
            Log.d("Permissions", "Manque 1 ou plusieurs permissions");
            return;
        }

        activeBluetooth();
    }

    // Unsubscription of the receiver
    @Override
    public void onStop() {
        super.onStop();
        requireContext().unregisterReceiver(receiver);
    }

    // Manipulation of buttons
    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        Button button = (Button) view.findViewById(R.id.button);

        if (button != null) {
            button.setOnClickListener(new View.OnClickListener() {
                public void onClick(View v) {
                    Log.d("BUTTONS", "User tapped the Supabutton in Menu Fragment (onViewCreated)");
                    // Autres actions
                    showBluetoothPopup();
                }
            });
        } else {
            Log.e(TAG, "Button 'supabutton' not found in fragment_menu.xml");
        }
    }




    // Methods "function"

    // Check and request permissions
    private void checkAndRequestPermissions() {
        List<String> permissionsToRequest = new ArrayList<>();

        for (String permission : BLUETOOTH_PERMISSIONS) {
            if (ContextCompat.checkSelfPermission(requireContext(), permission)
                    != PackageManager.PERMISSION_GRANTED) {
                permissionsToRequest.add(permission);
            }
        }

        if (!permissionsToRequest.isEmpty()) {
            requestPermissionLauncher.launch(permissionsToRequest.toArray(new String[0]));
        } else {
            Log.d("Permissions", "Toutes les permissions déjà accordées");
        }
    }

    // Check if bluetooth is activated
    // If not, enable it
    private void activeBluetooth() {

        // Set bluetooth adapter
        BluetoothManager bluetoothManager = requireContext().getSystemService(BluetoothManager.class);
        assert bluetoothManager != null;
        bluetoothAdapter = bluetoothManager.getAdapter();
        if (bluetoothAdapter == null) {
            // Device doesn't support Bluetooth
        }

        // Make sure bluetooth is activated
        assert bluetoothAdapter != null;
        if (!bluetoothAdapter.isEnabled()) {
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            requestEnableBluetoothLauncher.launch(enableBtIntent);
        }


    }

    // Display bluetooth device finder pop up
    public void showBluetoothPopup() {
        devicesAdapter = new ArrayAdapter<>(requireContext(),
                android.R.layout.simple_list_item_1);

        BluetoothManager bluetoothManager = (BluetoothManager) requireContext().getSystemService(Context.BLUETOOTH_SERVICE);
        BluetoothAdapter bluetoothAdapter;

        if (bluetoothManager != null) {
            bluetoothAdapter = bluetoothManager.getAdapter();
        } else {
            bluetoothAdapter = null;
        }

        // Lance la découverte
        if (ActivityCompat.checkSelfPermission(requireContext(), Manifest.permission.BLUETOOTH_SCAN) != PackageManager.PERMISSION_GRANTED) {
            // TODO: Consider calling
            //    ActivityCompat#requestPermissions
            // here to request the missing permissions, and then overriding
            //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
            //                                          int[] grantResults)
            // to handle the case where the user grants the permission. See the documentation
            // for ActivityCompat#requestPermissions for more details.
            return;
        }
        bluetoothAdapter.startDiscovery();

        AlertDialog.Builder builder = new AlertDialog.Builder(requireContext());
        builder.setTitle("Appareils Bluetooth");
        builder.setAdapter(devicesAdapter, (dialog, which) -> {
            // Quand on clique sur un appareil
            this.selectedDevice = devicesList.get(which);
            if (ActivityCompat.checkSelfPermission(requireContext(), Manifest.permission.BLUETOOTH_CONNECT) != PackageManager.PERMISSION_GRANTED) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                return;
            }
            Toast.makeText(requireContext(),
                    "Sélectionné : " + this.selectedDevice.getName(),
                    Toast.LENGTH_SHORT).show();

            // Ici tu peux lancer la connexion avec selectedDevice
        });
        builder.setNegativeButton("Annuler", (dialog, which) -> {
            bluetoothAdapter.cancelDiscovery();
            dialog.dismiss();
        });
        builder.show();
    }

    //Start bluetooth socket
    public void startBluetoothSocket() {
        ConnectThread connectThread = new ConnectThread(this.selectedDevice,requireContext(),bluetoothAdapter);
    }


}

