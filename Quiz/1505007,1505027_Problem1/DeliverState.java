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
public class DeliverState implements State{
    Server myServer;
    Socket smtpSocket;
    InetAddress mailHost;
    InetAddress localHost;
    BufferedReader in;
    PrintWriter pr;

    public DeliverState(Server s){
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
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }

    @Override
    public void deliver() {
        try {
        
            pr.println(".");
            pr.flush();
            
            
            pr.println("QUIT");
            pr.flush();
            
            
            System.out.println("Here");
            String welcome = in.readLine();
            System.out.println(welcome);
            //myServer.changeState(myServer.getWaitState());
            smtpSocket.close();
            
        } catch (IOException ex) {
            Logger.getLogger(DeliverState.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
