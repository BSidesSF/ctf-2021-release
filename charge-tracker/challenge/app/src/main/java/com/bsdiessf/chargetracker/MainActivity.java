package com.bsdiessf.chargetracker;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.BatteryManager;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import android.util.Log;
import android.widget.ImageView;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    private TextView batteryValue;
    private String part2 = "R3charg3";
    private ImageView batteryImage;
    private BroadcastReceiver mBatteryReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            int level = intent.getIntExtra(BatteryManager.EXTRA_LEVEL, 0);
            String levelStr = String.valueOf(level);
            Log.d("Battery update:",levelStr);
            if(level == 49){
                Log.d("Flag:", getResources().getString(R.string.part1) + part2 + "1tN0w}");
            }
            batteryValue.setText(levelStr + "%");
            if(level > 79) {
                batteryImage.setImageResource(R.drawable.high);
            }
            else if (level > 30 && level < 80){
                batteryImage.setImageResource(R.drawable.med);
            }
            else{
                batteryImage.setImageResource(R.drawable.low);
            }
        }
    };
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
        batteryImage = (ImageView) findViewById(R.id.batteryImageView);
        // Set the battery percentage on first load
        batteryValue = (TextView) this.findViewById(R.id.percentValue);
        BatteryManager bm = (BatteryManager) this.getSystemService(BATTERY_SERVICE);
        int batLevel = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_CAPACITY);
        batteryValue.setText(String.valueOf(batLevel) + "%");
        // Register a receiver to update on change
        this.registerReceiver(this.mBatteryReceiver, new IntentFilter(Intent.ACTION_BATTERY_CHANGED));
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mBatteryReceiver);
    }
}