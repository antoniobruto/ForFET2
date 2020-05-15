module ballstring (x,v,t)
  output x,v,t;

  parameter 
    L = 1,
    x_r = 0,
    c = 0.8,
    k = 100,
    d = 4,
    m = 1,
    g = 10;

  mode freefall
    begin
      ddt x = v ;
      ddt v = -g ;
      ddt  t = 1;
    end

  mode extension
    begin
      ddt x = v ;
    end

  property inv name_1
    mode ==freefall |=> x_r <= x <= x_r+L;
  endproperty 

  property inv name_2
    mode ==extension |=> x <= x_r;
  endproperty 

  property trans trans_1
    mode ==freefall && mode' == extension && v<0 |=> x'==x;
  endproperty 

  property trans trans_2
    mode ==extension && mode' == freefall && v>0 |=> x'==x;
  endproperty 

  property trans trans_3
    mode ==freefall && mode' == freefall && x == x_r+L  &&  v > 0 |=> v'==-c*v;
  endproperty 

  initial begin
    set begin
       mode ==freefall;
       x==0;
       v==0;
       t==0;
    end 
  end 

endmodule