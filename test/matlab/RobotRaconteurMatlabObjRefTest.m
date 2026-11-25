function RobotRaconteurMatlabObjRefTest(url)
    % Repeated access to the same objref returns working objects
    c=RobotRaconteur.ConnectService(url);
    o1 = c.get_o4();
    o2 = c.get_o4();
    for i=1:3
        o3 = c.get_o4();
        assert(o3.i_ind == 0)
    end
    assert(o1.i_ind == 0)
    assert(o2.i_ind == 0)
    clear o1 o3
    assert(o2.i_ind == 0)
    RobotRaconteur.DisconnectService(c)
end
