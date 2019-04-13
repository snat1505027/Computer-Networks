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
import java.net.UnknownHostException;

/**
 *
 * @author tisha
 */
public class Server {
    private String mailServer = "smtp.sendgrid.net";
    private int PORT = 25; 
    State closedState;
    State beginState;
    State waitState;
    State envState;
    State recptState;
    State writeState;
    State deliverState;
    State writingState;
    
    State serverState;
    
    
    Socket smtpSocket;
    InetAddress mailHost;
    InetAddress localHost;
    BufferedReader in;
    PrintWriter pr;
    
    public Server() throws UnknownHostException, IOException{
        //mailHost = InetAddress.getByName(mailServer);
        mailHost = InetAddress.getLocalHost();
        localHost = InetAddress.getLocalHost();
        smtpSocket = new Socket();    
        smtpSocket.connect(new InetSocketAddress(mailHost, PORT),20000);
        smtpSocket.setSoTimeout(20000);
        in = new BufferedReader(new InputStreamReader(smtpSocket.getInputStream()));
        pr = new PrintWriter(smtpSocket.getOutputStream(), true);
        
        closedState = new ClosedState(this);
        beginState = new BeginState(this);
        waitState = new WaitState(this);
        envState = new EnvCreateState(this);
        recptState = new RcptState(this);
        writingState = new WritingState(this);
        deliverState = new DeliverState(this);
        serverState = closedState;
    }
    
    String getMailServer(){
        return mailServer;
    }
    
    int getPORT(){
        return PORT;
    }
    
    void setMailHost(InetAddress inetAddress){
        this.mailHost = inetAddress;
    }
    
    void setLocalHost(InetAddress inetAddress){
        this.localHost = inetAddress;
    }
    
    void setSocket(Socket socket){
        this.smtpSocket = socket;
    }
    
    void setBufferReader(BufferedReader bufferedReader){
        this.in = bufferedReader;
    }
    
    void setPrintWriter(PrintWriter printWriter){
        this.pr = printWriter;
    }
    
    void changeState(State newState){
        this.serverState = newState;
    }
    
    void connect(){
        serverState.connect();
    }
    
    void helo(){
        serverState.helo();
    }
    
    void mailFrom(){
        serverState.mailFrom();
    }
    
    void rcptTo(){
        serverState.rcptTo();
    }
    
    void data(){
        serverState.data();
    }
    
    void writeMail(){
        serverState.writeMail();
    }
    
    void deliver(){
        serverState.deliver();
    }
    
    State getClosedState(){ return closedState; }
    State getBeginState(){ return beginState; }
    State getWaitState(){ return waitState; }
    State getenvState(){ return envState; }
    State getrecptState(){ return recptState; }
    State getwritingState(){ return writingState; }
    State getDeliverState(){ return deliverState; }
}
