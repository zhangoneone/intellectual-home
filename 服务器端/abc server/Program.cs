using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Data.Odbc;
using System.Data.OleDb;
using System.Net;
using System.Net.Sockets;
using System.Threading;

/*服务器端运行的程序，负责接收传递来的数据，
 * 处理之后保存到数据库，
 * 并侦听来自客户端的数据请求。
 * 有请求，则从数据库取数据给客户端*/
namespace abc
{
    class Program
    {
    //    static string s="36.63.45.58";
     //   static string s1 = "192.168.2.100";
     //   static string[] a = { s1, "test" }; 
        static void Main(string[] args)
        {
    //        Listener listener = new Listener();//在指定的ip和端口侦听
     //       new Sender().Send(a);
     //       Console.ReadLine();
      //      SocketTest.Send("36.63.45.58", 8080, "hello");
            new SocketTest().Start();
            Console.ReadLine();
        }
    }
}
