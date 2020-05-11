module cruiseControl(v,x,t)
	output v,x,t;
	
	parameter 
		minTemp = 500,
		maxTemp = 600,
		R0 = 50,
		R1 = 56,
		R2 = 60;

	mode brake_strong1 
		begin
			ddt v = -t - 2.5;
			ddt x = 0;
			ddt t = 1;
		end

	mode brake_strong2
		begin
			ddt v = -5;
			ddt x = 0;
			ddt t = 0;
		end

	mode brake_weak1
		begin
			ddt v = -t - 1.2;
			ddt x = 0;
			ddt t = 0.5;
		end

	mode brake_weak2
		begin
			ddt v = -2.5;
			ddt x = 0;
			ddt t = 0;
		end

	mode cruise
		begin
			ddt v = -0.001*x - 0.052*v;
			ddt x = v;
			ddt t = 0;
		end

	mode accelerate
		begin
			ddt v = 1.5;
			ddt x = 0;
			ddt t = 0;
		end


	property inv strong1
		mode == brake_strong1 |=> v>=15 && v<=40 && t>=0 && t<=2.5;
	endproperty

	property inv strong2
		mode == brake_strong2 |=> v>=15 && v<=40;
	endproperty

	property inv weak1
		mode == brake_weak1 |=> v>=5 && v<=20 && t>=0 && t<=1.3;
	endproperty

	property inv weak2
		mode == brake_weak2 |=> v>=5 && v<=20;
	endproperty
	
	property inv cruise
		mode == cruise |=> v>=-15 && v<=15 && x>=-500 && x<=500;
	endproperty
	
	property inv accelerate
		mode == accelerate |=> v>=-20 && v<=-5;
	endproperty
	
	property trans s1w1
		mode == brake_strong1 && mode' == brake_weak1 && t>=0 && t<=2.5 && v>=15 && v<=16 |=> t' == 0 && v'==v && x'==x;
	endproperty

	property trans s1s2
		mode == brake_strong1 && mode' == brake_strong2 && t==2.5 && v>=15 && v<=40 |=> t' == t && v'==v && x'==x;
	endproperty
	
	property trans s2w1
		mode == brake_strong2 && mode' == brake_weak1 && t==2.5 && v>=15 && v<=16 |=> t' == 0 && v'==v && x'==x;
	endproperty
	
	property trans w1s1
		mode == brake_weak1 && mode' == brake_strong1 && t>=0 && t<=1.3 && v>=18 && v<=20 |=> t' == 0 && v'==v && x'==x;
	endproperty
	
	property trans w1w2
		mode == brake_weak1 && mode' == brake_weak2 && t==1.3 && v>=5 && v<=20 |=> t' == t && v'==v && x'==x;
	endproperty
	
	property trans w1c
		mode == brake_weak1 && mode' == cruise && t>=0 && t<=1.3 && v>=5 && v<=11 |=> t' == t && v'==v && x'==x;
	endproperty
	
	property trans w2c
		mode == brake_weak2 && mode' == cruise && t==1.3 && v>=5 && v<=11 |=> x' == 0 && v'==v && t'==t;
	endproperty
	
	property trans cw1
		mode == cruise && mode' == brake_weak1 && x>=-500 && x<=500 && v>=13 && v<=15 |=> t' == 0 && v'==v && x'==x;
	endproperty
	
	property trans ca
		mode == cruise && mode' == accelerate && x>=-500 && x<=500 && v>=-15 && v<=-14 |=> t' == t && v'==v && x'==x;
	endproperty
	
	property trans ac
		mode == brake_weak2 && mode' == cruise && x>=-500 && x<=500 && v>=-6 && v<=-5 |=> x' == 0 && v'==v && t'==t;
	endproperty

	initial begin 
		set begin
			mode == brake_strong1;
			x==0;
			t>=0;
			t<=2.5;
			v>=0;
			v<=40;
		end
	end
endmodule
