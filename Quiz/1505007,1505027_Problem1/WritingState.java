/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtp;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Base64;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author tisha
 */
public class WritingState implements State {

    Server myServer;
    Socket smtpSocket;
    InetAddress mailHost;
    InetAddress localHost;
    BufferedReader in;
    PrintWriter pr;

    public WritingState(Server s) {
        myServer = s;
        this.pr = myServer.pr;
        this.in = myServer.in;
        this.localHost = myServer.localHost;
        this.mailHost = myServer.mailHost;
        this.smtpSocket = myServer.smtpSocket;
    }

    @Override
    public void connect() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void helo() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void mailFrom() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void rcptTo() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void data() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void writeMail() {
        BufferedReader br = null;
        try {
            File file = new File("C:\\Users\\Tisha\\Documents\\Computer_Networking_Sessional\\Container\\SMTP2.0\\header.txt");
            br = new BufferedReader(new FileReader(file));
            String temp;
            Scanner sc = new Scanner(System.in);
            while ((temp = br.readLine()) != null) {
                pr.println(temp);
                pr.flush();
            }

            pr.println("MIME-Version: 1.0");
            pr.flush();

            pr.println("Content-Type: multipart/mixed; boundary=sep");
            pr.flush();

            pr.println();
            pr.flush();

            file = new File("C:\\Users\\Tisha\\Documents\\Computer_Networking_Sessional\\Container\\SMTP2.0\\mail.txt");
            br = new BufferedReader(new FileReader(file));
            String st;

            pr.println("--sep");
            pr.flush();

            pr.println("Content-Type: text/plain; charset=us-ascii");
            pr.flush();

            pr.println();
            pr.flush();

            while ((st = br.readLine()) != null) {
                System.out.println(st);
                pr.println(st);
                pr.flush();
            }

            pr.println();
            pr.flush();

            System.out.println("Want to attach anything?");
            sc = new Scanner(System.in);
            String str = sc.nextLine();

            if (str.equalsIgnoreCase("y")) {
                file = new File("C:\\Users\\Tisha\\Documents\\Computer_Networking_Sessional\\Container\\SMTP2.0\\attachments.txt");
                br = new BufferedReader(new FileReader(file));

                
                String filename;
                while ((filename = br.readLine()) != null) {
                    System.out.println(filename);
                   
                    pr.println("--sep");
                    pr.flush();
                    String t = "Content-Type: application/octet-stream; name=\"" + filename + "\"";
                    pr.println(t);
                    pr.flush();

                    t = "Content-Disposition: attachment; filename=\"" + filename + "\"";
                    pr.println(t);
                    pr.flush();

                    pr.println("Content-Transfer-Encoding: base64");
                    pr.flush();

                    pr.println();
                    pr.flush();
                    t = "C:\\Users\\Tisha\\Documents\\Computer_Networking_Sessional\\Container\\SMTP2.0\\" + filename;
                    File f = new File(t);
                    String encodstring = encodeFileToBase64Binary(f);

                    pr.println(encodstring);
                    pr.flush();

                }
            }

                pr.println("--sep--");
                pr.flush();

                myServer.changeState(myServer.getDeliverState());

            }catch (FileNotFoundException ex) {
            Logger.getLogger(WritingState.class.getName()).log(Level.SEVERE, null, ex);
        }catch (IOException ex) {
            Logger.getLogger(WritingState.class.getName()).log(Level.SEVERE, null, ex);
        }finally {
            try {
                br.close();
            } catch (IOException ex) {
                Logger.getLogger(WritingState.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
        }

        @Override
        public void deliver
        
            () {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
        }

    

    private static String encodeFileToBase64Binary(File file) {
        String encodedfile = null;
        try {
            FileInputStream fileInputStreamReader = new FileInputStream(file);
            byte[] bytes = new byte[(int) file.length()];
            fileInputStreamReader.read(bytes);
            encodedfile = Base64.getEncoder().encodeToString(bytes);
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        return encodedfile;
    }
}
