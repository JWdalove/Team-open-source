#include "judgement_info.h"

#include "protocol.h" //协议文件

/* data send (forward) */
/* data receive */
receive_judge_t judge_rece_mesg;
u8 JudgeFrameCount=0;
float poked_speed_last;
void judgement_data_handle(uint8_t *p_frame)
{
    frame_header_t *p_header = (frame_header_t*)p_frame;
    memcpy(p_header, p_frame, HEADER_LEN);

    uint16_t data_length = p_header->data_length;
    uint16_t cmd_id      = *(uint16_t *)(p_frame + HEADER_LEN);
    uint8_t *data_addr   = p_frame + HEADER_LEN + CMD_LEN;
    JudgeFrameCount++;  //监视
    switch (cmd_id)
    {
    case GAME_STATE_DATA_ID:
        memcpy(&judge_rece_mesg.game_state, data_addr, data_length);
        break;
	
	  case ROBO_STATE_DATA_ID:
        memcpy(&judge_rece_mesg.game_information, data_addr, data_length);
        break;

    case HURT_STATE_DATA:
        memcpy(&judge_rece_mesg.blood_changed_data, data_addr, data_length);
        break;

    case REAL_SHOOT_DATA_ID:
    
        memcpy(&judge_rece_mesg.real_shoot_data, data_addr, data_length);
        break;
    
    case HEAT_POWER_DATA_ID:
        memcpy(&judge_rece_mesg.power_heat, data_addr, data_length);
        break;
	
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
	
	case STU_CUSTOM_DATA_ID:
		memcpy(&judge_rece_mesg.robot_communicate_id, data_addr, data_length);
		if(judge_rece_mesg.robot_communicate_id.data_cmd_id==0xd180)
			break;
		else
			memcpy(&judge_rece_mesg.robot_communicate, data_addr, data_length);
		break;
//	case STU_CUSTOM_DATA_ID:
//		memcpy(&judge_rece_mesg.robot_communicate, data_addr, data_length);
//		break;
    }

}
