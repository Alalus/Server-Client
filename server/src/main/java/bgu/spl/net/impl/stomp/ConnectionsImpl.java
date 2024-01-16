package bgu.spl.net.impl.stomp;

import bgu.spl.net.srv.Connections;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import bgu.spl.net.impl.rci.Info;
import bgu.spl.net.impl.rci.User;
import bgu.spl.net.srv.ConnectionHandler;

public class ConnectionsImpl<T> implements Connections<T> {

    // map between active client id to a connectionHandler
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> activeClients;

    @Override
    public boolean send(int connectionId, String msg) {

        if (activeClients.containsKey((Integer) connectionId)) {
            (activeClients.get((Integer) connectionId)).send(msg);
            return true;
        }
        return false;
    }

    @Override
    public void send(String channel, String msg) {

        Info info = Info.getInstance();
        ConcurrentLinkedQueue<Integer> subsIds = info.getTopicSubIds(channel);
        if (subsIds != null) {
            for (Integer sId : subsIds) {
                String message = message(msg, sId, info.getMId(), channel);
                User user = info.getUserBySubId(sId);
                int connectionId = user.getId();
                (activeClients.get((Integer) connectionId)).send(message); // send message to client with connectionId
                                                                           // by his ConnectionHandler
            }
        }
    }

    @Override
    public void disconnect(int connectionId) {

        if (activeClients != null && !activeClients.isEmpty()) {
            if (activeClients.containsKey((Integer) connectionId)) {
                activeClients.remove((Integer) connectionId);
            }
        }
    }

    @Override
    public void addConnection(int conId, ConnectionHandler<T> ch) {
        if (activeClients == null) {
            activeClients = new ConcurrentHashMap<Integer, ConnectionHandler<T>>();
        }
        activeClients.put((Integer) conId, ch);
    }

    public ConcurrentHashMap<Integer, ConnectionHandler<T>> getActiveClients() {
        return activeClients;
    }

    public String message(String msg, Integer sId, Integer mId, String topic) {
        String ans = "MESSAGE\nsubscription:" + sId + "\nmessage-id:" + mId + "\ndestination:"
                + topic + "\n\n" + getBody(msg) + "\n" + '\u0000';
        return ans;
    }

    public String getBody(String msg) {
        String start1 = msg.substring(msg.indexOf('/'));
        String start2 = start1.substring(start1.indexOf('\n') + 2);
        return start2.substring(0, start2.length() - 2); // without "\n" + '\u0000'
    }

}