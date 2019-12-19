using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace abc
{
    class Program
    {
        static void Main(string[] args)
        {
            SocketTest.Send("106.15.196.133", 8080, "I'm client!");
        }
    }
}
