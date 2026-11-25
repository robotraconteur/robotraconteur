function RobotRaconteurMatlabRateTest()
    % CreateRate must return the new rate id and must not modify its input
    freq = 100;
    freq_copy = freq;
    r = RobotRaconteur.CreateRate(freq);
    assert(freq == 100)
    assert(freq_copy == 100)
    for i=1:3
        r.Sleep();
    end
    r2 = RobotRaconteur.CreateRate(freq);
    r2.Sleep();
    clear r r2
end
