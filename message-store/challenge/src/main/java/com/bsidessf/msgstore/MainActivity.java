package com.bsidessf.msgstore;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.bsidessf.msgstore.models.Message;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import android.os.Bundle;
import android.content.Context;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.util.HashMap;
import java.util.Map;

public class MainActivity extends AppCompatActivity {
    private FirebaseAuth mAuth;
    Context context;
    private static String uid = null;
    private static String email = null;
    private DatabaseReference mDB;
    private static String dbPath = "users";
    private static String dbMsg = "msg";
    private static String dbEmail = "email";
    private static TextView currentMsg = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        context = this;

        // Initialize Firebase Auth and DB
        mAuth = FirebaseAuth.getInstance();
        mDB = FirebaseDatabase.getInstance().getReference();
        uid = mAuth.getCurrentUser().getUid();
        email = mAuth.getCurrentUser().getEmail();

        // References to UI elements
        EditText newMsg = (EditText) findViewById(R.id.msg);
        currentMsg = (TextView) findViewById(R.id.msgValue);
        Button button = (Button) findViewById(R.id.submit);

        //Fetch message
        getMessage();

        // Write a new message
        button.setOnClickListener( new View.OnClickListener(){
            @Override
            public void onClick(View v) {
                String msgStr = newMsg.getText().toString();
                mDB.child(dbPath).child(uid).child(dbEmail).setValue(email);
                mDB.child(dbPath).child(uid).child(dbMsg).setValue(msgStr);
                getMessage();
            }
        });
    }

    private void getMessage(){
        mDB.child(dbPath).child(uid).child(dbMsg).get().addOnCompleteListener(new OnCompleteListener<DataSnapshot>() {
            @Override
            public void onComplete(@NonNull Task<DataSnapshot> task) {
                if (!task.isSuccessful()) {
                    Log.e("firebase", "Error getting data", task.getException());
                }
                else {
                    String msgStr = String.valueOf(task.getResult().getValue());
                    Log.d("Fetched Message",msgStr);
                    currentMsg.setText(msgStr);
                }
            }
        });
    }

}