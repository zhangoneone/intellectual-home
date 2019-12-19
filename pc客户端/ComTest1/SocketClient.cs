using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
namespace SerialportSample
{
    public static class SocketClient
    {
        private static Encoding encode = Encoding.Default;
        /// <summary>
        /// 监听请求
        /// </summary>
        /// <param name="port"></param>
        public static void Listen(int port)
        {
            Socket listenSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            listenSocket.Bind(new IPEndPoint(IPAddress.Any, port));//监听任意ip地址的port端口
            listenSocket.Listen(100);                             //最多允许100个挂起请求
            Console.WriteLine("Listen " + port + " ...");
            while (true)
            {
                Socket acceptSocket = listenSocket.Accept();               //接收挂起的请求，返回一个连接套接字
                string receiveData = Receive(acceptSocket, 5000); //5 seconds timeout.
                Console.WriteLine("Receive：" + receiveData);
                acceptSocket.Send(encode.GetBytes("ok"));
                DestroySocket(acceptSocket); //import
            }
        }
        /// <summary>
        /// 发送数据
        /// </summary>
        /// <param name="host"></param>
        /// <param name="port"></param>
        /// <param name="data"></param>
        /// <returns></returns>
        public static string Send(string host, int port, string data)
        {
            string result = string.Empty;
            Socket clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            clientSocket.Connect(host, port);
            //  clientSocket.Send(encode.GetBytes(data));//发起了tcp连接
            //    Console.WriteLine("Send：" + data);
            while (true)
            {
                string s = "I'm client!";
                string receiveData = Receive(clientSocket, 1000);
                Console.WriteLine("Receive：" + receiveData);
                clientSocket.Send(encode.GetBytes(s));
                //if ((s = Console.ReadLine()) == "")
                //{
                //    //没有发送数据请求,处理接收
                //    string receiveData = Receive(clientSocket, 1000);
                //    Console.WriteLine("Receive：" + receiveData);
                //}
                //else
                //{
                //    //有发送数据请求，发送数据
                //    clientSocket.Send(encode.GetBytes(s));
                //    Console.WriteLine("Send：" + s);
                //}
                //     DestroySocket(clientSocket);
            }
            return result;
        }
        /// <summary>
        /// 接收数据
        /// </summary>
        /// <param name="socket"></param>
        /// <param name="timeout"></param>
        /// <returns></returns>
        private static string Receive(Socket socket, int timeout)
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