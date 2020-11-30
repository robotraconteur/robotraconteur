function c=RobotRaconteurMemoryClient(objecttype,stubid,membername)
%RobotRaconteurMemoryClient Access memory member on the service
%    Memories represent random access memory regions that are typically
%    represented as arrays of various shapes and types. Memories can be 
%    declared in service definition files using the `memory` member keyword
%    within service definitions. Services expose memories to clients, and 
%    the nodes will proxy read, write, and parameter requests between the client 
%    and service. The node will also break up large requests to avoid the
%    message size limit of the transport.
%
%    Memory clients are retrieved using clients connected with 
%    RobotRaconteur.ConnectService(). For example, to retrieve the my_memory
%    wire client, use:
%
%        my_memory = robot_client.get_my_memory()
%
%    RobotRaconteurMemoryClient provides transparent access to the memory member.
%    Standard array slicing operations can be used, as well as length, ndims,
%    numel, and size operations. To retreive or assign the entire memory, use
%    the (:) slice operation. For example:
%
%        % Set entire memory to 1
%        my_memory(:) = 1
%
%        % Get the entire memory
%        my_memory_conents = my_memory(:)
%
%    See also RobotRaconteurObject, RobotRaconteur.ConnectService, length, ndims,
%        numel, size
c=struct;
c.rrobjecttype=objecttype;
c.rrstubid=stubid;
c.rrmembername=membername;

c=class(c,'RobotRaconteurMemoryClient');