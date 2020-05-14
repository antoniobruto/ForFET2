function spaceex2haslac(xml_file,cfg_file)
% spaceex2haslac - convert SpaceEx models to HASLAC language
%
% Input:
%   xml_file - SpaceEx model saved as XML

% Output:
%   Text file with the same name as SpaceEx XML

% Usage:
%   spaceex2haslac
%   spaceex2haslac('buck.xml')
%   spaceex2haslac('batt.xml','batt.cfg')
%   spaceex2haslac('GRBX01-MES01.xml')
%   spaceex2haslac('SRNA01-SR0_.xml','SRNA01-SR01.cfg')
%   spaceex2haslac('SRA02-SR0_.xml','SRA02-SR0_.cfg')
%   spaceex2haslac('SRU02-SR0_.xml','SRU02-SR02.cfg')
%   spaceex2haslac('ISSF01.xml','ISSF01-ISS01.cfg')
%   spaceex2haslac('ISSC01.xml','ISSC01-ISS01.cfg')
%   spaceex2haslac('ISSC01.xml','ISSC01-ISS02-plot.cfg')
%   spaceex2haslac('BLDC01.xml','BLDC01-BDS01.cfg')
%   spaceex2haslac('ballstring.xml','ballstring_segm.cfg')

% Author:       Nikolaos Kekatos
% Written:      11-May-2020

license('inuse')
% matlab.codetools.requiredFilesAndProducts

if nargin == 0 % Default File
    xml_file='SRA01.xml';
    cfg_file='SRA01.cfg';
elseif nargin==1
    fprintf(['No configuration file has been provided. The analysis will consider \nas ' ...
        'initial conditions zero valued outputs and as initial location the one\n' ...
        'with the smallest SpaceEx index.\n']);
end

%%  Create a Matlab structure from the XML file
struct_temp = xml2struct(xml_file);
struct=struct_temp.sspaceex;

%% Parsing Components and Selecting Elements
no_comp=length(struct.component);
if no_comp>2
    error('We cannot parse network of hybrid automata. We support one base and one network component')
elseif no_comp==1
    fprintf('The model contains one base component which defines a single hybrid automaton.')
elseif no_comp==2
    fprintf(['\nThe  model contains two components. The first is a base component \n' ...
        'which defines a single hybrid automaton and the second is a network component \n' ...
        'which is used for defining the parameter values.\n']);
end
if no_comp==1
    compon=struct.component;
elseif no_comp==2
    compon=struct.component{1};
end
no_locations=length(compon.location);
try
    no_transitions=length(compon.transition);
catch
    no_transitions=0;
end
fprintf('\nReading component 1 ...\n\n')
fprintf('The component 1 contains %i locations and %i transitions.\n\n', no_locations,no_transitions)

%%% Continuous variables and parameters
no_continuous=0;
no_params=0;in=1;no_index=[];in_p=1;
for ii=1:length(compon.param)
    try
        if strcmp(compon.param{ii}.Attributes.dynamics,'any')
            no_continuous=no_continuous+1;
            name_continuous{in}=char(compon.param{ii}.Attributes.name);
            in=in+1;
        elseif strcmp(compon.param{ii}.Attributes.dynamics,'const')
            no_params=no_params+1;
            % no_index=[no_index;ii];
            name_param{in_p}=char(compon.param{ii}.Attributes.name);
            in_p=in_p+1;
        end
    end
end
fprintf('There are %i continuous variables, %i parameters out of %i elements.\n\n',no_continuous, no_params,length(compon.param))
fprintf('Finished reading component %i.\n\n',1)

if no_comp==2
    for ij=1:length(struct.component{2}.bind.map)
        map_all{ij}=struct.component{2}.bind.map{ij}.Attributes.key; % all parameters including states and constants
    end
    for jj=1:no_params
        % problematic if local variables exist: name_param{jj}=struct.component{2}.bind.map{no_index(jj)}.Attributes.key;
        % workaround:
        % map_all: contains all parameters
        % name_param: contains only constants
        % need to find name_param in map_all and then get the value (stored in
        % Text field).
        
        % name_param{jj} % used for testing
        index=find(ismember(map_all,name_param{jj}));
        % map_all{index} % used for testing
        value_param{jj}=struct.component{2}.bind.map{index}.Text;
    end
end

for id=1:no_locations
    if no_locations~=1
        flow_all=compon.location{id}.flow.Text;
    elseif no_locations==1
        flow_all=compon.location.flow.Text;
    end
    %%% Flows
    flow{id}=strsplit(flow_all,'&');
    flow{id}=remove_newline(flow{id});
    for j=1:length(flow{id})
        flow_pieces{id}{j}=strsplit(flow{id}{j},'==');
        if numel(flow_pieces{id}{j})==2
            flow_right{id}{j}=flow_pieces{id}{j}{2};
            flow_temp_left{id}{j}=strsplit(flow{id}{j},"'");
            flow_left{id}{j}=flow_temp_left{id}{j}{1};
        elseif numel(flow_pieces{id}{j})==1
            flow_left{id}{j}=flow_pieces{id}{j};
        end
    end
end

%%% Locations names and id
for id=1:no_locations
    if no_locations~=1
        loc.names{id}=compon.location{id}.Attributes.name;
        loc.id{id}=compon.location{id}.Attributes.id;
    elseif no_locations==1
        loc.names{id}=compon.location.Attributes.name;
        loc.id{id}=compon.location.Attributes.id;
    end
end

%% create Haslac file
%%% The name should be the same for module and filename
haslaac_name_temp=strsplit(xml_file,'.xml');
haslaac_name_no_extension=haslaac_name_temp(~cellfun('isempty',haslaac_name_temp));
haslaac_name=char(strcat(haslaac_name_no_extension,'.ha'))
% try
%     haslaac_module=struct.component{2}.Attributes.id;  % network component name
% catch
%     haslaac_module=compon.Attributes.id;  % base component name
% end
haslaac_module=char(haslaac_name_no_extension); %needed for ForFET

fid=fopen(haslaac_name,'w');

% regexprep(num2str(n),'\s+',',')

%%% Create module name and output values
variables=[];
for i_var=1:length(name_continuous)
    if i_var<length(name_continuous)
        variables=[variables,name_continuous{i_var},','];
    else
        variables=[variables,name_continuous{i_var}];
    end
end
fprintf(fid,'module %s (%s)\n',haslaac_module,variables);
fprintf(fid,'  output %s;\n\n',variables);

%%% Write Parameters
if no_params
    fprintf(fid,'  parameter \n');
end
for i_par=1:no_params
    if i_par<no_params
        fprintf(fid,'    %s = % s,\n',name_param{i_par},value_param{i_par});
    elseif i_par==no_params
        fprintf(fid,'    %s = % s;\n\n',name_param{i_par},value_param{i_par});
    end
end

%%% Write locations and flows

for i1=1:no_locations
    if no_locations~=1
        fprintf(fid,'  mode %s\n', compon.location{i1}.Attributes.name);
    elseif no_locations==1
        fprintf(fid,'  mode %s\n', compon.location.Attributes.name);
    end
    fprintf(fid,'    begin\n');
    for i2=1:length(flow_left{i1})
        try
            fprintf(fid,'      ddt %s = %s;\n',flow_left{i1}{i2},flow_right{i1}{i2});
        catch
            fprintf(fid,'      ddt %s ;\n',char(flow_left{i1}{i2}));
        end
    end
    fprintf(fid,'    end\n\n');
end

%%% Write invariants
debug_bool=1;
for i1=1:no_locations
    fprintf(fid,'  property inv name_%i\n', i1);
    if no_locations~=1
        inv_temp=compon.location{i1}.invariant.Text;
    elseif no_locations==1
        inv_temp=compon.location.invariant.Text;
    end
    %     inv_temp=break_inequalities_char(inv_temp);
    warning('Need to manually replace a<=x<=b for invariants.')
    inv_temp=remove_parenthesis(inv_temp);
    if debug_bool
        inv=strrep(inv_temp,'&',' && ');
    else
        inv=strrep(inv_temp,' & ',' && ');
    end
    warning('This will not work with models including "&&" by default');
    if no_locations~=1
        fprintf(fid,'    mode ==%s |=> %s;\n',compon.location{i1}.Attributes.name,inv);
    elseif no_locations==1
        fprintf(fid,'    mode ==%s |=> %s;\n',compon.location.Attributes.name,inv);
    end
    fprintf(fid,'  endproperty \n\n');
end

% Write Transitions

for it=1:no_transitions
    fprintf(fid,'  property trans trans_%i\n', it);
    if no_transitions~=1
        source=compon.transition{it}.Attributes.source;
        target=compon.transition{it}.Attributes.target;
    elseif no_transitions==1
        source=compon.transition.Attributes.source;
        target=compon.transition.Attributes.target;
    end
    source_name=char(loc.names(strcmp(loc.id,source)));
    target_name=char(loc.names(strcmp(loc.id,target)));
    if no_transitions~=1
        guard_all=compon.transition{it}.guard.Text;
    elseif no_transitions==1
        guard_all=compon.transition.guard.Text;
    end
    warning('The `guard_all` check might not be needed anymore.');
    if guard_all==1
        guard=guard_all;
    else
        if debug_bool
            guard=strrep(guard_all,'&',' && ');
        else
            guard=strrep(guard_all,' & ',' && ');
        end
    end
    try
        if no_transitions~=1
            reset_all=compon.transition{it}.assignment.Text;
        else
            reset_all=compon.transition.assignment.Text;
        end
        if reset_all==1 % Same with guard_all
            reset=reset_all;
        else
            if debug_bool
                reset=strrep(reset_all,'&',' && ');
            else
                reset=strrep(reset_all,' & ',' && ');
            end
        end
    catch
        fprintf('\n There are no resets in transition %i.\n',it);
        warning(" ForFET requires at least one reset, i.e. `x'==x`");
        reset=strcat(variables(1),"'==",variables(1));
    end
    fprintf(fid,"    mode ==%s && mode' == %s && %s |=> %s;\n",source_name,target_name,guard,reset);
    fprintf(fid,'  endproperty \n\n');
end

% Initial Conditions & Initial Locations
if nargin==2 ||nargin==0
    fid2 = fopen(cfg_file);
    tline = fgetl(fid2);
    lineCounter = 1;
    while ischar(tline)
        if contains(tline, 'initially')
            break;
        end
        % Read next line
        tline = fgetl(fid2);
        lineCounter = lineCounter + 1;
    end
    fclose(fid2);
    disp(tline);
    in_temp=strsplit(tline,{'&','"'});
    init_parts=in_temp(~cellfun('isempty',in_temp));
    init_parts=init_parts(2:end); % delete cell with initially
    temp=[];
    for ic=1:length(init_parts)
        if contains(init_parts(ic),'loc')
            temp=strsplit(char(init_parts(ic)),'==');
            break;
        end
    end
    if ~isempty(temp)
        init_parts{ic}=sprintf(' mode ==%s', char(temp(2))); % keep the right hand side after ==
    else
        if no_locations==1
            init_parts{ic}=sprintf(' mode ==%s', char(compon.location.Attributes.name)); % one location or not specified by the CFG
        else
            warning('Not implemented yet. Missing initial location.');
            warning('Manually add `mode==loc_name`');
        end
    end
    if ic~=1
        % Need to move mode/loc to the top of the initial conditions
        temp_1=init_parts{1};
        init_parts{1}=init_parts{ic};
        init_parts{ic}=temp_1;
        init_parts_final=break_inequalities(init_parts);
    else
        init_parts_final=init_parts;
    end
    fprintf(fid,'  initial begin\n');
    fprintf(fid,'    set begin\n');
    for in=1:length(init_parts_final)
        fprintf(fid,'      %s;\n',init_parts_final{in});
    end
    fprintf(fid,'    end \n');
    fprintf(fid,'  end \n\n');
    
elseif nargin<2
    fprintf(fid,'  initial begin\n');
    fprintf(fid,'    set begin\n');
    fprintf(fid,'       mode ==%s;\n',char(loc.names(1)));
    for idx=1:no_continuous
        fprintf(fid,'       %s==0;\n',name_continuous{idx});
    end
    fprintf(fid,'    end \n');
    fprintf(fid,'  end \n\n');
end
fprintf(fid,'endmodule');
fclose(fid);

disp('The translation was completed!')
end

function out=remove_parenthesis(in)

out=erase(in,"(");
out=erase(out,")");

end


function out=break_inequalities(in)
% in: cell array, each element char
% 0<=x<=10 and 0>=x>=-1
% if two inequality symbols, need to break the expression in two parts.
% need to create 0<=x and x<=10
out=cell(length(in)*2,1); % worst case every expression needs breaking in parts
for idx=1:length(in)
    if length(strfind(in{idx},'<='))==2  % contains 2 '<='
        temp_1=extractAfter(in{idx},'<='); % keeps x<=10
        idx_2=strfind(in{idx},'<=');
        temp_2=char(extractBetween(in{idx},1,idx_2(end)-1));
        out{idx}=temp_1;
        out{idx+10}=temp_2;
    elseif  length(strfind(in{idx},'>='))==2
        temp_1=extractAfter(in{idx},'>='); % keeps x<=10
        idx_2=strfind(in{idx},'>=');
        temp_2=extractBetween(in{idx},1,idx_2(end)-1);
        out{idx}=temp_1;
        out{idx+10}=temp_2;
    elseif length(strfind(in{idx},'<'))==2  % contains 2 '<='
        temp_1=extractAfter(in{idx},'<'); % keeps x<=10
        idx_2=strfind(in{idx},'<');
        temp_2=extractBetween(in{idx},1,idx_2(end)-1);
        out{idx}=temp_1;
        out{idx+10}=temp_2;
    elseif  length(strfind(in{idx},'>'))==2
        temp_1=extractAfter(in{idx},'>'); % keeps x<=10
        idx_2=strfind(in{idx},'>');
        temp_2=extractBetween(in{idx},1,idx_2(end)-1);
        out{idx}=temp_1;
        out{idx+10}=temp_2;
    else
        out{idx}=in{idx};
    end
end
out=out(~cellfun('isempty',out));
end

function out=remove_newline(in)
% in: cell array, each element char

for idx=1:length(in)
    out{idx}=regexprep(in{idx},'\n+','');
end
end

