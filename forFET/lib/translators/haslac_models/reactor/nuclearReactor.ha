module nuclearReactor (x,c1,c2)
  output x,c1,c2;

  parameter 
    minTemp = 500,
    maxTemp = 600,
    R0 = 50,
    R1 = 56,
    R2 = 60;

  mode noRods
    begin
      ddt x = 0.1 * x - R0 ;
      ddt  c1 = 1 ;
      ddt  c2 = 1;
    end

  mode rod1
    begin
      ddt x = 0.1 * x - R1 ;
      ddt  c1 = 1 ;
      ddt  c2 = 1;
    end

  mode rod2
    begin
      ddt x = 0.1 * x - R2 ;
      ddt  c1 = 1 ;
      ddt  c2 = 1;
    end

  mode meltdown
    begin
      ddt x = 0 ;
      ddt  c1 = 0 ;
      ddt  c2 = 0;
    end

  property inv name_1
    mode ==noRods |=> x>=minTemp  &&  x<=maxTemp;
  endproperty 

  property inv name_2
    mode ==rod1 |=> x>=540  &&  x<=maxTemp;
  endproperty 

  property inv name_3
    mode ==rod2 |=> x>=540  &&  x<=maxTemp;
  endproperty 

  property inv name_4
    mode ==meltdown |=> c2<=20  &&  c1<=20  &&  x>=minTemp  &&  x<=maxTemp;
  endproperty 

  property trans trans_1
    mode ==noRods && mode' == rod1 && c1>=20  &&  x==550 |=> x'==x  &&  c1'==c1  &&  c2'==c2;
  endproperty 

  property trans trans_2
    mode ==noRods && mode' == rod2 && c2>=20  &&  x==590 |=> x'==x  &&  c1'==c1  &&  c2'==c2;
  endproperty 

  property trans trans_3
    mode ==noRods && mode' == meltdown && x>( R1 - 1 ) * 10  &&  c2<20  &&  c1<20 |=> x'==x  &&  c1'==c1  &&  c2'==c2;
  endproperty 

  property trans trans_4
    mode ==noRods && mode' == meltdown && x>( R2 - 1 ) * 10  &&  c2<20 |=> x'==x  &&  c1'==c1  &&  c2'==c2;
  endproperty 

  property trans trans_5
    mode ==rod1 && mode' == noRods && x==540 |=> x'==x  &&  c1'==0  &&  c2'==c2;
  endproperty 

  property trans trans_6
    mode ==rod2 && mode' == noRods && x==540 |=> x'==x  &&  c1'==c1  &&  c2'==0;
  endproperty 

  initial begin
    set begin
       mode == noRods ;
       c2 == 20 ;
       c1 == 20 ;
       x == 530;
    end 
  end 

endmodule