function display(c)
%display Display information about object
disp(' ')
disp([inputname(1) ' ='])

disp(' ')

try

disp(['  ' type(c) ' - <a href="http://robotraconteur.com">Robot Raconteur</a> object reference'])

s=struct;
w=struct;

try
    lines1=textscan(members(c),'%s','delimiter','\n');
    lines=lines1{1};
    for i=1:length(lines)
        try
            line=lines{i};
            p1=textscan(line,'%s');
            p=p1{1};
            if (strcmp(p{1},'property'))
               S=substruct('.',p{3});               
               s.(p{3})=subsref(c,S);
            end
            if (strcmp(p{1},'wire'))
               S=substruct('.',p{3},'.','PeekInValue','()','');
               w.(p{3})=subsref(c,S);
            end
        catch member_err
            if contains(member_err.message,'RequestTimeout')
                break
            end
        end
    end
catch
end

if (length(fieldnames(s)) >0 && length(fieldnames(w)) > 0)
   disp(' ')
end

if (length(fieldnames(s))) > 0
   disp('  Properties:')
   disp(s)
end
if (length(fieldnames(w))) > 0
   disp('  Wires:')
   disp(w)
end
disp(['  <a href="matlab: members(' inputname(1) ')">Object Members</a>'])

catch
    disp('  Robot Raconteur object')
end
    disp(' ')
end



