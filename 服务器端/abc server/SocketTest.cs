using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
namespace abc
{
    public class SocketTest
    {
        //1是处理节点板提交的数据
        //2是处理客户端的命令/数据
        int port1 = 8080;
        int port2 = 9090;
         Socket acceptSocket1;//连接套接字1
         Socket acceptSocket2;//连接套接字2
         Thread ln1;//监听线程1
         Thread ln2;//监听线程2
         Thread th1;//通信线程1
         Thread th2;//通信线程2
         Thread hd1;//处理线程1
         Thread hd2;//处理线程2
        //节点板提交的数据
       public  class NodeBuf
        {
           public static bool StopFlag; //停止标记
           public static  byte[] nodebuf{ get; set; }//缓冲数组
        }
        //客户端提交的数据
        public class ClientBuf
        {
            public static bool StopFlag;
            public static byte[] clientbuf { get; set; }
        }

        byte [] node = new byte[1024];
        byte[] client = new byte[1024];//缓冲池

        private static Encoding encode = Encoding.Default;
        /// <summary>
        /// 监听请求
        /// </summary>
        /// <param name="port1"></param>
        public void Start()
        {
            ln1 = new Thread(Listen1);//开启两个线程，监听端口
            ln1.Start();
            ln2 = new Thread(Listen2);
            ln2.Start();
            //while (true)
            //{
            //      acceptSocket1 = listenSocket1.Accept();               //接收挂起的请求，返回一个连接套接字
            //      th = new Thread(handle);
            //      th.Start();
           //     acceptSocket.IOControl(IOControlCode.KeepAliveValues, KeepAlive(1, 30000,10000), null);//设置Keep-Alive参数,单位ms
            //    string receiveData = Receive(acceptSocket, 1000); //1 seconds timeout.
           //     Console.WriteLine("Receive：" + receiveData);
          //      acceptSocket.Send(encode.GetBytes("ok"));
          //  }
        }
        //socket 监听线程1
        public void Listen1()
        {
            Socket listenSocket1 = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            listenSocket1.Bind(new IPEndPoint(IPAddress.Any, port1));//监听任意ip地址的port1端口
            listenSocket1.Listen(100);                             //最多允许100个挂起请求
            Console.WriteLine("Listen " + port1 +" ...");
            acceptSocket1 = listenSocket1.Accept();               //接收挂起的请求，返回一个连接套接字
            //连接成功
            th1 = new Thread(handle1);
            th1.Start();
        }

        //socket 监听线程2
        public void Listen2()
        {
            Socket listenSocket2 = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            listenSocket2.Bind(new IPEndPoint(IPAddress.Any, port2));//监听任意ip地址的port1端口
            listenSocket2.Listen(100);                             //最多允许100个挂起请求
            Console.WriteLine("Listen " + port2 + " ...");
            acceptSocket2 = listenSocket2.Accept();               //接收挂起的请求，返回一个连接套接字
            //连接成功
            th2 = new Thread(handle2);
            th2.Start();
        }

        //socket通信线程
        //处理和节点之间通信
        public void handle1()
        {
            while (true)
            {
                //先从套接字中接收一下数据
                string receiveData = Receive(acceptSocket1, 1000);
                if (receiveData == "")//没有数据
                {
                    ;
                }
                else
                {
                     NodeBuf.nodebuf=encode.GetBytes(receiveData);
                    Console.WriteLine("From Node：" + encode.GetString(NodeBuf.nodebuf));
                   
                    acceptSocket2.Send(NodeBuf.nodebuf);   //发给2了
                }
            }
/*
            while (true)
            {
                string s = "";
                if ((s = Console.ReadLine()) == "")
                {
                    //无发送数据请求,处理接收
                    string receiveData = Receive(acceptSocket1, 1000);
                    Console.WriteLine("From Node：" + receiveData);
                }
                else
                {
                    //有发送数据请求
                    acceptSocket1.Send(encode.GetBytes(s));
                }
            }
            DestroySocket(acceptSocket1); //import
 * */
        }

        public void handle2()
        {
            while (true)
            {
                //先从套接字中接收一下数据
                string receiveData = Receive(acceptSocket2, 1000);
                if (receiveData == "")//没有数据
                {
                    ;
                }
                else
                {
                    ClientBuf.clientbuf = encode.GetBytes(receiveData);
                    Console.WriteLine("From CLient：" + encode.GetString(ClientBuf.clientbuf));
                 
                    acceptSocket1.Send(ClientBuf.clientbuf);  //发给1了
                  //  StringBuilder sb=new StringBuilder();
                 //   sb.AppendLine(receiveData);
                  //  sb.
                  //  ClientBuf.clientbuf
                   //Console.WriteLine("From Client：" + receiveData);
                }
            }
        }

        /// <summary>
        /// 套接字维持时间
        /// onOff:是否开启KeepAlive
        ///keepAliveTime:开始首次Ke epAlive探测前的TCP空闭时间
        ///keepAliveInterval: 两次KeepAlive探测间的时间间隔
        /// </summary>
        /// <param name=""></param>
        public byte[] KeepAlive(int onOff, int keepAliveTime, int keepAliveInterval)
        {
            byte[] buffer = new byte[12];
            BitConverter.GetBytes(onOff).CopyTo(buffer, 0);
            BitConverter.GetBytes(keepAliveTime).CopyTo(buffer, 4);
            BitConverter.GetBytes(keepAliveInterval).CopyTo(buffer, 8);
            return buffer;
        }
        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="host"></param>
        /// <param name="port"></param>
        /// <param name="data"></param>
        /// <returns></returns>
        public string Send(string host, int port, string data)
        {
            string result = string.Empty;
            Socket clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            clientSocket.Connect(host, port);
            clientSocket.Send(encode.GetBytes(data));
            Console.WriteLine("Send：" + data);
     //       result = Receive(clientSocket, 5000 * 2); //5*2 seconds timeout.返回指定套接字接收到的数据
            Console.WriteLine("Receive：" + result);
            DestroySocket(clientSocket);
            return result;
        }
        /// <summary>
        /// 接收数据,未接收到，则返回empty
        /// </summary>
        /// <param name="socket"></param>
        /// <param name="timeout"></param>
        /// <returns></returns>
        private string Receive(Socket socket, int timeout)
        {
            string result = string.Empty;
            socket.ReceiveTimeout = timeout;
            List<byte> data = new List<byte>();
            byte[] buffer = new byte[1024];
            int length = 0;
            try
            {
                while ((length = socket.Receive(buffer)) > 0)//当套接字数据非空
                {
                    for (int j = 0; j < length; j++)
                    {
                        data.Add(buffer[j]);
                    }
                    if (length < buffer.Length)
                    {
                        break;
                    }
                }
            }
            catch { }
            if (data.Count > 0)
            {
                result = encode.GetString(data.ToArray(), 0, data.Count);//接收到则返回字符串
            }
            return result;
        }
        /// <summary>
        /// 销毁Socket对象
        /// </summary>
        /// <param name="socket"></param>
        private static void DestroySocket(Socket socket)
        {
            if (socket.Connected)
            {
                socket.Shutdown(SocketShutdown.Both);
            }
            socket.Close();
        }
    }
}