package com.bsidessf.msgstore.models;
import com.google.firebase.database.Exclude;
import com.google.firebase.database.IgnoreExtraProperties;

import java.util.HashMap;
import java.util.Map;

@IgnoreExtraProperties
public class Message {
    public String msgStr;
    public String email;
    public Message() {
        // Default constructor required for calls to DataSnapshot.getValue(Post.class)
    }
    public Message(String msgStr, String email) {
        this.msgStr = msgStr;
        this.email = email;
    }
    @Exclude
    public Map<String, Object> toMap() {
        HashMap<String, Object> result = new HashMap<>();
        result.put("message", msgStr);
        result.put("email",email);
        return result;
    }
}
