function spaceex2haslac_octave(xml_file,cfg_file)
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

% Author:       Nikolaos Kekatos
% Written:      11-May-2020



% Default File
if nargin == 0
    xml_file='buck.xml';
    cfg_file='buck.cfg';
elseif nargin==1
    fprintf(['No configuration file has been provided. The analysis will consider \nas ' ...
        'initial conditions zero valued outputs and as initial location the one\n' ...
        'with the smallest SpaceEx index.\n']);
end

% 1: Create a Matlab structure from the XML file
struct_temp = xml2struct_octave(xml_file);
struct=struct_temp.sspaceex;

%%
% 2: Parsing components
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
no_locations=length(struct.component{1}.location);
no_transitions=length(struct.component{1}.transition);
fprintf('\nReading component 1 ...\n\n')
fprintf('The component 1 contains %i locations and %i transitions.\n\n', no_locations,no_transitions)
% Continuous variables and parameters
no_continuous=0;
no_params=0;in=1;
for ii=1:length(struct.component{1}.param)
    try
        if strcmp(struct.component{1}.param{ii}.Attributes.dynamics,'any')
            no_continuous=no_continuous+1;
            name_continuous{in}=char(struct.component{1}.param{ii}.Attributes.name);
            in=in+1;
        elseif strcmp(struct.component{1}.param{ii}.Attributes.dynamics,'const')
            no_params=no_params+1;
        end
    end
end
fprintf('There are %i continuous variables, %i parameters out of %i elements.\n\n',no_continuous, no_params,length(struct.component{1}.param))
fprintf('Finished reading component %i.\n\n',1)

for jj=1:no_params
    name_param{jj}=struct.component{2}.bind.map{jj}.Attributes.key;
    value_param{jj}=struct.component{2}.bind.map{jj}.Text;
end

for id=1:no_locations
    flow_all=struct.component{1}.location{id}.flow.Text;
    flow{id}=strsplit(flow_all,'&');
    for j=1:length(flow{id})
        flow_pieces{id}{j}=strsplit(flow{id}{j},'==');
        flow_right{id}{j}=flow_pieces{id}{j}{2};
        flow_temp_left{id}{j}=strsplit(flow{id}{j},"'");
        flow_left{id}{j}=flow_temp_left{id}{j}{1};
    end
end
for id=1:no_locations
    loc.names{id}=struct.component{1}.location{id}.Attributes.name;
    loc.id{id}=struct.component{1}.location{id}.Attributes.id;
end
%% create Haslac file
haslaac_name_temp=strsplit(xml_file,'.xml');
haslaac_name=haslaac_name_temp(~cellfun('isempty',haslaac_name_temp));
haslaac_name=char(strcat(haslaac_name,'.ha'))
haslaac_module=struct.component{2}.Attributes.id;  % network component name

fid=fopen(haslaac_name,'w');

% regexprep(num2str(n),'\s+',',')

% Create module name and output values
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

% Write Parameters
fprintf(fid,'  parameter \n');

for i_par=1:no_params
    if i_par<no_params
        fprintf(fid,'    %s = % s,\n',name_param{i_par},value_param{i_par});
    elseif i_par==no_params
        fprintf(fid,'    %s = % s;\n\n',name_param{i_par},value_param{i_par});
    end
end

% Write locations and flows

for i1=1:no_locations
    fprintf(fid,'  mode %s\n', struct.component{1}.location{i1}.Attributes.name);
    fprintf(fid,'    begin\n');
    for i2=1:length(flow_left{i1})
        fprintf(fid,'      ddt %s = %s;\n',flow_left{i1}{i2},flow_right{i1}{i2});
    end
    fprintf(fid,'    end\n\n');
end

% Write invariants

for i1=1:no_locations
    fprintf(fid,'  property inv name_%i\n', i1);
    inv_temp=struct.component{1}.location{i1}.invariant.Text;
    inv=strrep(inv_temp,' & ',' && ');
    fprintf(fid,'    mode ==%s |=> %s;\n',struct.component{1}.location{i1}.Attributes.name,inv);
    fprintf(fid,'  endproperty \n\n');
end

% Write Transitions

for it=1:no_transitions
    fprintf(fid,'  property trans trans_%i\n', it);
    source=struct.component{1}.transition{it}.Attributes.source;
    target=struct.component{1}.transition{it}.Attributes.target;
    source_name=char(loc.names(strcmp(loc.id,source)));
    target_name=char(loc.names(strcmp(loc.id,target)));
    inv=strrep(inv_temp,' & ',' && ');
    guard_all=struct.component{1}.transition{it}.guard.Text;
    if guard_all==1
        guard=guard_all;
    else
        guard=strrep(guard_all,' & ',' && ');
    end
    reset_all=struct.component{1}.transition{it}.assignment.Text;
    if reset_all==1
        reset=reset_all;
    else
        reset=strrep(reset_all,' & ',' && ');
    end
    fprintf(fid,"    mode ==%s && mode' == %s && %s |=> %s;\n",source_name,target_name,guard,reset);
    fprintf(fid,'  endproperty \n\n');
end

% Initial Conditions & Initial Locations
if nargin==2
    fid2 = fopen(cfg_file);
    tline = fgetl(fid2);
    disp(tline)           
    lineCounter = 1;
    tline = fgetl(fid2);
    while ischar(tline)
        if strfind(tline, 'initially')
            break;
        end
        % Read next line
        tline = fgetl(fid2);
        disp(tline)
        lineCounter = lineCounter + 1;
    end
    fclose(fid2);
    fprintf('Tline %s \n',tline)
    whos tline;
    in_temp=strsplit(tline,{'&','"'});
    init_parts=in_temp(~cellfun('isempty',in_temp));
    init_parts=init_parts(2:end); % delete cell with initially
    for ic=1:length(init_parts)
        if strfind(init_parts{ic},'loc')
            temp=strsplit(char(init_parts(ic)),'==');
            break;
        end
    end
    
    init_parts{ic}=sprintf(' mode ==%s', char(temp(2))); % keep the right hand side after ==
    fprintf(fid,'  initial begin\n');
    fprintf(fid,'    set begin\n');
    for in=1:length(init_parts)
        fprintf(fid,'      %s;\n',init_parts{in});
    end
    fprintf(fid,'    end \n');
    fprintf(fid,'  end \n\n');
    
elseif nargin<2
    fprintf(fid,'  initial begin\n');
    fprintf(fid,'    set begin\n');
    for idx=1:no_continuous
        fprintf(fid,'       %s==0;\n',name_continuous{idx});
    end
    fprintf(fid,'       mode ==%s;\n',char(loc.names(1)));
    fprintf(fid,'    end \n');
    fprintf(fid,'  end \n\n');
end
fprintf(fid,'endmodule');
fclose(fid);
disp('The translation was completed!')
end
%---------------------------