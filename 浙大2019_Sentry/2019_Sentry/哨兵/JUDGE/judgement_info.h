/****************************************************************************
 *  Copyright (C) 2018 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of?
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.? See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/
/** @file judgement_info.h
 *  @version 1.0
 *  @date June 2017
 *
 *  @brief the information from judgement system
 *
 *  @copyright 2017 DJI RoboMaster. All rights reserved.
 *
 */

#ifndef __JUDGEMENT_INFO_H__
#define __JUDGEMENT_INFO_H__

#include "struct.h"
#include "constant.h"
#define JUDGE_FRAME_BUFLEN 200
extern u8 JudgeFrameCount;
/**
  * @brief  judgement data command id
  */
typedef enum
{
	GAME_STATE_DATA_ID = 0x0001,  //比赛状态数据， 1Hz 周期发送
	GAME_RESULT_ID     = 0x0002, //比赛结果数据，比赛结束后发送 [6]
	ROBO_survive_DATA	 = 0x0003, //比赛机器人存活数据， 1Hz 周期发送
	
	REAL_FIELD_DATA_ID = 0x0101, //场地事件数据，事件改变后发送[5]
	FIELD_depot_DATA	 = 0x0102, //--场地补给站动作标识数据，动作改变后发送
	depot_Fill_DATA		 = 0x0103, //--请求补给站补弹数据，由参赛队发送，上限 10Hz。（RM 对抗赛尚未开放）
	
	ROBO_STATE_DATA_ID = 0x0201, //--机器人状态数据， 10Hz 周期发送 [1]
	HEAT_POWER_DATA_ID = 0x0202, //--实时功率热量数据， 50Hz 周期发送 [4]
	GAME_POSITION_ID	 = 0x0203, //机器人位置数据， 10Hz 发送 [8]
	GAIN_BUFF_ID    	 = 0x0204, //机器人增益数据， 增益状态改变后发送 [7]

	HURT_STATE_DATA		 = 0x0206, //伤害状态数据，伤害发生后发送
	REAL_SHOOT_DATA_ID = 0x0207, //--实时射击数据，子弹发射后发送		[3]						 
	STU_CUSTOM_DATA_ID = 0x0301, //--机器人间交互数据，发送方触发发送，上限 10Hz [100]


} judge_data_id_e;


//------------------------------------------------------------------------------------
//2019
//二、接口协议说明
//1.比赛状态数据： 0x0001。 发送频率： 1Hz
typedef __packed struct
{
uint8_t game_type : 4;
uint8_t game_progress : 4;
uint16_t stage_remain_time;
} ext_game_state_t;

//2. 比赛结果数据： 0x0002。 发送频率：比赛结束后发送
typedef __packed struct
{
uint8_t winner;
} ext_game_result_t;

//3. 机器人存活数据： 0x0003。 发送频率： 1Hz
typedef __packed struct
{
uint16_t robot_legion;
} ext_game_robot_survivors_t;

//4. 场地事件数据： 0x0101。 发送频率：事件改变后发送
typedef __packed struct
{
uint32_t event_type;
} ext_event_data_t;

//5. 补给站动作标识： 0x0102。 发送频率：动作改变后发送
typedef __packed struct
{
uint8_t supply_projectile_id;
uint8_t supply_robot_id;
uint8_t supply_projectile_step;
uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

//6. 请求补给站补弹子弹： cmd_id (0x0103)。发送频率：上限 10Hz。 RM 对抗赛尚未开放
typedef __packed struct
{
uint8_t supply_projectile_id;
uint8_t supply_robot_id;
uint8_t supply_num;
} ext_supply_projectile_booking_t;

//7. 比赛机器人状态： 0x0201。 发送频率： 10Hz
typedef __packed struct
{
uint8_t robot_id;
uint8_t robot_level;
uint16_t remain_HP;
uint16_t max_HP;
uint16_t shooter_heat0_cooling_rate;
uint16_t shooter_heat0_cooling_limit;
uint16_t shooter_heat1_cooling_rate;
uint16_t shooter_heat1_cooling_limit;
uint8_t mains_power_gimbal_output : 1;
uint8_t mains_power_chassis_output : 1;
uint8_t mains_power_shooter_output : 1;
} ext_game_robot_state_t;

//8. 实时功率热量数据： 0x0202。 发送频率： 50Hz
typedef __packed struct
{
uint16_t chassis_volt;
uint16_t chassis_current;
float chassis_power;			//底盘输出功率
uint16_t chassis_power_buffer;	//底盘缓冲能量
uint16_t shooter_heat0;			//17mm热量
uint16_t shooter_heat1;			//42mm热量
} ext_power_heat_data_t;

//9. 机器人位置： 0x0203。 发送频率： 10Hz
typedef __packed struct
{
float x;
float y;
float z;
float yaw;
} ext_game_robot_pos_t;

//10. 机器人增益： 0x0204。 发送频率：状态改变后发送
typedef __packed struct
{
uint8_t power_rune_buff;
}ext_buff_musk_t;

//11. 空中机器人能量状态： 0x0205。 发送频率： 10Hz
typedef __packed struct
{
uint8_t energy_point;
uint8_t attack_time;
} aerial_robot_energy_t;

//12. 伤害状态： 0x0206。 发送频率：伤害发生后发送
typedef __packed struct
{
uint8_t armor_id : 4;
uint8_t hurt_type : 4;
} ext_robot_hurt_t;

//13. 实时射击信息： 0x0207。 发送频率：射击后发送
typedef __packed struct
{
uint8_t bullet_type;
uint8_t bullet_freq;
float bullet_speed;
} ext_shoot_data_t;

//三、机器人间交互数据
//1. 交互数据接收信息： 0x0301。 发送频率：上限 10Hz
typedef __packed struct
{
uint16_t data_cmd_id;
uint16_t send_ID;
uint16_t receiver_ID;
}ext_student_interactive_header_data_t;

//1. 客户端 客户端自定义数据： cmd_id:0x0301。内容 ID:0xD180。 发送频率：上限 10Hz
//typedef __pack struct
//{
//float data1;
//float data2;
//float data3;
//uint8_t masks;
//} client_custom_data_t

//2. 交互数据 机器人间通信： 0x0301。发送频率：上限 10Hz
typedef __packed struct
{
uint8_t data[30];
} robot_interactive_data_t;


/**
  * @brief  the data structure receive from judgement
  */
typedef struct
{
    ext_game_state_t        game_state; //-1
    ext_game_result_t       game_result_data;//-2
    ext_game_robot_state_t  game_information;   //7. 比赛机器人状态： 0x0201。 发送频率： 10Hz
    ext_power_heat_data_t	power_heat;	        //8. 实时功率热量数据： 0x0202。 发送频率： 50Hz
    ext_game_robot_pos_t    game_robo_pos;//-9
    ext_buff_musk_t         get_buff_data;//-10
    ext_robot_hurt_t        blood_changed_data;//-12
    ext_shoot_data_t        real_shoot_data;   //13
    ext_event_data_t        rfid_data;	//-4
	ext_student_interactive_header_data_t	robot_communicate_id;
	robot_interactive_data_t				robot_communicate;
} receive_judge_t;


/* data send (forward) */
/* data receive */
extern receive_judge_t judge_rece_mesg;

void  judgement_data_handle(uint8_t *p_frame);

#endif
