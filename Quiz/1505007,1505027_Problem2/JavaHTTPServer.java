package http;


import java.util.Date;
import java.io.FileNotFoundException;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.IOException;
import java.io.StringWriter;
import javafx.scene.chart.Chart;
import java.io.BufferedOutputStream;
import java.io.FileInputStream;import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.util.StringTokenizer;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.io.FileWriter;

public class JavaHTTPServer implements Runnable {

    File ROOT_FILE = new File(".");
    static final int PORT = 6789;
    

    private Socket conn;

    public JavaHTTPServer(Socket c) {
        conn = c;
        
    }

    public static void main(String[] args) {
        try {
            ServerSocket sConnect = new ServerSocket(PORT);
            System.out.println("\nListening connections on port : " + PORT + " ...\n");

            while (true) {
                JavaHTTPServer s = new JavaHTTPServer(sConnect.accept());

               
                System.out.println("Connecton opened. (" + new Date() + ")");
                
                Thread thread = new Thread(s);
                thread.start();
            }

        } catch (IOException e) {
            System.out.println("Server Connection error : " + e.getMessage());
        }
    }

    @Override
    public void run() {
        // we manage our particular client connection
        String fileRequested = null;
        String ip = new String();
        Date d =  new Date();
        BufferedReader in = null;
        PrintWriter out = null;
        BufferedOutputStream br = null;
        
        try {
            in = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            out = new PrintWriter(conn.getOutputStream());
            br = new BufferedOutputStream(conn.getOutputStream());
            
            String first = in.readLine();
            
            if(first!=null){
                
                char[] cbuf = new char[1000];
                in.read(cbuf);
                System.out.println(cbuf);
                String strreq = new String(cbuf);
                System.out.println(strreq);
                int beg = strreq.indexOf("Host: ");
                int end = strreq.indexOf("Connection");
                
                for(int i = beg+6; i<end-2;i++)
                {
                    ip = ip+ strreq.charAt(i);
                }
                
                System.out.println("Headers: "+first);
                StringTokenizer list = new StringTokenizer(first);
                String method = list.nextToken().toUpperCase(); 
                fileRequested = list.nextToken().toLowerCase();
                

                if (fileRequested.endsWith("/")) {
                    fileRequested += "index.html";
                }
                System.out.println(fileRequested);

                File file = new File(ROOT_FILE, fileRequested);
                String content = getContentType(fileRequested);
                int fileLength = (int) file.length();
                
                if(file.exists()){
                    if (method.equals("GET")) { 
                        byte[] fileData = readFileData(file, fileLength);

                        out.println("HTTP/1.1 200 OK");
                        out.println("Server: Java HTTP Server: 1.0");
                        out.println("Date: " + new Date());
                        out.println("Content-type: " + content);
                        out.println("Content-length: " + fileLength);
                        out.println(); 
                        out.flush(); 

                        br.write(fileData, 0, fileLength);
                        br.flush();
                        String date=""+d;
                        logfile(ip, date, first, "200", Integer.toString(fileLength));
                    }
                    else if (method.equals("POST")) { 
                        int i;
                        for(i = 0;i<cbuf.length-4;i++)
                        {
                            if(cbuf[i]=='u'&&cbuf[i+1]=='s'&&cbuf[i+2]=='e'&&cbuf[i+3]=='r')
                            {
                                break;
                            }
                            
                        }
                       i = i+5;
                       String s =  new String();
                       for(i=i;i<cbuf.length;i++)
                       {
                           if(cbuf[i]=='+')
                               s=s+" ";
                           else s=s+cbuf[i];
                       }
                       
                        
                        CreateHTMLPage(method,s);
                        byte[] fileData = readFileData(file, fileLength);

                        out.println("HTTP/1.1 200 OK");
                        out.println("Server: Java HTTP Server: 1.0");
                        out.println("Date: " + new Date());
                        out.println("Content-type: " + content);
                        out.println("Content-length: " + fileLength);
                        out.println(); 
                        out.flush(); 

                        br.write(fileData, 0, fileLength);
                        br.flush();
                        
                        String date=""+d;
                        logfile(ip, date, first, "200", Integer.toString(fileLength));
                    }
                }
                else 
                {
                    fileNotFound(out, br, fileRequested);
                    String date=""+d;
                    logfile(ip, date, "[Error] : "+first, "404 File Not Found", Integer.toString(0));
                }
                System.out.println("Request served: "+fileRequested + "Type : " + content);
            }
        } catch (IOException e) {
            System.err.println("IO error"+e);
        } finally {
            try {
                in.close();
                out.close();
                br.close();
                conn.close(); // we close socket connection
            } catch (Exception e) {
                System.err.println("Close Stream Error" + e.getMessage());
            }

                System.out.println("Connection closed.\n");
        }

    }

    private byte[] readFileData(File file, int fileLength) throws IOException {
        
        byte[] fData = new byte[fileLength];
        FileInputStream in = null;
        
        try {
            in = new FileInputStream(file);
            in.read(fData);
        } finally {
            if (in != null) {
                in.close();
            }
        }

        return fData;
    }

    // return supported MIME Types
    private String getContentType(String fileRequested) {
        if (fileRequested.endsWith(".htm") || fileRequested.endsWith(".html")) {
            return "text/html";
        }
        else if(fileRequested.endsWith("jpg")){
            return "image/jpg";
        }
        else if(fileRequested.endsWith("png")){
            return "image/png";
        }
        else if(fileRequested.endsWith("tiff")){
            return "image/tiff";
        }
        else if(fileRequested.endsWith("bmp")){
            return "image/bmp";
        }
        else if(fileRequested.endsWith("pdf")){
            return "application/pdf";
        }else {
            return "text/plain";
        }
    }

    private void fileNotFound(PrintWriter pr, OutputStream out, String fileRequested) throws IOException {
        File file = new File(ROOT_FILE, "404.html");
        int fileLength = (int) file.length();
        byte[] fileData = readFileData(file, fileLength);
        String content = "text/html";
        
        pr.println("HTTP/1.1 404 File Not Found");
        pr.println("Server: Java HTTP Server from SSaurel : 1.0");
        pr.println("Date: " + new Date());
        pr.println("Content-type: " + content);
        pr.println("Content-length: " + fileLength);
        pr.println(); // blank line between headers and content, very important !
        pr.flush(); // flush character output stream buffer

        out.write(fileData, 0, fileLength);
        out.flush();
        
       
        System.out.println(fileRequested + " not found");
     
    }
    
    private void logfile(String ip, String time, String reqtype, String status, String fsize){
        String fileName = "Logfile.txt"; 
        
  
        try { 
  
            // Open given file in append mode. 
            BufferedWriter out = new BufferedWriter( 
                   new FileWriter(fileName, true)); 
            String str = "\n"+ip + " " + "[" + time + "] \"" +reqtype+"\" "+status+" "+fsize;
            out.write("\n");
            out.write(str); 
            out.close(); 
        } 
        catch (IOException e) { 
            System.out.println("exception occoured" + e); 
        }
  
    }
    
    private void CreateHTMLPage(String req,String str){
        
            File file = new File("form_submited.html");
            
            FileWriter fileWriter = null;
            BufferedWriter bufferedWriter = null;
            try {
            fileWriter = new FileWriter(file);
            bufferedWriter = new BufferedWriter(fileWriter);

            String htmlPage = "<html>\n" +
                            "	<head>\n" +
                            "		<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n" +
                            "	</head>\n" +
                            "	<body>\n" +
                            "		<h1> Welcome to CSE 322 Offline 1</h1>\n" +
                            "		<h2> HTTP REQUEST TYPE-> " +req+ "</h2>\n"+
                            "		<h2> Post-> " + str+"</h2>\n"+
                            "		<form name=\"input\" action=\"http://localhost:6789/form_submited.html\" method=\"post\">\n" +
                            "		Your Name: <input type=\"text\" name=\"user\">\n" +
                            "		<input type=\"submit\" value=\"Submit\">\n" +
                            "		</form>\n" +
                            "	</body>\n" +
                            "</html>" ;

            bufferedWriter.write(htmlPage);

            
            System.out.println("Html page created");
            bufferedWriter.flush();
            fileWriter.flush();

            } catch (IOException e) {
                e.printStackTrace();
            }finally{
            try {

            bufferedWriter.close();
                fileWriter.close();
            } catch (IOException e) {
            
            e.printStackTrace();
            }


        }
    }

}
