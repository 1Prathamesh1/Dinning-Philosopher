#include"header.h"
#define NUM_PHIL 5
#define NUM_CYCLES 5

int semid;

void sem_wait(int);
void sem_signal(int);
void philoshoper(int);

int main(){
	int val=1,status;
	int p[5]={0};
	//Create semaphore array
	semid=semget(4,NUM_PHIL,IPC_CREAT|0664);
	if(semid==-1){
		perror("semget");
		return 1;
	}	
	
	//Set each semaphore to 1
	for(int i=0;i<NUM_PHIL;i++){
		if(semctl(semid,i,SETVAL,val)==-1){
			perror("semctl");
			return 1;
		}
	}
	printf("Dining philosopher started with %d philosophers\n",NUM_PHIL);
	

	//Create 5 child process(philoshoper)
	
	for(int i=0;i<NUM_PHIL;i++){
		p[i]=fork();
		if(p[i]==-1){
			perror("fork failed");
			exit(1);
		}
		
		if(p[i]==0)
			philoshoper(i);
	}

	//Wait for all childrens to finish
	for(int i=0;i<NUM_PHIL;i++){
		wait(NULL);
	}
	printf("All philosophers have finished\n");
	if(semctl(semid,0,IPC_RMID)==-1){
		perror("semctl");
		return 1;
	}
	return 0;
}

void sem_wait(int n){
	struct sembuf v;
	v.sem_num=n;
	v.sem_op=-1;//Decrement the semaphore value to -1
	v.sem_flg=0;
	
	if(semop(semid,&v,1)==-1){
		perror("semop");
		exit(1);
	}
}

void sem_signal(int n){
	struct sembuf v;
	v.sem_num=n;
	v.sem_op=1;//Increment the value of semaphore to 1
	v.sem_flg=0;
	
	if(semop(semid,&v,1)==-1){
		perror("semop");
		exit(1);
	}
}

void philoshoper(int id){
	int left_fork,right_fork;
	left_fork=id;
	right_fork=(id+1)%NUM_PHIL;
	
	for(int i=0;i<NUM_CYCLES;i++){
		printf("Philosopher %d is thinking\n",id+1);
		sleep(2);
	
		printf("Philosopher %d is hungry\n",id+1);
		if(id%2==0){
			sem_wait(left_fork);
			printf("Philosopher %d picked up left fork %d\n",id+1,left_fork);
		
			sem_wait(right_fork);	
			printf("Philosopher %d picked up right fork %d\n",id+1,right_fork);
		}
		else{
			sem_wait(right_fork);	
			printf("Philosopher %d picked up right fork %d\n",id+1,right_fork);	
			sem_wait(left_fork);
			printf("Philosopher %d picked up left fork %d\n",id+1,left_fork);
		}
		
		printf("Philosopher %d is eating...\n",id+1);
		sleep(2);
		
		//Release both forks
		sem_signal(left_fork);
		sem_signal(right_fork);
		printf("Philosopher %d put down all forks\n",id+1);
	}
	printf("Philosopher %d has finished eating %d times and is leaving..\n",id+1,NUM_CYCLES);
	exit(0);
}
