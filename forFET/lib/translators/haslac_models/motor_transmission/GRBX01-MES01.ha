module GRBX01-MES01 (vx,vy,px,py,I,t)
  output vx,vy,px,py,I,t;

  parameter 
    zeta = 0.9,
    ms = 3.2,
    mg2 = 18.1,
    Jg2 = 0.7,
    Rs = 0.08,
    theta = 0.628318530717959,
    deltap = -0.003,
    Fs = 70,
    Tf = 1;

  mode move_free
    begin
      ddt vx = Fs/ms ;
      ddt 
vy = -Rs*Tf/Jg2 ;
      ddt 
px = vx ;
      ddt 
py = vy ;
      ddt 
I = 0;
    end

  mode meshed
    begin
      ddt false 
    end

  property inv name_1
    mode ==move_free |=> px<=deltap  && 
py<=-px*0.726542528005361  &&  py>=px*0.726542528005361  && 
t'==1;
  endproperty 

  property inv name_2
