
import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Scanner;

/**
* Author: Zhirui wang, ID: 300491848
* Reference by Martin Ombura Jr. <@martinomburajr>
* https://medium.com/martinomburajr/java-create-your-own-hello-world-server-2ca33b6957e
*/
public class MyServer {
    public static void main(String[] args) {
        connectToServer();
    }

    public static void connectToServer() {

        //Try connect to the server on an unused port eg 5099. A successful connection will return a socket
        try(ServerSocket serverSocket = new ServerSocket(5000)) {
            
            //When a client connects to the server with right port, a socket is returned.        
            Socket connectionSocket = serverSocket.accept();

            String[] jokes = {
                "\nWhat do you call a boomerang that won’t come back? A stick.\n",
                "\nWhat do you call a cow with no legs? Ground beef.\n",
                "\nWhat time is it when the clock strikes 13? Time to get a new clock.\n",
                "\nWhat time is it when the clock strikes 13? Time to get a new clock.\n",
                "\nHow does a cucumber become a pickle? It goes through a jarring experience.\n"
            };

            //Create Input&Outputstreams for the connection
            InputStream inputToServer = connectionSocket.getInputStream();
            OutputStream outputFromServer = connectionSocket.getOutputStream();

            //Create a scanner to read from the client user
            Scanner scanner = new Scanner(inputToServer, "UTF-8");
            PrintWriter serverPrintOut = new PrintWriter(new OutputStreamWriter(outputFromServer, "UTF-8"), true);
            
            //the clinet will get a message from the server
            serverPrintOut.println(jokes[(int)Math.random() * jokes.length]);
            serverPrintOut.println("Type 'other one' to get more jokes or 'close' to close\n");
            //Have the server take input from the client and echo it back
            //This should be placed in a loop that listens for a terminator text e.g. bye
            boolean close = false;

            while(!close && scanner.hasNextLine()) {
                
                String line = scanner.nextLine();

                if(line.toLowerCase().trim().equals("close")) {
                    close = true;
                    serverPrintOut.println("Server Close, Thanks for useing my server~ ");

                }else if(line.toLowerCase().trim().equals("other one")){
                    int r = (int) (Math.random()*jokes.length);
                    String randomJoke = jokes[r];
                    serverPrintOut.println(randomJoke);
                    serverPrintOut.println("type 'other one' to get more jokes or 'close' to close\n");
                }else{
                    serverPrintOut.println("Can't understand command: " + line +
                                        "Type 'other one' to get more jokes or 'close' to close\n");
                }

            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}