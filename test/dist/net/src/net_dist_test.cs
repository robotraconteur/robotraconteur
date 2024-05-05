using RobotRaconteur;
internal class Program
{
    class DistfilesTest_impl : experimental.distfiles_test.DistfilesTest_default_impl
    {
        public override double c { get; set; }
        public override double add(double a, int b)
        {
            return a + b + c;
        }
    }

    private static void Main(string[] args)
    {
        Console.WriteLine(RobotRaconteurNode.s.RobotRaconteurVersion);

        using (var node_setup = new ServerNodeSetup("experimental.distfiles_test", 0, args))
        {
            var o = new DistfilesTest_impl();
            RobotRaconteurNode.s.RegisterService("DistfilesTest", "experimental.distfiles_test", o);
            var port = node_setup.TcpTransport.GetListenPort();

            var c = (experimental.distfiles_test.DistfilesTest)RobotRaconteurNode.s.ConnectService(
                $"rr+tcp://localhost:{port}/?service=DistfilesTest");
            c.c = 5;
            if (c.add(1, 2) != 8)
                throw new Exception("Invalid result");
            Console.WriteLine("Test passed");
        }
    }
}
