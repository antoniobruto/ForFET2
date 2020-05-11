module thermostat(temp,u)
	output temp,u;


	mode loc
		begin
   		ddt temp = 0.5*u - 0.5*temp;
		end


	initial begin 
		set begin
			mode == loc;
			temp == 65;
			u == 70;
		end
	end

endmodule	
