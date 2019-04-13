/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtp;

import java.io.IOException;
import java.util.Scanner;

/**
 *
 * @author tisha
 */
public class Process {
    Server myServer;
    public  Process() throws IOException{
        myServer = new Server();
        myServer.connect();
    }
    
    void fullProcess(){
        Scanner sc = new Scanner(System.in);
        myServer.helo();
        //while(true){    
            myServer.mailFrom();
            myServer.rcptTo();
            myServer.data();
            myServer.writeMail();
            myServer.deliver();
            /*System.out.println("Want to send more?");
            String str = sc.nextLine();
            if(!str.equalsIgnoreCase("y")){
                break;
            }
        }*/
    }
}
