#include "Task_CANSend.h"


void Task_CANSend(void *Parameters)
{
    CanSend_Type    CANSendData;
    for(;;)
    {
        xQueueReceive(Queue_CANSend, &CANSendData, portMAX_DELAY);

        if(CANSendData.CANx == 1)
        {
            CAN_Transmit(CAN1, &(CANSendData.TxMessage));
        }
        else
        {
            CAN_Transmit(CAN2, &(CANSendData.TxMessage));
        }
    }
}
