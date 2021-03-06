/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"

#define ARRIVAL_TIME_A (core_array[i]->current_job->time)
#define ARRIVAL_TIME_B (core_array[flagged]->current_job->time)
#define REMAINING_TIME_A (core_array[i]->current_job->running_time-(time-core_array[i]->current_job->placed_on_core))
#define REMAINING_TIME_B (core_array[flagged]->current_job->running_time-(time-core_array[flagged]->current_job->placed_on_core))
#define PRIORITY_A (core_array[i]->current_job->priority)
#define PRIORITY_B (core_array[flagged]->current_job->priority)

/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements. 
*/

int total_job_waiting_time = 0;
int total_turnaround_time = 0;
int total_response_time = 0;
int jobs_finished = 0;
int num_cores;
priqueue_t job_queue;
core_t **core_array;
scheme_t pri_scheme;

int fifo_compare(const void * a, const void * b)
{
    return (1);
}

int sjf_compare(const void * a, const void * b)
{
    const job_t * job1 = a;
	  const job_t * job2 = b;
    
    if((job1->running_time - job2->running_time) == 0)
    {
        if(job1->time > job2->time)
        {
            return (1);
        }
        
    }
	  return ( job1->running_time - job2->running_time );
}

int pri_compare(const void * a, const void * b)
{
    const job_t * job1 = a;
	  const job_t * job2 = b;
    
    if((job1->priority - job2->priority) == 0)
    {
        if(job1->time > job2->time)
        {
            return (1);
        }
        
    }
	  return ( job1->priority - job2->priority );
}

/**
  Initalizes the scheduler.
 
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
    num_cores = cores;
    pri_scheme = scheme;
    switch(pri_scheme)
    {
        case FCFS:
            priqueue_init(&job_queue,fifo_compare);
            break;
        case SJF:
            priqueue_init(&job_queue,sjf_compare);
            break;
        case PSJF:
            priqueue_init(&job_queue,sjf_compare);
            break;
        case PRI:
            priqueue_init(&job_queue,pri_compare);
            break;
        case PPRI:
            priqueue_init(&job_queue,pri_compare);
            break;
        case RR:
            priqueue_init(&job_queue,fifo_compare);
            break;                      
        default:
            printf("Invalid value for scheme!!! Received: %d\n",pri_scheme );
            break;                          
    }

    int i = 0;

    core_array = calloc(num_cores, sizeof(core_t*));
    while(i<num_cores)
    {
        struct core_t *tmp = (struct core_t*)malloc(sizeof(struct core_t));
        tmp->current_job = NULL;
        core_array[i]=tmp;
        i++;
    }
}



/**
  Called when a new job arrives.
 
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. 
 
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
    
    struct job_t *tmp = (struct job_t*)malloc(sizeof(struct job_t));
    tmp->time = time;
    tmp->job_number = job_number;
    tmp->running_time = running_time;
    tmp->priority = priority;   
    tmp->previously_scheduled = false;
    tmp->time_placed_in_queue = time;
    tmp->placed_on_core = -1; 
    
    int i = 0;
    int flagged = 0;
    switch(pri_scheme)
    {
        case FCFS:
            printf("FCFS schedule\n");
            while(i<num_cores)
            {
                if(core_array[i]->current_job == NULL)
                {
                    tmp->placed_on_core = time;
                    if(!tmp->previously_scheduled){
                        tmp->first_placed_on_core = time;
                    }
                    tmp->previously_scheduled = true;
                    core_array[i]->current_job = tmp;
                    printf("Assigned job %d to core number %d\n",job_number,i);
                    return i;
                }
            i++;
            }
            priqueue_offer(&job_queue,tmp);
            break;
        case SJF:
            printf("SJF schedule\n");
            while(i<num_cores)
            {
                //I think jobs with same running_time are being ordered incorrectly in the queue
                if(core_array[i]->current_job == NULL)
                {
                    tmp->placed_on_core = time;
                    if(!tmp->previously_scheduled){
                        tmp->first_placed_on_core = time;
                    }                    
                    tmp->previously_scheduled = true;
                    core_array[i]->current_job = tmp;
                    printf("Assigned job %d to core number %d\n",job_number,i);
                    return i;
                }
            i++;
            }
            
            priqueue_offer(&job_queue,tmp);
            break;
        case PSJF:
            printf("PSJF schedule\n");

            ///Check for idle cores
            while(i<num_cores)
            {
                if(core_array[i]->current_job == NULL)
                {
                    tmp->placed_on_core = time;
                    if(!tmp->previously_scheduled){
                        tmp->first_placed_on_core = time;
                    }                                        
                    tmp->previously_scheduled = true;
                    core_array[i]->current_job = tmp;
                    printf("Assigned job %d to core number %d\n",job_number,i);
                    return i;
                }
            i++;
            }
            
            ///find core with job with most time remaining.
            i=1;
            flagged = 0;
            while(i<num_cores)
            {
                if((REMAINING_TIME_A) == (REMAINING_TIME_B))
                {
                    if(ARRIVAL_TIME_A > ARRIVAL_TIME_B)
                    {
                        flagged = i;
                    }
                }
                else if ((REMAINING_TIME_A) > (REMAINING_TIME_B))
                {
                    flagged = i;
                }
                
            i++;
            }
            
            printf("The longest job is on core %d\n",flagged);
            
            ///figure out if longest job remaining is longer than the job that jsut arrived.
            printf("Job %d runtime %d is less than Job %d runtime %d\n",core_array[flagged]->current_job->job_number,REMAINING_TIME_B,tmp->job_number,tmp->running_time);
            if((REMAINING_TIME_B) > (tmp->running_time))
            {
                printf("placed job %d into the queue. Placed job %d onto core %d!\n",core_array[flagged]->current_job->job_number,tmp->job_number,flagged);
                core_array[flagged]->current_job->running_time = REMAINING_TIME_B;
                core_array[flagged]->current_job->time_placed_in_queue = time;
                if(core_array[flagged]->current_job->first_placed_on_core == time)
                {
                    core_array[flagged]->current_job->previously_scheduled = false;
                }
                priqueue_offer(&job_queue,core_array[flagged]->current_job);
                tmp->placed_on_core = time;
                if(!tmp->previously_scheduled){
                    tmp->first_placed_on_core = time;
                }                                    
                tmp->previously_scheduled = true;
                core_array[flagged]->current_job = tmp;
                return flagged;
            }
            else
            {
                printf("Placed the new job, job: %d, into the queue.",tmp->job_number);
                priqueue_offer(&job_queue,tmp);
                return -1;    
            }
            break;
        case PRI:
            printf("PRI schedule\n");
            while(i<num_cores)
            {
                if(core_array[i]->current_job == NULL)
                {
                    tmp->placed_on_core = time;
                    if(!tmp->previously_scheduled){
                        tmp->first_placed_on_core = time;
                    }                                        
                    tmp->previously_scheduled = true;
                    core_array[i]->current_job = tmp;
                    printf("Assigned job %d to core number %d\n",job_number,i);
                    return i;
                }
            i++;
            }
            
            priqueue_offer(&job_queue,tmp);
            break;
        case PPRI:
            printf("PPRI schedule\n");
            //Check for idle cores
            while(i<num_cores)
            {
                if(core_array[i]->current_job == NULL)
                {
                    tmp->placed_on_core = time;
                    if(!tmp->previously_scheduled){
                        tmp->first_placed_on_core = time;
                    }                                        
                    tmp->previously_scheduled = true;
                    core_array[i]->current_job = tmp;
                    printf("Assigned job %d to core number %d\n",job_number,i);
                    return i;
                }
            i++;
            }
            
            ///find core with job with most time remaining.
            i=1;
            flagged = 0;
            while(i<num_cores)
            {
                if((PRIORITY_A) == (PRIORITY_B))
                {
                    if(ARRIVAL_TIME_A > ARRIVAL_TIME_B)
                    {
                        flagged = i;
                    }
                }
                else if ((PRIORITY_A) > (PRIORITY_B))
                {
                    flagged = i;
                }
                
            i++;
            }
            
            printf("The priority closes to 0 job is on core %d\n",flagged);
            
            ///figure out if longest job remaining is longer than the job that jsut arrived.
            if((PRIORITY_B) > (tmp->priority))
            {
                printf("placed job %d into the queue. Placed job %d onto core %d!\n",core_array[flagged]->current_job->job_number,tmp->job_number,flagged);
                core_array[flagged]->current_job->running_time = REMAINING_TIME_B;
                core_array[flagged]->current_job->time_placed_in_queue = time;
                if(core_array[flagged]->current_job->first_placed_on_core == time)
                {
                    core_array[flagged]->current_job->previously_scheduled = false;
                }
                priqueue_offer(&job_queue,core_array[flagged]->current_job);
                tmp->placed_on_core = time;
                if(!tmp->previously_scheduled){
                    tmp->first_placed_on_core = time;
                }                                    
                tmp->previously_scheduled = true;
                core_array[flagged]->current_job = tmp;
                return flagged;
            }
            else
            {
                printf("Placed the new job, job: %d, into the queue.",tmp->job_number);
                priqueue_offer(&job_queue,tmp);
                return -1;    
            }
            break;       
        case RR:
            printf("RR schedule\n");
            while(i<num_cores)
            {
                if(core_array[i]->current_job == NULL)
                {
                    tmp->placed_on_core = time;
                    if(!tmp->previously_scheduled){
                        tmp->first_placed_on_core = time;
                    }                                        
                    tmp->previously_scheduled = true;
                    core_array[i]->current_job = tmp;
                    printf("Assigned job %d to core number %d\n",job_number,i);
                    return i;
                }
            i++;
            }
            priqueue_offer(&job_queue,tmp);
            break;                                         
        default:
            printf("Invalid value for scheme!!! Received: %d\n",pri_scheme );
            break;
    }
	  return -1;
}


/**
  Called when a job has completed execution.
 
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
 
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
    jobs_finished++;
    total_turnaround_time+=(time-core_array[core_id]->current_job->time);
    free(core_array[core_id]->current_job);
    struct job_t* queued_job = priqueue_poll(&job_queue);
    
    core_array[core_id]->current_job = queued_job;
    
    if(core_array[core_id]->current_job != NULL)
    {
    core_array[core_id]->current_job->placed_on_core = time;
        total_job_waiting_time+=(time-core_array[core_id]->current_job->time_placed_in_queue);
        if(!core_array[core_id]->current_job->previously_scheduled)
        {
            total_response_time+=(time-core_array[core_id]->current_job->time_placed_in_queue);
            if(!core_array[core_id]->current_job->previously_scheduled){
                core_array[core_id]->current_job->first_placed_on_core = time;
            }                                
            core_array[core_id]->current_job->previously_scheduled = true;
        }
        printf("Job was waiting in queue for %d time cycles!",time-core_array[core_id]->current_job->time);
        return core_array[core_id]->current_job->job_number;
    }
	  return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
 
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator. 
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
    printf("Quatum expired!\n");
    
    /////maybe call the job_finished function if the quantum happens when the job is scheduled to finish anyways.
    core_array[core_id]->current_job->time_placed_in_queue = time;
    priqueue_offer(&job_queue,core_array[core_id]->current_job);
    
    struct job_t* queued_job = priqueue_poll(&job_queue);
    core_array[core_id]->current_job = queued_job;
    total_job_waiting_time+=(time-core_array[core_id]->current_job->time_placed_in_queue);
    
    if(!core_array[core_id]->current_job->previously_scheduled)
    {
        total_response_time+=(time-core_array[core_id]->current_job->time_placed_in_queue);
        core_array[core_id]->current_job->previously_scheduled = true;
    }            
    
    return core_array[core_id]->current_job->job_number;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

    Waiting Time: Time a job spends waiting in the queue. 

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	  return ((float)total_job_waiting_time/(float)jobs_finished);
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

    Turnaround Time: Time to complete a task (ready -> complete)

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	  return ((float)total_turnaround_time/(float)jobs_finished);
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

    Response Time: Time to schedule a task (ready -> first scheduled)

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	  return ((float)total_response_time/(float)jobs_finished);
}


/**
  Free any memory associated with your scheduler.
 
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
    priqueue_destroy(&job_queue);
    int i=0;
    while(i<num_cores)
    {
        free(core_array[i]);
        i++;
    }
    free(core_array);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:
*/
void scheduler_show_queue()
{
    int i = 0;
    while( priqueue_at( &job_queue, i ) != NULL )
    {
        struct job_t* data = priqueue_at( &job_queue, i );   
        printf(" [Jn:%d, rt:%d, pri:%d] ->", data->job_number,data->running_time,data->priority );
        i++;
    }
    printf(" [NULL]\n"); 
}