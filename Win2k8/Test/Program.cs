using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Test
{
    class Program
    {
        static void Main(string[] args)
        {
            string fileName = "domain2.txt";

            string sourcePath = @"C:\vhosts\ft8eisas\vhost\httpdoc";
            string targetPath = @"C:\vhosts\6pd0v3zg\vhost\httpdoc\subdir";

            System.IO.Directory.CreateDirectory(targetPath);

            string sourceFile = System.IO.Path.Combine(sourcePath, fileName);
            string destFile = System.IO.Path.Combine(targetPath, fileName);

            System.IO.File.Copy(sourceFile, destFile, true);

        }
    }
}
