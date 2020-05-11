module batteryCharger(v,i)
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
			ddt v = -Vdischarge;
			ddt i = 0;
		end

	mode precharge 
		begin
			ddt v = ( Vfullrate + E - v ) / T1;
			ddt i = 0;
		end

	mode constantCurrent 
		begin
			ddt v = (Vterm + E - v)/T2;
			ddt i = 0;
		end

	mode constantVoltage 
		begin
			ddt v = 0;
			ddt i = -i/T3;
		end

	mode maintenance 
		begin
			ddt v = -v/T4;
			ddt i = 0;
		end

	property inv off
		mode == off |=> 0<=v && v<=Vterm && i==0;
	endproperty

	property inv precharge
		mode == precharge |=> 0<=v && v<=Vfullrate && i==Iprecharge;
	endproperty

	property inv constantCurrent
		mode == constantCurrent |=> (Vfullrate-E)<=v && v<=Vterm && 0<=i && i<=Ichg;
	endproperty

	property inv constantVoltage
		mode == constantVoltage |=> (Vterm-E)<=v && v<=(Vterm+E) && (IEOC-E)<=i && i<=Ichg;
	endproperty

	property inv maintenance
		mode == maintenance	|=> Vrestart<=v && v<=Vterm && 0<=i && i<=Ichg;
	endproperty

	property trans off_pc
		mode == off && mode' == precharge && true |=> i'==Iprecharge && v'==v;
	endproperty

	property trans pc_cc
		mode == precharge && mode' == constantCurrent && v>=(Vfullrate - E) && v<=Vfullrate |=> i'==Ichg && v'==v;
	endproperty

	property trans cc_cv
		mode == constantCurrent && mode'==constantVoltage && (Vterm-E)<=v && v<=(Vterm+E) |=> i'==i && v'==v;
	endproperty

	property trans cv_maint
		mode == constantVoltage && mode'==maintenance && i<=IEOC |=> i'==i && v'==v;
	endproperty

	property trans main_cc
		mode == maintenance && mode'==constantCurrent && v<=(Vrestart+E) |=> i'==Ichg && v'==v;
	endproperty

	initial begin 
		set begin
			mode == off;
			i == 0;
			v == 0;
		end
	end
			
endmodule
