using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace abc
{
    class Listener
    {
        private Thread th;
        private Thread recth;
        private TcpListener tcpl;
        private IPAddress localip;
        private int port=8080;
        public bool listenerRun = true;
        Socket recsocket;
      //listenerRun为true，表示可以接受连接请求，false则为结束程序
      //string s = "172.19.156.44";
     //   string s = "127.0.0.1";
        string s = "192.168.2.100";
        public Listener()//构造函数
        {
            Byte[] IPByte = IPAddress.Parse(s).GetAddressBytes();//IP地址换成字节数组
            localip = new IPAddress(IPByte);
            th = new Thread(new ThreadStart(Listen));//新建一个用于监听的线程
            th.Start();//打开新线程
        }

        public void Stop()
        {
            tcpl.Stop();
            th.Abort();//终止线程
            recth.Abort();
        }

        private void Listen()
        {
             try
             {
                 tcpl = new TcpListener(localip,port);//指定ip地址和端口处新建一个TcpListener对象
                 tcpl.Start();
                 Console.WriteLine("ip<{0}>port<{1}>started listening..",localip,port);

                 while(listenerRun)//开始监听
                {
                    recsocket = tcpl.AcceptSocket();//侦听到一个tcp连接请求，返回一个socket给recsocket
                    recth =new Thread( new ThreadStart(Get_data));
                    recth.Start();
                }
             }
            catch(System.Security.SecurityException)
            {
                Console.WriteLine("防火墙不授权给应用，应用哭了...");
            }
            catch(Exception e)
            {
                Console.WriteLine(e.ToString()+"\r\n");
                Stop();
            }
        }

        private void Get_data()//获取数据
        {
            while (true)
            {
                try
                {
                    string remote = recsocket.RemoteEndPoint.ToString();
                    Byte[] stream = new Byte[80];
                    int i = recsocket.Receive(stream);//接受连接请求的字节流
                    string msg = "<" + remote + ">" + System.Text.Encoding.UTF8.GetString(stream, 0, i);
                    Console.WriteLine(msg);//在控制台显示字符串
                }
                catch (Exception)
                { Console.WriteLine("获取数据部分产生异常"); Stop(); }
            }
        }
    }
}
