INSERT INTO bank_schema.clients(first_name, middle_name,last_name,gender,passport_series,passport_number,issuing,issuing_date,id_number,
	birth_address,current_city,current_address,city_of_residence,residence_address,family_status,citizenship,retiree,monthly_income,conscription) 
VALUES('pavel','gennadevich','sidorovich',1,'KH','2462649','Волковысский РОВД','2014-01-01','id_number',
	'birth_address','current_city','current_address','city_of_residence','residence_address',0,'citizenship',false,100,false) 
RETURNING id
