/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package smtp;

/**
 *
 * @author tisha
 */
public interface State {
    void connect();
    void helo();
    void mailFrom();
    void rcptTo();
    void data();
    void writeMail();
    void deliver();
}
