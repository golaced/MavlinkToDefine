
#include "LoopQueue.h"

int initCharLoopQueue( LoopQueue* loopQueue, unsigned char* pc ,unsigned int len, unsigned char uc )	//init the queue, need to specify the length of array. all Element value is 0
{
	int i = 0;
	
	if( loopQueue==0 )
	{
		return 0;
	}

	loopQueue->data = pc;
	loopQueue->front = loopQueue->rear = 0;
	loopQueue->length = len;
	loopQueue->realLen = 0;
	for( ; i<loopQueue->length; ++i )
	{
		loopQueue->data[i] = uc;	
	}
	return 1;	
}

int destroyLoopQueue( LoopQueue* loopQueue )			    //destroy the queue, the point=NULL, all element value is 0
{
	loopQueue->data = 0;
	if( loopQueue->data==0 )
	{
		return 1;
	}
	return 0;
}	
							
int loopQueueLength( LoopQueue* loopQueue )		//get the length of queue
{
	if(loopQueue==0) 
	{
		return -1;	
	}	
	return loopQueue->realLen;
}

int loopQueueIsEmpty( LoopQueue* loopQueue )                //Determine whether the queue is empty.yes,return 1; otherwise return 0
{
	if( loopQueue==0 )
	{
		return -1;
	}
	
	if( loopQueue->rear == loopQueue->front && loopQueue->realLen==0 )
	{
		return 1;
	}
	return 0;
}

int insertCharLoopQueue( LoopQueue* loopQueue, unsigned char value )					//insert element to queue
{
	if( loopQueue==0 )
	{
		return 0;
	}
	
	loopQueue->data[loopQueue->rear] = value;
	loopQueue->rear = (loopQueue->rear + 1) % loopQueue->length;
	loopQueue->realLen++;
	return 1;
	
}

unsigned char readCharLoopQueue( LoopQueue* loopQueue )					//read element in queue
{
	unsigned char uc = 0;
		
	if( loopQueue==0 )
	{
		return 0;
	}
	
	uc = loopQueue->data[loopQueue->front];
	loopQueue->front = (loopQueue->front+1) % loopQueue->length;
	loopQueue->realLen--;
	return uc;	
	
}

