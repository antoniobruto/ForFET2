module SX_Mesh_original (vx,vy,px,py,I,t)
  output vx,vy,px,py,I,t;

  parameter 
    zeta = 0.9,
    ms = 3.2,
    mg2 = 18.1,
    Jg2 = 0.09,
    Rs = 0.08,
    theta = 0.628318530717959,
    deltap = -0.003,
    Fs = 70,
    Tf = 1,
    vx = vx;

  mode move_free
    begin
      ddt vx = Fs/ms ;
      ddt vy = -Rs*Tf/Jg2 ;
      ddt px = vx ;
      ddt py = vy ;
      ddt I = 0 ;
      ddt t = 1;
    end

  mode meshed
    begin
      ddt false ;
    end

  property inv name_1
    mode ==move_free |=> px<=deltap  && 
py<=-px*0.726542528005361  &&  py>=px*0.726542528005361  && 
t<=T;
  endproperty 

  property inv name_2
    mode ==meshed |=> false;
  endproperty 

  property trans trans_1
    mode ==move_free && mode' == move_free && py>=-px*0.726542528005361  &&  &&  
vx*0.587785252292473+vy*0.809016994374947>0 |=> I:=I+(vx*0.587785252292473+vy*0.809016994374947)*(zeta+1)*ms*mg2/(ms*(0.809016994374947*0.809016994374947)+mg2*(0.587785252292473*0.587785252292473))  && 
vx:=(vx*(ms*0.809016994374947*0.809016994374947-mg2*zeta*0.587785252292473*0.587785252292473)+vy*(-(zeta+1)*mg2*0.587785252292473*0.809016994374947))/(ms*(0.809016994374947*0.809016994374947)+mg2*(0.587785252292473*0.587785252292473))  && 
vy:=(vx*(-(zeta+1)*ms*0.587785252292473*0.809016994374947)+vy*(mg2*0.587785252292473*0.587785252292473-ms*zeta*0.809016994374947*0.809016994374947))/(ms*(0.809016994374947*0.809016994374947)+mg2*(0.587785252292473*0.587785252292473));
  endproperty 

  property trans trans_2
    mode ==move_free && mode' == move_free && py<=px*0.726542528005361  &&  &&  
vx*0.587785252292473-vy*0.809016994374947>0 |=> I:=I+(vx*0.587785252292473-vy*0.809016994374947)*(zeta+1)*ms*mg2/(ms*(0.809016994374947*0.809016994374947)+mg2*(0.587785252292473*0.587785252292473))  && 
vx:=(vx*(ms*0.809016994374947*0.809016994374947-mg2*zeta*0.587785252292473*0.587785252292473)+vy*((zeta+1)*mg2*0.587785252292473*0.809016994374947))/(ms*(0.809016994374947*0.809016994374947)+mg2*(0.587785252292473*0.587785252292473))  && 
vy:=(vx*((zeta+1)*ms*0.587785252292473*0.809016994374947)+vy*(mg2*0.587785252292473*0.587785252292473-ms*zeta*0.809016994374947*0.809016994374947))/(ms*(0.809016994374947*0.809016994374947)+mg2*(0.587785252292473*0.587785252292473));
  endproperty 

  property trans trans_3
    mode ==move_free && mode' == meshed && px>=deltap  &&  vx>=0  &&  vy>=0 |=> I:=I+ms*vx+ms*vy  && 
vx:=0  && 
vy:=0;
  endproperty 

  property trans trans_4
    mode ==move_free && mode' == meshed && px>=deltap  &&  vx>=0  &&  vy<=0 |=> I:=I+ms*vx-ms*vy  && 
vx:=0  && 
vy:=0;
  endproperty 

  property trans trans_5
    mode ==move_free && mode' == meshed && px>=deltap  &&  vx<=0  &&  vy>=0 |=> I:=I-ms*vx+ms*vy  && 
vx:=0  && 
vy:=0;
  endproperty 

  property trans trans_6
    mode ==move_free && mode' == meshed && px>=deltap  &&  vx<=0  &&  vy<=0 |=> I:=I-ms*vx-ms*vy  && 
vx:=0  && 
vy:=0;
  endproperty 

  initial begin
    set begin
       mode ==move_free;
       vy==0;
       px==-0.0165 ;
      py<=0.003001 ;
       I==0 ;
       t==0 ;
      vx==0 ;
       0.002999<=py;
    end 
  end 

endmodule