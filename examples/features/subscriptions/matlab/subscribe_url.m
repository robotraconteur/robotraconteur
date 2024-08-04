% subscribe_url.m - Connect to a service using a URL

url = 'rr+tcp://localhost:29200?service=reynard';
sub = RobotRaconteur.SubscribeService(url);
c = sub.GetDefaultClientWait(10);
c.say('Hello from URL subscription!');
sub.Close();
