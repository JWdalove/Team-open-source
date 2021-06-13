#include "judgement_info.h"
#include "uart_judge.h"
#include "protocol.h" //协议文件

/* data send (forward) */
/* data receive */
extern uint16_t  heat_max;
uint16_t heat_last;
receive_judge_t judge_rece_mesg;
int Poked_n;
float poked_speed_last;
int Preview_n;                    //可发射弹丸数
u8 shoot_cnt;

void judgement_data_handle(uint8_t *p_frame)
{
  frame_header_t *p_header = (frame_header_t*)p_frame;
  memcpy(p_header, p_frame, HEADER_LEN);
  
  uint16_t data_length = p_header->data_length;
  uint16_t cmd_id      = *(uint16_t *)(p_frame + HEADER_LEN);
  uint8_t *data_addr   = p_frame + HEADER_LEN + CMD_LEN;
	
  switch (cmd_id)
  {
    case ROBO_STATE_DATA_ID:
      memcpy(&judge_rece_mesg.game_information, data_addr, data_length);
    break;

    case REAL_BLOOD_DATA_ID:
      memcpy(&judge_rece_mesg.blood_changed_data, data_addr, data_length);
    break;

    case REAL_SHOOT_DATA_ID:
		{
      poked_speed_last = judge_rece_mesg.real_shoot_data.bullet_speed;//上次速度
		  memcpy(&judge_rece_mesg.real_shoot_data, data_addr, data_length);

			if(judge_rece_mesg.real_shoot_data.bullet_type==1 && judge_rece_mesg.real_shoot_data.bullet_speed - poked_speed_last)
			{

				Poked_n++;       //剩余弹丸数目

				Preview_n--;     //当前可发弹数目
				Preview_n = Preview_n <= 0 ? 0 : Preview_n;
			}	

			break;
		}   
		case HEAT_POWER_DATA_ID:
		{
			memcpy(&judge_rece_mesg.power_heat, data_addr, data_length);
			{
//			  if(judge_rece_mesg.power_heat.shooterheat0 != heat_last)
//				{
//				  heat_preview = judge_rece_mesg.power_heat.shooterheat0;
//					heat_last = judge_rece_mesg.power_heat.shooterheat0;
//				}
				
			break;
			}
			

		}
		case REAL_FIELD_DATA_ID:
			memcpy(&judge_rece_mesg.rfid_data, data_addr, data_length);	
		break;
		case GAME_RESULT_ID:
			memcpy(&judge_rece_mesg.game_result_data, data_addr, data_length);	
		break;
		case GAIN_BUFF_ID:
			memcpy(&judge_rece_mesg.get_buff_data, data_addr, data_length);		
		break;
		case GAME_POSITION_ID:
			memcpy(&judge_rece_mesg.game_robo_pos, data_addr, data_length);		
		break;

  }
}
