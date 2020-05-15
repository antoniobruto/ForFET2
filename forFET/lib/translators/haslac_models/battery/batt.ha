module batt (v,i)
  output v,i;

  parameter 
    Vinit = 0,
    Vdischarge = -10e-5,
    Vterm = 4.2,
    Vfullrate = 3,
    Vrestart = 4,
    Iprecharge = 0.05,
    Ichg = 1,
    IEOC = 0.025,
    E = 0.005,
    T1 = 1200,
    T2 = 150,
    T3 = 150,
    T4 = 1200;

  mode off
    begin
      ddt v = -Vdischarge ;
      ddt  i = 0;
    end

  mode precharge
    begin
      ddt v = ( Vfullrate + E - v ) / T1 ;
      ddt  i = 0;
    end

  mode constantCurrent
    begin
      ddt v = ( Vterm + E - v ) / T2 ;
      ddt  i = 0;
    end

  mode constantVoltage
    begin
      ddt v = 0 ;
      ddt  i = -i / T3;
    end

  mode maintenance
    begin
      ddt v = -v / T4 ;
      ddt  i = 0;
    end

  property inv name_1
    mode ==off |=> i==0  &&  v<=Vterm  &&  0<=v;
  endproperty 

  property inv name_2
    mode ==precharge |=> i==Iprecharge  &&  v<=Vfullrate  &&  0<=v;
  endproperty 

  property inv name_3
    mode ==constantCurrent |=> i<=Ichg  &&  0<=i  &&  v<=Vterm  &&   Vfullrate - E <=v;
  endproperty 

  property inv name_4
    mode ==constantVoltage |=> i<=Ichg  &&   IEOC - E <=i  &&  v<= Vterm + E   &&   Vterm - E <=v;
  endproperty 

  property inv name_5
    mode ==maintenance |=> i<=Ichg  &&  0<=i  &&  v<=Vterm  &&  Vrestart<=v;
  endproperty 

  property trans trans_1
    mode ==off && mode' == precharge && true |=> i'==Iprecharge  &&  v'==v;
  endproperty 

  property trans trans_2
    mode ==precharge && mode' == constantCurrent && v<=Vfullrate  &&  v>=( Vfullrate - E ) |=> i'==Ichg  &&  v'==v;
  endproperty 

  property trans trans_3
    mode ==constantCurrent && mode' == constantVoltage && v<=( Vterm + E )  &&  ( Vterm - E )<=v |=> i'==i  &&  v'==v;
  endproperty 

  property trans trans_4
    mode ==constantVoltage && mode' == maintenance && i<=IEOC |=> i'==i  &&  v'==v;
  endproperty 

  property trans trans_5
    mode ==maintenance && mode' == constantCurrent && v<=( Vrestart + E ) |=> i'==Ichg  &&  v'==v;
  endproperty 

  initial begin
    set begin
       mode == off ;
       v == 0 ;
       i == 0;
    end 
  end 

endmodule