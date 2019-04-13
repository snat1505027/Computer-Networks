/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtp;

import java.io.BufferedReader;
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
public class BeginState implements State {

    Server myServer;
    Socket smtpSocket;
    InetAddress mailHost;
    InetAddress localHost;
    BufferedReader in;
    PrintWriter pr;

    public BeginState(Server s) {
        myServer = s;
        this.pr = myServer.pr;
        this.in = myServer.in;
        this.localHost = myServer.localHost;
        this.mailHost = myServer.mailHost;
        this.smtpSocket = myServer.smtpSocket;
    }

    @Override
    public void connect() {

    }

    @Override
    public void helo() {
        Scanner sc = new Scanner(System.in);

        while (true) {
            String str = sc.nextLine();
            if (str.equalsIgnoreCase("HELO") || str.equalsIgnoreCase("EHLO")) {
                try {
                    pr.println(str + " " + localHost);
                    pr.flush();
                    String welcome = in.readLine();
                    System.out.println(welcome);

                    /*pr.println("AUTH LOGIN");
                    pr.flush();
                    
                    pr.println("c3BvbmRvbg==");
                    pr.flush();
                    
                    pr.println("U3BvbmRvbjc3");
                    pr.flush();
                    */
                    myServer.changeState(myServer.getWaitState());
                    break;
                } catch (IOException ex) {
                    Logger.getLogger(BeginState.class.getName()).log(Level.SEVERE, null, ex);
                }
            }
            else{
                System.out.println("Invalid");
            }
        }
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
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void deliver() {
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

}
