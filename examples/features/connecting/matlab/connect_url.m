% connect_url.m - Connect to a service using a URL

url = 'rr+tcp://localhost:29200?service=reynard';
c = RobotRaconteur.ConnectService(url);
c.say('Hello from connect_url!');
RobotRaconteur.DisconnectService(c);
