function RobotRaconteurMatlabSubscriptionClientTest(url)
    % Getting the default client of a subscription more than once returns the
    % stub that already has a MATLAB object. Each call must return a copy that
    % MATLAB can own and free.
    sub = RobotRaconteur.SubscribeService(url);
    c1 = sub.GetDefaultClientWait(10);
    c1.b1 = true;
    for i=1:5
        c2 = sub.GetDefaultClientWait(10);
        assert(c2.b1 == true)
        c3 = sub.GetDefaultClient();
        assert(c3.b1 == true)
        clients = sub.GetConnectedClients();
        assert(clients.Count == 1)
        clear c2 c3 clients
    end
    clear c1
    sub.Close();
    clear sub
end
