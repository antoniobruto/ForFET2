module nuclearReactor(x,c1,c2)
	output x,c1,c2;
	
	parameter 
		minTemp = 500,
		maxTemp = 600,
		R0 = 50,
		R1 = 56,
		R2 = 60;

	mode noRods 
		begin
			ddt x = 0.1*x - R0;
			ddt c1 = 1;
			ddt c2 = 1;
		end

	mode rod1 
		begin
			ddt x = 0.1*x - R1;
			ddt c1 = 1;
			ddt c2 = 1;
		end

	mode rod2
		begin
			ddt x = 0.1*x - R2;
			ddt c1 = 1;
			ddt c2 = 1;
		end

	mode meltdown
		begin
			ddt x = 0;
			ddt c1 = 0;
			ddt c2 = 0;
		end

	property inv noRods
		mode == noRods |=> x<=maxTemp && x>=minTemp;
	endproperty

	property inv rod1
		mode == rod1 |=> x<=maxTemp;
	endproperty

	property inv rod2
		mode == rod2 |=> x<=maxTemp;
	endproperty

	property inv meltdown
		mode == meltdown |=> x>=maxTemp;
	endproperty

	property trans noRods_rod1
		mode == noRods && mode' == rod1 && c1>=20 |=> x' == x && c1' == c1 && c2' == c2;
	endproperty

	property trans noRods_rod2
		mode == noRods && mode' == rod2 && c2>=20 |=> x' == x && c1' == c1 && c2' == c2;
	endproperty

	property trans rod1_noRods
		mode == rod1 && mode' == noRods && true|=> x' == x && c1' == 0 && c2' == c2;
	endproperty

	property trans rod1_meltdown
		mode == rod1 && mode' == meltdown && x >= maxTemp |=> x' == x && c1' == 0 && c2' == c2;
	endproperty

	property trans rod2_noRods
		mode == rod2 && mode' == noRods && true |=> x' == x && c1' == c1 && c2' == 0;
	endproperty
	
	property trans rod2_meltdown
		mode == rod2 && mode' == meltdown && x >= maxTemp |=> x' == x && c1' == 0 && c2' == c2;
	endproperty

	property trans noRods_meltdown
		mode == noRods && mode' == meltdown && x >= maxTemp |=> x' == x && c1' == c1 && c2' == c2;
	endproperty

	initial begin 
		set begin
			mode == noRods;
			x == 530;
			c1 == 20;
			c2 == 20;
		end
	end

endmodule
