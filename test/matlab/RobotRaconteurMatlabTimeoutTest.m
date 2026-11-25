function RobotRaconteurMatlabTimeoutTest(url)
    % Timeouts are given in seconds and may be fractional
    sub = RobotRaconteur.SubscribeService(url);
    c = sub.GetDefaultClientWait(5.5);
    c.b1 = true;
    assert(c.b1 == true)
    clear c
    sub.Close();
    clear sub
end
