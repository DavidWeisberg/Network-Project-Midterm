import org.apache.http.*;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpHead;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.impl.client.DefaultHttpClient;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.*;
import java.util.LinkedHashMap;
import java.util.Map;

public class ProxyServer {


    private static String getRequestURL(String requestLine) {
        return requestLine.substring(requestLine.indexOf(" ") + 1, requestLine.lastIndexOf(" "));
    }

    private static String getRequestMethod(String requestLine) {
        return requestLine.substring(0, requestLine.indexOf(" "));
    }

    public static void main(String[] args) {

        try {
            ServerSocket serverSocket = new ServerSocket(0); // Sets port #

            System.out.println(serverSocket.getLocalPort());    // Display port number

            // Iterative server
            while (true) {
                Socket clientSocket = serverSocket.accept();    // Accepting client connection.

                // Input from Browser
                BufferedReader in = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

                String requestLine = in.readLine(); // The Request-Line of the HTTP Request
                if (requestLine == null) {
//                    System.out.println("No Request-Line");
                    continue;
                }

                String requestMethod = getRequestMethod(requestLine);
                String requestURL = getRequestURL(requestLine);

                // The line that will display the host address, HTTP method, and the requested URL.
                StringBuffer lineOne = new StringBuffer("");
                String clientAddress = clientSocket.getInetAddress().toString();
                if (!clientAddress.startsWith("/"))
                    lineOne.append(clientAddress);
                else
                    lineOne.append(clientAddress.substring(1));
                lineOne.append(":\t");
                lineOne.append(requestMethod);
                lineOne.append(" ");
                lineOne.append(requestURL);
                System.out.println(lineOne);

                if (requestMethod.equals("GET") || requestMethod.equals("HEAD") || requestMethod.equals("POST")) {
                    // Headers of the HTTP Request
                    LinkedHashMap<String, String> requestHeadersMap = new LinkedHashMap<String, String>();
                    while (true) {      // Keep looping to read multiple lines from client
                        String requestHeaderLine = in.readLine();
                        if (requestHeaderLine.equals(""))
                            break;      // Break whenever we reach the end of headers
                        String header_KeyValue[] = requestHeaderLine.split(":");
                        String key = header_KeyValue[0].trim();
                        String value = header_KeyValue[1].trim();
                        requestHeadersMap.put(key, value);
                    }

                    // Output to client
                    PrintWriter out = new PrintWriter(clientSocket.getOutputStream(), true);

                    try {
                        // Connect to the requested URL
                        HttpClient httpClient = new DefaultHttpClient();
                        //Request
                        HttpRequest httpRequest;
                        if (requestMethod.equals("GET"))
                            httpRequest = new HttpGet(requestURL);
                        else if (requestMethod.equals("POST"))
                            httpRequest = new HttpPost(requestURL);
                        else
                            httpRequest = new HttpHead(requestURL);
                        // Add Request headers
                        for (Map.Entry<String, String> entry : requestHeadersMap.entrySet()) {
                            if (entry.getKey().equals("Host") || entry.getKey().equals("Accept-Encoding"))
                                continue;
                            httpRequest.addHeader(entry.getKey(), entry.getValue());
                        }

                        // Response
                        HttpResponse httpResponse = httpClient.execute((HttpUriRequest) httpRequest);
                        // Response status line
                        StatusLine responseStatusLine = httpResponse.getStatusLine();
                        out.write(responseStatusLine.toString());
                        out.write("\r\n");
                        if (responseStatusLine.getStatusCode() != HttpStatus.SC_OK) {
                            /*System.out.println("Http Response: " + responseStatusLine.getStatusCode() +
                                    " " + responseStatusLine.getReasonPhrase());*/
                            continue;
                        }

                        // Response Headers
                        for (Header responseHeader : httpResponse.getAllHeaders()) {
                            if (responseHeader.getName().equals("Transfer-Encoding"))
                                continue;
//                            System.out.println(responseHeader);
                            out.write(responseHeader.toString());
                            out.write("\r\n");
                        }
                        out.write("\r\n");

                        // Response content
                        HttpEntity httpEntity = httpResponse.getEntity();
                        BufferedReader bufferedReader = new BufferedReader(
                                new InputStreamReader(httpEntity.getContent()));
                        int responseContentCharacter;
                        while ((responseContentCharacter = bufferedReader.read()) != -1) {
                            out.write((char) responseContentCharacter);
                        }

                    } catch (ConnectException e) {
//                        System.out.println("Error connecting to the server");
                    } catch (UnknownHostException e) {
//                        System.out.println("Error connecting to the server");
                    } catch (ClientProtocolException e) {
//                        System.out.println("Client Protocol Exception");
                    } catch (SocketException e) {
//                        System.out.println("Socket Exception");
                    } catch (IllegalArgumentException e) {
//                        System.out.println("Illegal Argument Exception");
                    } finally {
                        out.close();            // Closing the output stream
                    }
                } else {
                    /*System.out.println("Request method is " + requestMethod + ". The server does not" +
                            " handle this method.");*/
                }

                in.close();             // Closing the input stream
                clientSocket.close();   // Closing the client socket.
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
