package bgu.spl.net.srv;

import bgu.spl.net.api.StompMessageEncoderDecoder;
import bgu.spl.net.api.StompMessagingProtocol;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;

public class BlockingConnectionHandler<T> implements Runnable, ConnectionHandler<T> {

    private final StompMessagingProtocol<T> protocol;
    private final StompMessageEncoderDecoder encdec;
    private final Socket sock;
    private BufferedInputStream in;
    private BufferedOutputStream out;
    private volatile boolean connected = true;

    public BlockingConnectionHandler(
            Socket sock,
            StompMessageEncoderDecoder reader,
            StompMessagingProtocol<T> protocol) {

        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
    }

    @Override
    public void run() {
        try (Socket sock = this.sock) { // just for automatic closing
            int read;

            in = new BufferedInputStream(sock.getInputStream()); // read from socket
            out = new BufferedOutputStream(sock.getOutputStream()); // write to socket

            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0) { // read from socket
                String nextMessage = encdec.decodeNextByte((byte) read); // decode message byte by byte
                if (nextMessage != null) { // if we reached end of message
                    protocol.process(nextMessage); // process message and handle it by connections
                }
            }
            if (protocol.shouldTerminate()) {
                close();
            }

        } catch (IOException ex) {
            ex.printStackTrace();

        }

    }

    @Override
    public void close() throws IOException {
        int connectionId = protocol.getConnectionId();
        protocol.disconnect(connectionId);
        connected = false;

        try {
            sock.close();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }

    @Override
    public synchronized void send(String msg) {
        try {
            out.write(encdec.encode(msg));
            out.flush();
        } catch (IOException ex) {
            ex.printStackTrace();
        }

    }

}
