function irobot_create_client()

    %Example iRobot Create client in MATLAB

    %Connect to the service
    c=RobotRaconteur.ConnectService('rr+tcp://localhost:22354?service=create');


    %Drive a bit
    c.drive(0.2,1.0);
    pause(0.5);
    c.drive(0,1.0);

    %Demonstrate events and callbacks
    %Enable events and callbacks.  Only do this if you plan on using them,
    %the buffer can overflow if they are not processed...
    RobotRaconteur.EnableEvents(c);

    %addlistener for the 'bump' event so that the Bump function is called
    addlistener(c,'bump',@bump);

    %Set the callback for the play_callback
    c.play_callback=@play_callback;
    c.claim_play_callback();

    %Because MATLAB is single threaded, we need to use the main thread
    %to check for events and callbacks.  This is pretty cumbersome but is
    %the only way to handle events and callbacks.  We will check for 10
    %seconds looping with 100 ms delay between checks.

    disp('Waiting for events/callbacks')
    for i=1:100
        %Process any pending events or callbacks
        RobotRaconteur.ProcessRequests();
        pause(.1);
    end
    disp('End waiting for events/callbacks')
    %Disable events when you don't need them
    RobotRaconteur.DisableEvents(c);


    %Demonstrate using a wire

    %Connect the wire
    state_wire=c.create_state.Connect();

    for i=1:10
        try
            %Receive a packet
            state=state_wire.InValue;
            %Print out the data
            disp(state)
        catch e
            %If the error is "Value not set" ignore, otherwise print the error
            if (isempty(strfind(e.message,'Value not set')))
                disp(e.message)
            end
        end
        pause(.1)
    end
    %Close the wire, we are done
    state_wire.Close();

    %Disconnect the service
    RobotRaconteur.DisconnectService(c)

        %Function for Bump event, called on event
        function bump()
           disp('Bump!')
        end

        %Function for play_callback
        function notes=play_callback(distance, angle)
            disp('play_callback!')
            disp(distance)
            disp(angle)

            %Return some notes to play (uint8 type)
            notes=uint8([69,16,60,16,69,16])';
        end

    end
