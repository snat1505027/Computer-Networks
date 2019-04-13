/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtp;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.Socket;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author tisha
 */
public class EnvCreateState implements State {

    Server myServer;
    Socket smtpSocket;
    InetAddress mailHost;
    InetAddress localHost;
    BufferedReader in;
    PrintWriter pr;

    public EnvCreateState(Server s) {
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
        Scanner sc = new Scanner(System.in);
        while (true) {
            String str = sc.nextLine();

            if (str.equalsIgnoreCase("RCPT")) {

                BufferedReader br = null;
                try {
                    File file = new File("C:\\Users\\Tisha\\Documents\\Computer_Networking_Sessional\\Container\\SMTP2.0\\sender_receiver.txt");
                    br = new BufferedReader(new FileReader(file));
                    String st;
                    while ((st = br.readLine()) != null) {
                        if (st.equals("Receivers:")) {
                            while ((st = br.readLine()) != null) {
                                System.out.println(st);
                                //mf = br.readLine();
                                pr.println("RCPT TO:<" + st + ">");
                                pr.flush();
                                String welcome = in.readLine();
                                System.out.println(welcome);
                            }
                            break;
                        }
                        //System.out.println(st);
                    }

                    myServer.changeState(myServer.getrecptState());
                    break;
                } catch (FileNotFoundException ex) {
                    Logger.getLogger(EnvCreateState.class.getName()).log(Level.SEVERE, null, ex);
                } catch (IOException ex) {
                    Logger.getLogger(EnvCreateState.class.getName()).log(Level.SEVERE, null, ex);
                } finally {
                    try {
                        br.close();
                    } catch (IOException ex) {
                        Logger.getLogger(EnvCreateState.class.getName()).log(Level.SEVERE, null, ex);
                    }
                }
                break;
            }
            else{
                System.out.println("Invalid");
            }
        }
    }

    @Override
    public void data() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void writeMail() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void deliver() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
}
