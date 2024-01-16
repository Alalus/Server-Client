package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.rci.Info;
import bgu.spl.net.impl.rci.User;
import bgu.spl.net.srv.Connections;

public class StompMessagingProtocolImpl<T> implements StompMessagingProtocol<T> {

    private boolean terminate;
    private int connectionId;
    private ConnectionsImpl<T> connections;

    @Override
    public void start(int connectionId, Connections<T> connections) {
        terminate = false;
        this.connectionId = connectionId;
        this.connections = (ConnectionsImpl<T>) connections;

    }

    // proccess by using Connections
    @Override
    public void process(String message) {
        message = message.substring(1);
        Info info = Info.getInstance();
        int end = message.indexOf('\n');

        String messageType = message.substring(0, end);
        // (*) need to check in every message: if containsReciept = true -> send to
        // client receipt frame

        if (messageType.equals("CONNECT")) {

            String username = getValue(message, "login");
            String password = getValue(message, "passcode");
            User user = info.getUserByUsername(username);

            if (user != null) { // user exist
                if (user.isActive()) { // user already logged in
                    String problem = "User already logged in";
                    connections.send(connectionId, error(message, getReceiptId(problem), problem));
                    terminate = true;
                } else { // user doesnt have active connection
                    if (user.checkPassword(password)) { // connect
                        connections.send(connectionId, connected());
                        user.Connect();
                    } else { // incorrect password
                        String problem = "Wrong password";
                        connections.send(connectionId, error(message, getReceiptId(message), problem));
                        terminate = true;
                    }
                }
            } else { // user doesnt exist - create user

                User newUser = new User(connectionId, username, password);

                connections.send(connectionId, connected());
                info.logIn(newUser, connectionId);
            }

        } else if (messageType.equals("DISCONNECT")) {

            // disconnection succssesful -> server need to sent a reciept to the client
            // just if all the clients messages processed by server

            connections.send(connectionId, receipt(message));
            disconnect(connectionId);

        }

        else if (messageType.equals("SEND")) { // new message frame from server

            String username = getValue(message, "user");
            User user = info.getUserByUsername(username);
            String topic = (getValue(message, "destination")).substring(1);
            if (user != null) {
                if (info.topicExist(topic) && info.getTopicSubs(topic).contains(user)) { // topic exist & user subscribe
                                                                                         // to it
                    connections.send(topic, message);
                } else { // cant send message to topic user isnt subscribe to
                    // send error frame
                    String problem = "Cant send message to this topic because not subscribe to it";
                    String error = error(message, getReceiptId(message), problem);
                    connections.send(connectionId, error);
                    terminate = true;
                }
            }

        }

        else if (messageType.equals("SUBSCRIBE")) {

            String id = getValue(message, "id");
            User user = info.getUserBySubId(Integer.parseInt(id));
            String topic = (getValue(message, "destination")).substring(1);
            boolean flag = false;
            String problem = "Cant subscribe user that doesnt exist";
            if (user != null) { // user exist
                flag = true;
                if (info.topicExist(topic) && info.getTopicSubs(topic).contains(user)) {
                    flag = false;
                    problem = "user already subscribe to this topic";
                }
            }
            if (flag) {
                Integer subId = Integer.parseInt(id);
                info.addToSub(subId, user, topic);
                System.out.println("receipt sent from server: " + receipt(message));
                connections.send(connectionId, receipt(message));
            } else { // error
                String error = error(message, getReceiptId(message), problem);
                connections.send(connectionId, error);
                terminate = true;
            }
        }

        else if (messageType.equals("UNSUBSCRIBE")) {

            Integer subId = Integer.parseInt(getValue(message, "id"));
            User user = info.getUserById(connectionId);

            if (user == null) { // user doesn't exist
                String problem = "Cant unsubscribe user that doesnt exist";
                String error = error(message, getReceiptId(message), problem);
                connections.send(connectionId, error);
                terminate = true;
            } else { // user exist
                String topic = user.getTopicById(subId);
                if (topic != null) { // user is subscribe to this topic
                    info.removeFromSub(subId, topic);
                    connections.send(connectionId, receipt(message));
                } else {
                    String problem = "user is not subscribe to this topic, cant unsubscribe from it";
                    String error = error(message, getReceiptId(message), problem);
                    connections.send(connectionId, error);
                    terminate = true;
                }
            }
        }
    }

    public int getConnectionId() {
        return connectionId;
    }

    public String getValue(String msg, String value) {
        int start = msg.indexOf(value);
        String ans = msg.substring(start);
        return ans.substring(ans.indexOf(":") + 1, ans.indexOf('\n'));
    }

    public String error(String msg, String receiptId, String problem) {
        String error = "ERROR\nmessage: malformed frame received\n";
        if (!receiptId.equals("")) { // if we want to add more information (receipt id)
            error = error + "receipt-id:" + receiptId + "\n";
        }
        if (!msg.equals("")) { // if we want body frame will include the message that caused the error
            error = error + "\n" + "The message:\n" + "----" + "\n" + msg + "----";
        }
        error = error + problem + "\n" + '\u0000';
        return error;
    }

    public boolean containsReceipt(String msg) {
        if (msg.contains("receipt"))
            return true;
        else
            return false;
    }

    public String getReceiptId(String msg) {
        String id = "";
        if (containsReceipt(msg)) {
            int start = msg.indexOf("receipt");
            String str = msg.substring(start);
            id = str.substring(str.indexOf(":") + 1, str.indexOf('\n'));
        }
        return id;
    }

    public String receipt(String msg) {
        String ans = "RECEIPT\nreceipt-id:" + getReceiptId(msg) + "\n\n" + '\u0000';
        return ans;
    }

    public String connected() {
        String ans = "CONNECTED\nversion:1.2" + "\n\n" + '\u0000';
        return ans;
    }

    public void disconnect(int connectionId) {
        Info info = Info.getInstance();
        User user = info.getUserById(connectionId);
        if (user != null)
            user.disconnect();
        connections.disconnect(connectionId); // remove connectionId from activeClients
        terminate = true;
    }

    @Override
    public boolean shouldTerminate() {
        return terminate;
    }

}