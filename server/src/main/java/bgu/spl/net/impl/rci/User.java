package bgu.spl.net.impl.rci;

import java.util.concurrent.ConcurrentHashMap;

public class User {
    private int id;
    private String userName;
    private String password;
    private boolean active;
    private ConcurrentHashMap<Integer, String> subIdToTopic; // map subId to topics that user subscribe to

    public User(int id, String user, String pass) {
        this.id = id;
        this.userName = user;
        this.password = pass;
        this.active = false;
        subIdToTopic = new ConcurrentHashMap<>();
    }

    public void Connect() {
        active = true;
    }

    public void disconnect() {
        Info info = Info.getInstance();
        for (Integer subId : subIdToTopic.keySet()) {
            info.removeFromSub(subId, subIdToTopic.get(subId));
        }
        info.disconnect(id); // remove user from connectionToUser
        active = false;
        id = -1; // user dont have current connection
        subIdToTopic.clear();
        // when dissconnect, the user's subscriptions should be deleted (happens in removeSub)
    }

    public boolean isActive() {
        return active;
    }

    public String getUserName() {
        return userName;
    }

    public int getId() {
        return id;
    }

    public void addSub(int subId, String topic) {
        subIdToTopic.put(subId, topic);
    }

    public void removeSub(Integer subId, String topic) {
        subIdToTopic.remove(subId, topic);
    }

    public boolean checkPassword(String password_) {
        return password.equals(password_);
    }

    public String getTopicById(int subId) {
        return subIdToTopic.get(subId);
    }
}
