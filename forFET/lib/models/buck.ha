module buck(v,i,t)
	output v,i,t;
	
	parameter 
		a00o = 0,
		a01o = -21052.6316,
		a10o = 38095.2381,
		a11o = -40100.2506,
		a00c = 0,
		a01c = -21052.6316,
		a10c = 38095.2381,
		a11c = -40100.2506,
		bounds = 1000,
		T = 1e-05,
		b0o = 0,
		b1o = 0,
		b0c = 21052.6316,
		b1c = 0,
		Vs = 12,
		D = 0.51667;
		

	mode closed 
		begin
			ddt t = 1;
			ddt v = (a10c * i + a11c * v + b1c * Vs);
			ddt i = (a00c * i + a01c * v + b0c * Vs);
		end

	mode open 
		begin
			ddt t = 1;
			ddt v = (a10o * i + a11o * v + b1o * Vs);
			ddt i = (a00o * i + a01o * v + b0o * Vs);
		end

	property inv closed
		mode == closed |=> v<=bounds && v>=-bounds && i<=bounds && i>=-bounds && t<=D * T && t>=0;
	endproperty

	property inv precharge
		mode == open |=> v<=bounds && v>=-bounds && i<=bounds && i>=-bounds && t<=(1-D) * T && t>=0;
	endproperty

	property trans closed_open
		mode == closed && mode' == open && t>=D * T |=> i'==i && t'==0 && v'==v;
	endproperty

	property trans open_closed
		mode == open && mode' == closed && t>=(1-D) * T |=> i'==i && t'==0 && v'==v;
	endproperty

	initial begin 
		set begin
			mode == closed;
			i == 0;
			v == 0;
			t == 0;
		end
	end
			
endmodule
