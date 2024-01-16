package bgu.spl.net.srv;

import java.io.IOException;

//import java.io.IOException;


public interface Connections<T> {


    boolean send(int connectionId, String msg);

    void send(String channel, String msg);

    void disconnect(int connectionId) throws IOException;

    public void addConnection(int conId, ConnectionHandler<T> ch);
}
