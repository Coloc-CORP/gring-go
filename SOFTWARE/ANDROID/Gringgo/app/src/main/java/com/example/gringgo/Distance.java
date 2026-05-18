package com.example.gringgo;

import android.graphics.Color;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import com.example.gringgo.ViewModel.BluetoothViewModel;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link Distance#newInstance} factory method to
 * create an instance of this fragment.
 */
public class Distance extends Fragment {

    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    public Distance() {
        // Required empty public constructor
    }
    private BluetoothViewModel bluetoothViewModel;


    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @param param1 Parameter 1.
     * @param param2 Parameter 2.
     * @return A new instance of fragment Distance.
     */
    // TODO: Rename and change types and number of parameters
    public static Distance newInstance(String param1, String param2) {
        Distance fragment = new Distance();
        Bundle args = new Bundle();
        args.putString(ARG_PARAM1, param1);
        args.putString(ARG_PARAM2, param2);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            mParam1 = getArguments().getString(ARG_PARAM1);
            mParam2 = getArguments().getString(ARG_PARAM2);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_distance, container, false);
    }

    // Manipulation of buttons
    @Override
    public void onViewCreated(@NonNull View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        TextView tvDistance = view.findViewById(R.id.chiffre_distance); // Assure-toi d'avoir ces ID dans ton XML
        TextView tvStep = view.findViewById(R.id.chiffre_pas);
        //TextView tvBattery = view.findViewById(R.id.battery);

        bluetoothViewModel.getHeartRate().observe(getViewLifecycleOwner(), hr -> {
            tvDistance.setText(hr);
        });

        bluetoothViewModel.getHeartRate().observe(getViewLifecycleOwner(), hr -> {
            tvStep.setText(hr);
        });
    }
}