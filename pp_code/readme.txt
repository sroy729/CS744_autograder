Folder Structure:

22d0378-22d0376-rollnumber2/
	|
	----readme.txt
	|
  	----loadtesting/
  		|
		----ver1/
    		|
    		----ver2/
	    	|
	    	----ver3/
	|
	----code/
		|
		----ver1/
    		|
    		----ver2/
	    	|
	    	----ver3/
	    	|
		----ver4/
			|
			----ver4_with_mysql/
			|
			----ver4_with_csv/
			
Each ver2/3/4 folders contain separate directories for submissions and errors:

|
----compiler_error
	|
	----compilerError_*.txt
|
----runtime_error
	|
	----runtimeError_*.txt
|
----submissions
	|
	----program_*.txt
|
----outputs
	|
	----output_*.txt
|
----executables
	|
	----executable_*.txt


How to run ver1 server?
1) cd to ver1/ folder
2) On server machine: run ./server_script.sh <port number>
3) On client machine: run ./client_script.sh <port> <num of clients> <num of iterations> <sleep time>

How to run ver2 server?
1) cd to ver2/ folder
2) On server machine: run ./server_script.sh <port number>.
3) On client machine: run ./client_script.sh <port> <num_clients> <num_iterations> <sleep_time> <timeout seconds>

How to run ver3 server?
1) cd to ver3/ folder
2) On server machine: run ./server_script.sh <port number> <number of threads>
3) On client machine: run ./client_script.sh <port number> <number of clients> <number of iterations> <sleep time> <timeout seconds>
	
How to run ver4 server?
1) cd to ver4/ folder
2) On server machine: run ./server_script.sh <port number> <number of threads>
3) On client machine: run ./client_script.sh <port number> <number of clients> <number of iterations> <sleep time> <timeout seconds>	

Loadtesting:
1) On client side: client generates response time, throughput, timeout, request rate files for different number of clients. Run ./final_script.sh to generate graph for each of these metrics.
2) On server side: 
	a) ./server_script.sh starts vmstat command to capture cpu utilization values using "vmstat -n 5" command 		and store vmstat pid in vmstat_pid.txt to kill pid after completing loadtesting for each client.
	b) Run ./nlwp_script.sh to start capturing number of lieght weight processes using "ps -eLF|grep
	multiThreadServer|wc" command.
	b) Atlast run ./vmstat_kill.sh to kill the vmstat process and generate avgCpuUtilzation and nlwp files.

