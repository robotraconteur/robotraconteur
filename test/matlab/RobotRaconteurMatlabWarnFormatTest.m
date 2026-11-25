function RobotRaconteurMatlabWarnFormatTest(url)
    % An error raised in an async handler is reported as a warning. The error message
    % must be printed as text, not used as a printf format (it contains % conversions)
    c=RobotRaconteur.ConnectService(url);
    g=c.gen_func1();
    g.async_Next(@err_handler, 15);
    pause(1)
    RobotRaconteur.ProcessRequests()
    g.Close()
    RobotRaconteur.DisconnectService(c)

    function err_handler(key, res, err)
        error('RobotRaconteurTest:format', '%s', 'handler error %s %s %s %s %s %s %s %s %n')
    end
end
