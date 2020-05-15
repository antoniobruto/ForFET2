module rdv_safe (x,y,vx,vy,t)
  output x,y,vx,vy,t;

  mode P2
    begin
      ddt t = 1 ;
      ddt  x = vx ;
      ddt  y = vy ;
      ddt  vx = -0.057599765881773*x+0.000200959896519766*y-2.89995083970656*vx+0.00877200894463775*vy ;
      ddt  vy = -0.000174031357370456*x-0.0665123984901026*y-0.00875351105536225*vx-2.90300269286856*vy;
    end

  mode P3
    begin
      ddt t = 1 ;
      ddt  x = vx ;
      ddt  y = vy ;
      ddt  vx = -0.575999943070835*x+0.000262486079431672*y-19.2299795908647*vx+0.00876275931760007*vy ;
      ddt  vy = -0.000262486080737868*x-0.575999940191886*y-0.00876276068239993*vx-19.2299765959399*vy;
    end

  mode Passive
    begin
      ddt t = 1 ;
      ddt  x = vx ;
      ddt  y = vy ;
      ddt  vx = 0.0000575894721132000*x+0.00876276*vy ;
      ddt  vy = -0.00876276*vx ;
    end

  property inv name_1
    mode ==P2 |=> t<=200  &&  x<=-100;
  endproperty 

  property inv name_2
    mode ==P3 |=> t<=200  &&  y>=-100  &&  x+y>=-141.1  &&  x>=-100  &&  y-x<=141.1  &&  y<=100  &&  x+y<=141.1  &&  x<=100  &&  y-x>=-141.1;
  endproperty 

  property inv name_3
    mode ==Passive |=> t>=120  &&  t<=300;
  endproperty 

  property trans trans_1
    mode ==P2 && mode' == P3 && t<=200  &&  y>=-100  &&  x+y >=-141.1  &&  x>=-100  &&  y-x<=141.1  &&  y<=100  &&  x+y<=141.1  &&  x<=100  &&  y-x>=-141.1 |=> x'==x;
  endproperty 

  initial begin
    set begin
       mode ==P2;
      y<=-375 ;
       vx==0 ;
       vy==0 ;
       t==0 ;
      x<=-875 ;
       -425<=y;
      -925<=x;
    end 
  end 

endmodule