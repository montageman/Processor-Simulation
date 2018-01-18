//Phillip Yellott

#include <sys/time.h>
#include <sys/types.h>
#include <poll.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include <unistd.h>
using namespace std;

int fetch(int fd, fd_set rfds,timeval tv) 
{  
	int fetched;
 
   
   	if (select (fd+1, &rfds, NULL,NULL,&tv))
    	read(fd, &fetched, 4);

   	return fetched;
}

void end_check(int checkval, bool mode)
{
   	if (checkval>999 && !mode)
   	{
      	cout << "Unauthorized access" << endl<< "Terminating" <<endl;
      	_Exit(1);
   	}
}



int main(int argc, char * argv[])
{
   	int cpu_to_memory_pipe[2];
   	int memory_to_cpu_pipe[2];
   

   	if (pipe(cpu_to_memory_pipe) == -1) 
   	{
      	cout << "error piping 1" << endl;
      	return -1;
   	}

   	if (pipe(memory_to_cpu_pipe) == -1)
   	{
      	cout << "error piping 2" << endl;
      	return -1;
   	}

   

   

// ------------------   CPU    -------------------------------------------

   	int pid;
   	if (pid = fork()) //This is an easy way for me to fork.
   	{
      	
      	
      	// zero is read, one is write
      	close(cpu_to_memory_pipe[0]);
      	close(memory_to_cpu_pipe[1]);

      	int buffer;
     
        //the following two blocks sre used for a test in fetch()

      	//set up timeout for check()
      	struct timeval tv;
      	tv.tv_sec = 5;
      	tv.tv_usec = 0;
      
      	//set up file descriptor to test for content
      	fd_set rfds;
      	FD_ZERO (&rfds);
      	FD_SET (memory_to_cpu_pipe[0], &rfds);

       
      
      	int PC= 0, SP = 993, IR= 50, AC=0, X=0, Y=0;
      	bool kernalmode = false;
      	int timer = atoi(argv[2]); 
      	int count_timer = 0;


      	//exection loop process
      	bool running = true;
      	while (running)
      	{
	
        int FIND_IT = 1005;
        int result;
        
	
		write(cpu_to_memory_pipe[1], &PC, 4);
        IR = fetch(memory_to_cpu_pipe[0],rfds ,tv);  
	

         
        switch (IR) 
        {
           	case 1:
	    		//AC = memory[PC+1];
	    		PC++;
	    		write(cpu_to_memory_pipe[1],&PC, 4); 			
         		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
                
	       		break;

	    	case 2:
            	PC++;    
               
	       		//get next memory location
	       		write(cpu_to_memory_pipe[1], &PC, 4);
	       		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
               	end_check(AC,kernalmode);

	       		//get the memory at that location
	       		write(cpu_to_memory_pipe[1], &AC, 4);
               	AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
               	end_check(AC,kernalmode);

       			break;

	    	case 3:
				//AC = memory[ memory[AC] ];
              	PC++;

	       		write(cpu_to_memory_pipe[1], &PC, 4);
	       		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
               	end_check(AC,kernalmode);

	       		//get the memory at that location
	       		write(cpu_to_memory_pipe[1], &AC, 4);
	       		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
            	end_check(AC,kernalmode);
	
	       		//get the last value in our chain
               	write(cpu_to_memory_pipe[1], &AC, 4);
	       		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
               	end_check(AC,kernalmode);

               	break;

	    	case 4:
               //AC = memory[ memory[PC+1] +X ] 
               
	       		PC++;
                
                //nested AC query
	       		write(cpu_to_memory_pipe[1], &PC, 4);
	       		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
               	end_check(AC,kernalmode);
               
	       		AC = AC + X;
	       		write(cpu_to_memory_pipe[1], &AC, 4);
	       		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
               	end_check(AC,kernalmode);
	       
	       		break;
	    

	    	case 5:
               //AC = memory[ memory[PC+1] + Y] 
               
	       		PC++;

	       		write(cpu_to_memory_pipe[1], &PC, 4);
	       		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
            	end_check(AC,kernalmode);
               
	       		AC = AC + Y;
	       
	       		write(cpu_to_memory_pipe[1], &AC, 4);
	       		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
               	end_check(AC,kernalmode);


	       		break;

	    	case 6:
	       		//AC = memory[SP+X];
               
	       		PC++;
               	//temp store SP+X
	       		AC = SP + X; 
	       		//and then search for the value given 
	       		write(cpu_to_memory_pipe[1], &AC, 4);
	       		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
               	end_check(AC,kernalmode);
               
	       		break;


	    	case 7:
	       		// Save AC into memory
	       
	       		PC++;
	       
	       		write(cpu_to_memory_pipe[1], &PC, 4);
	       		AC = AC + (10000 * fetch(memory_to_cpu_pipe[0], rfds, tv));
               
               	AC = AC + 100000000; 
  
               	end_check(AC%10000,kernalmode);

	       		write(cpu_to_memory_pipe[1], &AC, 4);
            	   
               	AC = (AC - 100000000) %10000;
	       		break;


	    	case 8:
               	//AC = random int with range=[1,100];
               	AC = rand()%100 + 1;
	       		break;


	    	case 9:
               	//fetch next memory, if 1, cout int, if 2 cout char, else error
	       		PC++;
               
	          
	       		write(cpu_to_memory_pipe[1], &PC, 4);
	        
	       		if ( fetch(memory_to_cpu_pipe[0], rfds, tv)==1)
	          		cout << AC; //AC is already an int
                
	       		else 
	          		cout << (char) AC;// << endl;
	       		break;

	    	case 10:
               	AC = AC + X;
	       		break;
  
            case 11:
	       		AC = AC + Y;
	       		break;

	    	case 12:
	       		AC = AC - X;
	       		break;

	    	case 13:
	       		AC = AC - Y;
	       		break;

	    	case 14:
	       		X = AC;
	       		break;

	    	case 15:
	       		AC = X;
	       		break;

	    	case 16:
	       		Y = AC;
	       		break;

	    	case 17:
	       		AC = Y;
	       		break;

	    	case 18: 
	       		SP = AC;
	       		break;

	    	case 19:
	       		AC = SP;
	       		break;	    
 
            case 20:
	       		// PC = IR-1; 
	       		///we sub 1 because we add 1 to it at the end of the switch.
               
	       		PC++;
               
	       		//get next line and save the result to PC
	       		write(cpu_to_memory_pipe[1], &PC, 4);
	       		PC = fetch(memory_to_cpu_pipe[0], rfds, tv)-1;
               	end_check(PC,kernalmode);
               
	       		break;

            case 21:
               	//if AC == 0, jump. else skip
               	
               	if (AC == 0)
	       		{
	          		
		  			PC++;
	          		write(cpu_to_memory_pipe[1], &PC, 4);
	          		PC = fetch(memory_to_cpu_pipe[0], rfds, tv);
                  	end_check(PC,kernalmode);
	          		PC--;       
	       		}

	       		else 
	       		{
                	//we still need to skip the number followed by 21
                	PC++;  
	       		}

	       		break;

            case 22:
             	//if AC == 0, jump. else skip
               
	       		if (AC != 0)
	       		{
	          		PC++;
	          		write(cpu_to_memory_pipe[1], &PC, 4);
	          		PC = fetch(memory_to_cpu_pipe[0], rfds, tv);
                  	end_check(PC,kernalmode);
	          
		  			PC--;
	       		}
	       		else 
	       		{
	       		//we still need to skip the number followed by 22
	       		PC++; 
	       		}
               
	       		break;
           
            case 23:
               	//put PC on the stack and jump
	       		PC++;
	       
	       		PC = PC + 100000000 + (SP* 10000);

	       		write(cpu_to_memory_pipe[1], &PC, 4);
               
	       		PC = PC% 10000;
               
	       		write(cpu_to_memory_pipe[1], &PC, 4);
	       		PC = fetch(memory_to_cpu_pipe[0], rfds, tv);
               	end_check(PC,kernalmode);
	       
	       		PC--;      
               	SP--;
	       		break;

            case 24:
	       		//Pop and return to an address 
	       		SP++;
               
	       		//Get the conents out of the stack
	       		write(cpu_to_memory_pipe[1], &SP, 4);
	       		PC = fetch(memory_to_cpu_pipe[0], rfds, tv);
               	end_check(PC,kernalmode);
               
	       		break;

            case 25:
	       		X++;	    
	       		break;

            case 26:
               	X--;
	       		break;

	    	case 27:
               //push AC on the stack
               
	       		AC = AC + 100000000 + (SP* 10000);
	    
	       		write(cpu_to_memory_pipe[1], &AC, 4);
               	AC = AC % 10000;
	       		SP--;
               
	       		break;

	    	case 28:
               	//Pop AC off the stack
               
	       		SP++;
	       
	       		write(cpu_to_memory_pipe[1], &SP, 4);
	       		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
	     
	       		break;
            
            case 29:
               	//put PC on the stack and jump
	       		kernalmode = true;
               


	       		//set the regester values to be sent               
	       		Y  = Y  + 109990000;
	       		X  = X  + 109980000;
	       		AC = AC + 109970000;
	       		IR = IR + 109960000;
	       		SP = SP + 109950000;
	       		PC = PC + 109940000;
               
	       		write(cpu_to_memory_pipe[1], &Y,  4);
	       		write(cpu_to_memory_pipe[1], &X,  4);
	       		write(cpu_to_memory_pipe[1], &AC, 4);
	       		write(cpu_to_memory_pipe[1], &IR, 4);
	       		write(cpu_to_memory_pipe[1], &SP, 4);
	       		write(cpu_to_memory_pipe[1], &PC, 4);


               	//fetch the kernalmode regester values
	       		Y  = 1999;
	       		X  = 1998;
	       		AC = 1997;
	       		IR = 1996;
	       		SP = 1995;
	       		
	       		//PC is a constant with syscal and need not be fetched
	       		PC = 1500;

	       		write(cpu_to_memory_pipe[1], &Y, 4);
	       		Y = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       
	       		write(cpu_to_memory_pipe[1], &X, 4);
	       		X = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       
	       		write(cpu_to_memory_pipe[1], &AC, 4);
	       		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       
	       		write(cpu_to_memory_pipe[1], &IR, 4);
	       		IR = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       
	       		write(cpu_to_memory_pipe[1], &SP, 4);
	       		SP = fetch(memory_to_cpu_pipe[0], rfds, tv);
	      
	      		PC--; 
	      		//kernal mode is now engaged
	       		break;

           	case 30:
	      		//Pop and return to an address 
	       
              	// cout<<"does this hit?"<<endl;
	       		//set the regester values to be sent               
	       		Y  = Y  + 119990000;
	       		X  = X  + 119980000;
	       		AC = AC + 119970000;
	       		IR = IR + 119960000;
	       		SP = SP + 119950000;
	       		//we dont need to save PC since PC is defined by what called it.
               
	       		write(cpu_to_memory_pipe[1], &Y,  4);
	       		write(cpu_to_memory_pipe[1], &X,  4);
	       		write(cpu_to_memory_pipe[1], &AC, 4);
	       		write(cpu_to_memory_pipe[1], &IR, 4);
	       		write(cpu_to_memory_pipe[1], &SP, 4);


               //fetch the kernalmode regester values
	    
	       		Y  = 999;
	       		X  = 998;
	       		AC = 997;
	       		IR = 996;
	       		SP = 995;
	       		PC = 994;
	       	       

	       		write(cpu_to_memory_pipe[1], &Y, 4);
	       		Y = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       
	       		write(cpu_to_memory_pipe[1], &X, 4);
	       		X = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       
	       		write(cpu_to_memory_pipe[1], &AC, 4);
	       		AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       
	       		write(cpu_to_memory_pipe[1], &IR, 4);
	       		IR = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       	
	       		write(cpu_to_memory_pipe[1], &SP, 4);
	       		SP = fetch(memory_to_cpu_pipe[0], rfds, tv);

	       		write(cpu_to_memory_pipe[1], &PC, 4);
	       		PC = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       
               	kernalmode = false;
               //kernal mode is now disengaged
	       
           
	       break;
            	case 50:
	       		running = false;
	       		AC = 200000000;
	       		//this is the terminate signal to memory
	       		write(cpu_to_memory_pipe[1], &AC, 4);
	       		break;

            default:
	         	//there should not be a default, so I am ignoring this.
               	break;

         }//End of Switch

        PC++; //prep to get next instruction
      	 
        
        //this is the interupt timer
		if (!kernalmode)
		count_timer++;

        //And here is the timer interupt
	    //it is the same as when IR = 29, but with the PC set to 1000 instead of 1500
		if (count_timer == timer)
		{
	    	//reset timer;
	    	count_timer = 0;
	   
       		//enter kernal mode 
	       	kernalmode = true;
               
	       	//set the regester values to be sent               
	       	Y  = Y  + 109990000;
	       	X  = X  + 109980000;
	       	AC = AC + 109970000;
	       	IR = IR + 109960000;
	       	SP = SP + 109950000;
	       	//PC will get incremented when it hits the loop again, so it get decrimented here to reach the right code
	       	PC = PC + 109940000 - 1;
               
	       	write(cpu_to_memory_pipe[1], &Y,  4);
	       	write(cpu_to_memory_pipe[1], &X,  4);
	       	write(cpu_to_memory_pipe[1], &AC, 4);
	       	write(cpu_to_memory_pipe[1], &IR, 4);
	       	write(cpu_to_memory_pipe[1], &SP, 4);
	       	write(cpu_to_memory_pipe[1], &PC, 4);


            //fetch the kernalmode regester values
	       	Y  = 1999;
	       	X  = 1998;
	       	AC = 1997;
	       	IR = 1996;
	       	SP = 1995;
	       
	       	//PC is a constant with syscal and need not be fetched
	       	PC = 1000;

	       	write(cpu_to_memory_pipe[1], &Y, 4);
	       	Y = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       
	       	write(cpu_to_memory_pipe[1], &X, 4);
	       	X = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       
	       	write(cpu_to_memory_pipe[1], &AC, 4);
	       	AC = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       
	       	write(cpu_to_memory_pipe[1], &IR, 4);
	       	IR = fetch(memory_to_cpu_pipe[0], rfds, tv);
	       
	       	write(cpu_to_memory_pipe[1], &SP, 4);
	       	SP = fetch(memory_to_cpu_pipe[0], rfds, tv);	

            //kernal mode is now engaged
	   }//end of timer interupt
	
      }//end Running while loop
      
      
      return 0;
   }//End of CPU

 // -----------------   Memory   -------------------------------------
  
   else
   {

  		//this is the memory
      	//preleminary pipe closes
      	close(cpu_to_memory_pipe[1]);
      	close(memory_to_cpu_pipe[0]);
      
      	//innitialize with all elements zero
      	int memory [2000] = {0};

      	//parse
      	//open the file to load into memory
      	string inputfile;
      	inputfile= argv[1] ;
      	fstream myfilestream;
      	myfilestream.open(inputfile, ios::in);

      	string line;
      

      	if (myfilestream)
      	{
          
      		char firstchar; 
	 		int memory_position = 0;
        	while (!myfilestream.eof())
	 		{
	    		getline(myfilestream, line);
            
	    		//if the line is not empty, continue
	    		if (line.compare(""))
	    		{
	       			firstchar = line.at(0); 
	       			if (firstchar == '.') 
	       			{
	          			string to_use = line.substr(1, line.find(" "));
                  		memory_position = atoi(to_use.c_str() );
	       			}	
	       
               		//this is dreadful and ugly, but effective.
	       			//testing lead to this method becoming necessary
               		else if ((firstchar == '1')||(firstchar == '2')||(firstchar == '3')||(firstchar == '4')||(firstchar == '5')||(firstchar == '6')||(firstchar == '7')||(firstchar == '8')||(firstchar == '9')||(firstchar == '0'))
	       			{
                  		string to_store = line.substr(0, line.find(" "));   
		  				memory[memory_position] = atoi(to_store.c_str());
		  
		  				memory_position++;
	       			}    
            	}
	 		}//end of parse while
        
	 	//explicitly initializing kernam mode stack values
	 	memory[1995] = 1994;    
     
     	}//End of if (succesful_file_open)
   
    else
    {
    	cout << "FAILURE: file failed to open" <<endl;
       	return 0;
    }

      
    //set up select() like we did in Memory
      
    fd_set rfds;
    struct timeval tv;
     
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    
    FD_ZERO(&rfds);
    FD_SET(cpu_to_memory_pipe[0], &rfds);


    int buf_int;
      
    bool running = true;
    
    while(running)
	{
   		if (select(cpu_to_memory_pipe[0]+1, &rfds, NULL,NULL, &tv)) 
     	read(cpu_to_memory_pipe[0], &buf_int, 4);
 
      	else break;  // exit loop
      
         

      	if (buf_int<2000) 
        {
	  		write(memory_to_cpu_pipe[1], &memory[buf_int], 4);
        }
	 	else if ((buf_int /100000000) ==2)
	    {
	    
	    	running = false;
        }
	 	else if( buf_int<200000000)
	 	{
	    	buf_int = buf_int - 100000000;
	    
	    	//write to memory
        	//ints will e in the form 1xxxxyyyy
	    	//where 1 is a check command to store
	    	//where xxxx is a 4 digit number for address into
	    	//where yyyy is a 4 digit number for the data
	    
	    
	    	int temp_adr = buf_int/ 10000;

            memory[temp_adr] = buf_int%10000;
            
	    	//test the insertion
           	// cout << "Writing "<<memory[temp_adr]<<" into memory["<<temp_adr<<"]"<<endl;


	 	}//
         

     
     }//End of memory execution loop


        

   }// end of memory
   return 0;
}//end of memory
