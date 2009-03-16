#include "types.h"
#include "event.h"
#include "myRTOS.h"
#include <hidef.h> /* for EnableInterrupts macro */



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Create Semaphore Function                   /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSSemCreate (INT16U cnt, OS_EVENT **event)
{
  int i=0;

  OS_EVENT *pont_event;

  if (iNesting > 0) {                                /* See if called from ISR ...               */
      return(IRQ_PEND_ERR);                          /* ... can't CREATE from an ISR             */
  }
    
  // Enter critical
  // Verifica se ainda h� blocos de controle de eventos dispon�veis
  if (iOS_Event < OS_MAX_EVENTS)
  {
      iOS_Event++;
      pont_event = OSEventFreeList;
      
      // Posiciona a lista de blocos de controle de eventos livre no pr�ximo bloco dispon�vel
      OSEventFreeList = &OSEventTbl[iOS_Event];
  } else
  {
    // Caso n�o haja mais blocos dispon�veis, retorna exce��o
    return(NO_AVAILABLE_EVENT);
  }    
    

    // Exit Critical
  pont_event->OSEventType    = OS_EVENT_TYPE_SEM;
  pont_event->OSEventCnt     = cnt;                      /* Set semaphore value                      */
  pont_event->OSEventPtr     = (void *)0;                /* Unlink from ECB free list                */
  pont_event->OSEventGrp = 0;
  
  for(i=0;i<OS_EVENT_TABLE_SIZE;i++)
  {
    pont_event->OSEventTbl[i]=0;  
  }
  
  *event = pont_event;
  
  return(ALLOC_EVENT_OK);
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Semaphore Pend Function                     /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSSemPend (OS_EVENT *pont_event, INT16U timeout)
{
  INT16S i = 0;
  INT16U time_wait = 0;
  OSEnterCritical;
  
  if (pont_event->OSEventType != OS_EVENT_TYPE_SEM)
  {
    OSExitCritical;
    return ERR_EVENT_TYPE;
  }
 
  
  if (pont_event->OSEventCnt > 0)
  {
    pont_event->OSEventCnt--;
    OSExitCritical;
    return OK;
  }
 
  
  if(iNesting > 0)
  {
    return(IRQ_PEND_ERR);
  }
  
  ContextTask[currentTask].Suspended = TRUE;
  ContextTask[currentTask].SuspendedType = SEMAPHORE;

  
  // Verificar o timeout em rela��o ou counter overflow
  // Verificar o timeout em rela��o ou counter overflow
  if (timeout)
  {  
    time_wait = counter + timeout;
    if (time_wait >= TickCountOverFlow)
      time_wait = timeout - (TickCountOverFlow - counter);
    ContextTask[currentTask].TimeToWait = time_wait;
  } else
    ContextTask[currentTask].TimeToWait = 65000;
  
  
  
  // Aloca a tarefa com semaforo pendente na lista do evento
  for(i=0;i<OS_EVENT_TABLE_SIZE;i++)
  {
    if(pont_event->OSEventTbl[i] == currentTask)
    {
      break;
    }
      
    if(pont_event->OSEventTbl[i] == 0)
    {
      pont_event->OSEventTbl[i] = currentTask;
      pont_event->OSEventGrp++;
      break;
    }
  }
  
  // Troca contexto e s� retorna quando estourar Delay ou quando ocorrer um Post do Semaforo
  ChangeContext();
  
}


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Semaphore Post Function                     /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

INT8U OSSemPost(OS_EVENT *pont_event)
{
  INT16U i=0;
  INT16U j=0;
  INT8U Max_Priority = 0;
  
  if (pont_event->OSEventType != OS_EVENT_TYPE_SEM)
  {   /* Validate event block type                     */
    return ERR_EVENT_TYPE;;
  }
  
  if (iNesting == 0)
    OSEnterCritical;
  if (pont_event->OSEventGrp != 0)                    /* See if any task waiting for semaphore         */
  {
    for(i=0;i<OS_EVENT_TABLE_SIZE;i++)
    {
	    if (pont_event->OSEventTbl[i] != 0)
	    {
	      if(ContextTask[pont_event->OSEventTbl[i]].Priority >= Max_Priority)
	      {
	        // Descobre a tarefa de maior prioridade esperando o recurso
	        Max_Priority = ContextTask[pont_event->OSEventTbl[i]].Priority;
	        j = i;
	      }
      }
    }
    // Coloca a tarefa de maior prioridade na Ready List
    ContextTask[pont_event->OSEventTbl[j]].Suspended = READY;     // Coloca a tarefa na Ready List
    // Retira esta tarefa das tarefas que esperam por evento / recurso
    pont_event->OSEventTbl[j] = 0;
    // Decrementa o n�mero de tarefas esperando pelo evento / recurso
    pont_event->OSEventGrp--;
    // Exit Critical
    if (iNesting == 0)
      OSExitCritical;
    return OK;
  }
    
  if (pont_event->OSEventCnt < 65535u)          /* Make sure semaphore will not overflow         */
  {            
    pont_event->OSEventCnt++;                     /* Increment semaphore count to register event   */
    // Exit Critical
    if (iNesting == 0)
      OSExitCritical;
    return OK;
  }
  // Exit Critical                           /* Semaphore value has reached its maximum       */
  if (iNesting == 0)
    OSExitCritical;
  return ERR_SEM_OVF;
}
