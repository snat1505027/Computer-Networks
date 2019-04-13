/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtp;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author tisha
 */
public class ClosedState implements State{
    Server myServer;
    Socket smtpSocket;
    InetAddress mailHost;
    InetAddress localHost;
    BufferedReader in;
    PrintWriter pr;
    
    public ClosedState(Server s){
        myServer = s;
        this.pr = myServer.pr;
        this.in = myServer.in;
        this.localHost = myServer.localHost;
        this.mailHost = myServer.mailHost;
        this.smtpSocket = myServer.smtpSocket;
    }

    @Override
    public void connect() {
        try {
            
            String initialID = in.readLine();    
            System.out.println(initialID);
            //smtpSocket.connect(new InetSocketAddress(mailHost, myServer.getPORT()),1);
            //smtpSocket.setSoTimeout(1);
            myServer.changeState(myServer.getBeginState());
        
        } catch (SocketTimeoutException exception) {
            // Output expected SocketTimeoutExceptions.
            Logger.getLogger(ClosedState.class.getName()).log(Level.SEVERE, null, exception);
        } catch (IOException ex) {
            Logger.getLogger(ClosedState.class.getName()).log(Level.SEVERE, null, ex);
        }
        
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
        throw new UnsupportedOperationException("Not supported yet."); //To change body of generated methods, choose Tools | Templates.
    }
    
}
