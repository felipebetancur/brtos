
/*********************************************************************************************************
*                                               BRTOS
*                                Brazilian Real-Time Operating System
*                            Acronymous of Basic Real-Time Operating System
*
*                              
*                                  Open Source RTOS under MIT License
*
*
*
*                                 OS Event Handle Structs Declarations
*
*
*   Author: Gustavo Weber Denardin
*   Revision: 1.0
*   Date:     20/03/2009
*
*********************************************************************************************************/


#include "event.h"
#include "queue.h"

#include <stdlib.h>

INT16U iBRTOS_Sem   = 0;
INT16U iBRTOS_Mutex = 0;
INT16U iBRTOS_Mbox  = 0;
INT16U iBRTOS_Queue = 0;
INT16U iOS_Q = 0;





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Semaphore Control Block Declaration         /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#if (BRTOS_SEM_EN == 1)
  void             *OSSemFreeList;                      // Pointer to list of free Semaphore control blocks
  BRTOS_Sem        BRTOS_Sem_Table[BRTOS_MAX_SEM];      // Table of EVENT control blocks
#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Mutex Control Block Declaration             /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#if (BRTOS_MUTEX_EN == 1)
  void             *OSMutexFreeList;                      // Pointer to list of free Mutex control blocks
  BRTOS_Mutex      BRTOS_Mutex_Table[BRTOS_MAX_MUTEX];    // Table of EVENT control blocks
#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Mbox Control Block Declaration              /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#if (BRTOS_MBOX_EN == 1)
  void             *OSMboxFreeList;                      // Pointer to list of free Mbox control blocks
  BRTOS_Mbox       BRTOS_Mbox_Table[BRTOS_MAX_MBOX];     // Table of EVENT control blocks
#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Queue Control Block Declaration             /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

#if (BRTOS_QUEUE_EN == 1)
  void             *OSQueueFreeList;                      // Pointer to list of free Queue control blocks
  BRTOS_Queue      BRTOS_Queue_Table[BRTOS_MAX_QUEUE];    // Table of EVENT control blocks
#endif

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
/////      Initialize Block List Control               /////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

void initEvents(void)
{
  // Posiciona a lista de blocos de controle de eventos livres no inicio
  #if (BRTOS_SEM_EN == 1)
  OSSemFreeList   = &BRTOS_Sem_Table[0];
  #endif
  
  #if (BRTOS_MUTEX_EN == 1)
  OSMutexFreeList = &BRTOS_Mutex_Table[0];
  #endif
    
  #if (BRTOS_MBOX_EN == 1)
  OSMboxFreeList  = &BRTOS_Mbox_Table[0];
  #endif
  
  #if (BRTOS_QUEUE_EN == 1)
  OSQueueFreeList = &BRTOS_Queue_Table[0];
  #endif
}

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////





////////////////////////////////////////////////////////////////////////////////////
//                                                                                //
// Uma vari�vel global deve ser criada para controlar a entrada                   //
// em multiplas interrup��es. A troca de contexto s� deve ocorrer                 //
// quando est� vari�vel for igual a zero, ou seja, quando todas as                //
// interrup��es foram tratadas                                                    //
//                                                                                //
// Devem ser criadas fun��es para retirar e colocar dados no buffer circular      //
// Devem ser criadas fun��es                                                      //
//                                                                                //
// OBS:
// Uma interrup��o s� poder� gerar altera��o de contexto se a tarefa que acordou  //
// com o evento for de maior prioridade que a tarefa interrompida                 //
//                                                                                //
////////////////////////////////////////////////////////////////////////////////////