function RobotRaconteurMatlabMemoryWriteDimsTest(url)
    % mem(:) = data on a multidim memory takes the count from every dimension of data
    c=RobotRaconteur.ConnectService(url);
    m2 = c.namedarray_m2;
    data = reshape(1:(7*10*20), [7,10,20]);
    m2(:) = data;
    data2 = m2(:);
    assert(isequal(size(data2), [7 10 20]))
    assert(isequal(data2, data))
    RobotRaconteur.DisconnectService(c)
end
