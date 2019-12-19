using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;
using System.IO;

namespace abc
{
    public class Sender
    {
     //   string ip="";
        int port=9090;
        IPAddress ipaddr;
       public void Send(string[] aInput)
        {
            Byte[] IPByte = IPAddress.Parse(aInput[0]).GetAddressBytes();//IP地址换成字节数组
            ipaddr = new IPAddress(IPByte);
            IPEndPoint ipt = new IPEndPoint(ipaddr,port);

        string stream = "";
        //获得要发送的信息
        for(int i=2; i<aInput.Length; i++)
        {
        stream += aInput[i] ;
        }
        TcpClient tcpc = new TcpClient(ipt); //新建一个TcpClient对象
        try
        {
            tcpc.Connect(ipt);
            if (tcpc.Connected)
            {
                NetworkStream tcpStream = tcpc.GetStream();//获取一个tcp的流
                StreamWriter reqStreamW = new StreamWriter(tcpStream);
                reqStreamW.Write(stream);
                reqStreamW.Flush();//发送信息
                tcpStream.Close();
                tcpc.Close();
            }
            else { Console.WriteLine("tcp客户端未连接主机！\r\n"); }
        }
        catch(Exception e)
        {
        Console.WriteLine(e.ToString()+"\r\n");
        tcpc.Close();
        }
       }
    }
}
