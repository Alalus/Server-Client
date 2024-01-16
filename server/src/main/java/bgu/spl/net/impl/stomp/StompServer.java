package bgu.spl.net.impl.stomp;
import bgu.spl.net.api.StompMessageEncoderDecoder;
import bgu.spl.net.srv.Server;

public class StompServer {


    public static void main(String[] args) {

         // you can use any server... 
        //  int port = 7777;

        int port = Integer.parseInt(args[0]);
        String serverType = args[1];

        if(serverType.equals("tpc")){
            Server.threadPerClient(
                port, //port
                () -> new StompMessagingProtocolImpl<>(), 
                () -> new StompMessageEncoderDecoder()).serve(); //stomp message encoder decoder factory

        }
        else{ // reactor
            Server.reactor(
            Runtime.getRuntime().availableProcessors(),
            port, //port
            () -> new StompMessagingProtocolImpl<>(), 
            () -> new StompMessageEncoderDecoder()).serve(); //stomp message encoder decoder factory
 
        }

    }
    
}
