classdef RobotRaconteur
         
    methods
        function a=RobotRaconteur(c)
           error('Valid commands are Connect, Disconnect, EnableEvents, DisableEvents, RequestObjectLock, ReleaseObjectLock, MonitorEnter,MonitorExit, ProcessRequests, FindService, Shutdown'); 
        end
    end
    
		
    methods(Static=true)
        function ret=Connect(url,username,credentials)
			if(nargin ==1)
				ret=RobotRaconteurMex('Connect',url); 
			else
				ret=RobotRaconteurMex('Connect',url,username,credentials); 
		    end
        end
		
		function ret=ConnectService(url,username,credentials)
			if nargin ==1
				ret=RobotRaconteurMex('Connect',url); 
			else
				ret=RobotRaconteurMex('Connect',url,username,credentials); 
		    end
        end
        
        function Disconnect(objref)
           disconnect(objref);
        end
        
		function DisconnectService(objref)
           disconnect(objref);
        end
		
        function EnableEvents(objref)
           enableevents(objref);
        end
        
        function DisableEvents(objref)
           disableevents(objref);
        end
        
        function s=FindService(name)
           s=RobotRaconteurMex('FindService',name); 
        end
        
		function s=FindServiceByType(name)
           s=RobotRaconteurMex('FindService',name); 
        end
		
		function s=FindNodeByID(name)
           s=RobotRaconteurMex('FindNodeByID',name); 
        end
		
		function s=FindNodeByName(name)
           s=RobotRaconteurMex('FindNodeByName',name); 
        end
		
		function UpdateDetectedNodes()
			RobotRaconteurMex('UpdateDetectedNodes');
		end
		
		function s=GetDetectedNodes()
			s=RobotRaconteurMex('GetDetectedNodes');
		end
		
        function ProcessRequests()
           
           RobotRaconteurMex('ProcessRequests');
        end
        
        function RequestObjectLock(objref,type)
            if (nargin==1)
                type='User';
            end
            if (strcmp(type,'User'))
                lockop(objref,'RequestUserLock');
            elseif (strcmp(type,'Client'))
                lockop(objref,'RequestClientLock');
            else
                error('Unknown command')
            end
        end
        
        function ReleaseObjectLock(objref)
            lockop(objref,'ReleaseUserLock');
        end
        
        function MonitorEnter(objref)
           lockop(objref,'MonitorEnter'); 
        end
        
        function MonitorExit(objref)
            lockop(objref,'MonitorExit'); 
        end
        
        function types=GetPulledServiceTypes(obj)
           types=getpulledservicetypes(obj);
        end
        
        function def=GetPulledServiceType(obj,name)
           def=getpulledservicetype(obj,name);
        end
        
		function t=nowUTC()
			ts=RobotRaconteurMex('NowUTC');
			t=datenum(datevec(ts,'yyyymmddTHHMMSS.FFF'));
		end
		
		function t=clockUTC()
			ts=RobotRaconteurMex('NowUTC');
			t=datevec(ts,'yyyymmddTHHMMSS.FFF');
		end

		function t=NowNodeTime()
			ts=RobotRaconteurMex('NowNodeTime');
			t=datenum(datevec(ts,'yyyymmddTHHMMSS.FFF'));
		end
		
		function t=NodeSyncTimeUTC()
			ts=RobotRaconteurMex('NodeSyncTimeUTC');
			t=datenum(datevec(ts,'yyyymmddTHHMMSS.FFF'));
		end

		function Sleep(duration)
			RobotRaconteurMex('Sleep',double(duration));
		end
		
		function r=CreateRate(frequency)
			r=RobotRaconteurRate(frequency);
		end
		
		function ret=SubscribeServiceByType(service_types,filter)
            if(nargin ==1)
				ret=RobotRaconteurMex('SubscribeServiceByType',service_types); 
            else
				ret=RobotRaconteurMex('SubscribeServiceByType',service_types,filter); 
            end
        end
		
        function ret=SubscribeServiceInfo2(service_types,filter)
            if(nargin ==1)
				ret=RobotRaconteurMex('SubscribeServiceInfo2',service_types); 
            else
				ret=RobotRaconteurMex('SubscribeServiceInfo2',service_types,filter); 
            end
		end
		
		function ret=SubscribeService(url,username,credentials)
			if nargin ==1
				ret=RobotRaconteurMex('SubscribeService',url); 
			else
				ret=RobotRaconteurMex('SubscribeService',url,username,credentials); 
		    end
        end
        
		%Server Functions
		
		function n=GetServiceType(servicename)
			n=RobotRaconteurMex('GetServiceType',servicename);
		end
		
		function n=GetRegisteredServiceTypes()
			n=RobotRaconteurMex('GetRegisteredServiceTypes');
		end
		
		function RegisterServiceType(typestring)
			RobotRaconteurMex('RegisterServiceType',typestring);
		end
		
		function StartLocalClient(nodeid)
			RobotRaconteurMex('StartLocalClient',nodeid);			
        end
		
		function StartLocalServer(nodeid)
			RobotRaconteurMex('StartLocalServer',nodeid);			
        end
        
        function StartTcpServer(port)
           RobotRaconteurMex('StartTcpServer',int32(port)); 
        end
		
		function StartTcpServerUsingPortSharer()
			RobotRaconteurMex('StartTcpServerUsingPortSharer');
		end
		
		function port=GetTcpListenPort()
			port=RobotRaconteurMex('GetTcpListenPort');
		end
		
		function ret=IsTcpPortSharerRunning()
			ret=RobotRaconteurMex('IsTcpPortSharerRunning');
		end
		
		function LoadTlsNodeCertificate()
			RobotRaconteurMex('LoadTlsNodeCertificate');
		end
		
		function ret=IsTlsNodeCertificateLoaded()
			ret=RobotRaconteurMex('IsTlsNodeCertificateLoaded');
		end
						
		function RegisterService(name, type, obj, security)
			if (nargin==3)
				RobotRaconteurMex('RegisterService', name, type, obj);
			else
				RobotRaconteurMex('RegisterService', name, type, obj, security);
			end
		end
		
		function CloseService(name)
			RobotRaconteurMex('CloseService', name);
		
		end
		
		function ProcessServerRequests(timeout)
			if (nargin==0)
				RobotRaconteurMex('ProcessServerRequests',10);
			else
				if (timeout>10)
					timeout=10;
				end
				RobotRaconteurMex('ProcessServerRequests',timeout);
			end
		
		end
		
        function Shutdown()
           RobotRaconteurMex('Shutdown');
           clear RobotRaconteurMex
        end
		
		function timeout=getTransactionTimeout()
			timeout=RobotRaconteurMex('GetTransactionTimeout');
		end
		
		function setTransactionTimeout(timeout)
			RobotRaconteurMex('SetTransactionTimeout',timeout);
		end
		
		function timeout=getTransportTimeout()
			timeout=RobotRaconteurMex('GetTransportTimeout');
		end
		
		function setTransportTimeout(timeout)
			RobotRaconteurMex('SetTransportTimeout',timeout);
		end

		function level=GetLogLevel()
			level=RobotRaconteurMex('GetLogLevel');
		end

		function SetLogLevel(level)
			RobotRaconteurMex('SetLogLevel',level);
		end
		
		function OpenLogFile(filename)
			RobotRaconteurMex('OpenLogFile',filename);
		end

		function CloseLogFile()
			RobotRaconteurMex('CloseLogFile');
		end
	end
    
end

